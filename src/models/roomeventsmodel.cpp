#include "roomeventsmodel.h"

#include <QLoggingCategory>

#include <events/redactionevent.h>
#include <user.h>

namespace Determinant {

using namespace QMatrixClient;

static Q_LOGGING_CATEGORY(logger, "determinant.roommodel")

    //
    // Model:
    //                    t <---|
    // +---------+--------------+
    // | Pending |   Messages   |
    // +---------+--------------+
    // Not merged New        Old
    //            maxTimelineIndex
    //                       minTimelineIndex
    //

    RoomEventsModel::RoomEventsModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int RoomEventsModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (!m_room || parent.isValid())
        return 0;

    return m_room->timelineSize() + m_pendingEvents;
}

QVariant RoomEventsModel::data(const QModelIndex& index, int role) const
{
    if (!m_room || !index.isValid())
        return QVariant();

    int row = index.row();
    if (row > m_room->timelineSize() + m_pendingEvents)
        return QVariant();

    bool isPending = row < m_pendingEvents;

    const RoomEvent* evt;
    const PendingEventItem* pendingEvt;
    const TimelineItem* timelineEvt;
    if (isPending) {
        auto& evtRef = *(
            m_room->pendingEvents().crbegin() + row);
        pendingEvt = &evtRef;
        evt = evtRef.get();
    } else {
        auto& evtRef = *(
            m_room->messageEvents().crbegin() + (row - m_pendingEvents));
        timelineEvt = &evtRef;
        evt = evtRef.get();
    }

    switch (role) {
    case DisplayRole:
        return renderMessageText(evt);

    case AuthorRole:
        return QVariant::fromValue(
            isPending ? m_room->localUser() : m_room->user(evt->senderId()));

    case EventIdRole:
        break;

    case AnnotationRole:
        if (isPending)
            return pendingEvt->annotation();
        break;

    case TimeRole:
        return isPending
            ? pendingEvt->lastUpdated()
            : evt->timestamp();

    case HiddenRole:
        if (isPending)
            return pendingEvt->deliveryStatus() & EventStatus::Hidden;
        return false;
    }

    return QVariant();
}

QHash<int, QByteArray> RoomEventsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(DisplayRole, "display");
    roles.insert(EventIdRole, "eventId");
    roles.insert(AnnotationRole, "annotation");
    roles.insert(TimeRole, "time");
    roles.insert(HiddenRole, "hidden");
    roles.insert(AuthorRole, "author");
    return roles;
}

QVariant RoomEventsModel::renderMessageText(const RoomEvent* event) const
{
    if (event->isRedacted()) {
        QString reason = event->redactedBecause()->reason();
        if (reason.isEmpty())
            return tr("Redacted");
        else
            return tr("Redacted: %1").arg(reason);
    }

    return visit(
        *event,
        [this](const RoomMessageEvent& evt) {
            return evt.plainBody();
        },
        tr("Unknown event"));
}

int RoomEventsModel::pendingEventsCount() const
{
    return m_pendingEvents;
}

void RoomEventsModel::onBeginInsertMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    beginInsertRows({}, m_pendingEvents, m_pendingEvents + events.size() - 1);
}

void RoomEventsModel::onBeginInsertOldMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    int size = m_room->timelineSize() + m_pendingEvents;
    beginInsertRows({}, size, size + events.size() - 1);
}

void RoomEventsModel::onBeginSyncMessage(int i)
{
    if (i == 0)
        return;

    m_movingEvents = true;
    int index = m_pendingEvents - (i + 1);
    beginMoveRows({}, index, index, {}, m_movingEvents);
    --m_pendingEvents;
}

void RoomEventsModel::onEndSyncMessage()
{
    if (m_movingEvents) {
        m_movingEvents = false;
        endMoveRows();
    }

    updateRow(m_pendingEvents);
}

void RoomEventsModel::onReplacedMessage(
    const RoomEvent* newEvent, const RoomEvent* oldEvent)
{
    Q_ASSERT(newEvent->id() == oldEvent->id());
}

void RoomEventsModel::updateRow(int row, const QVector<int>& roles)
{
    auto modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex, roles);
}

void RoomEventsModel::updateRow(int row)
{
    auto modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex);
}

void RoomEventsModel::updateEvent(const QString& eventId)
{
    updateRow(findEvent(eventId));
}

int RoomEventsModel::findEvent(const QString& eventId)
{
    auto iter = m_room->findInTimeline(eventId);
    if (iter == m_room->timelineEdge()) {
        qCWarning(logger) << "Non-existing event" << eventId << "replaced";
        return -1;
    }

    return std::distance(m_room->messageEvents().rbegin(), iter)
        + m_pendingEvents;
}

Room* RoomEventsModel::room() const
{
    return m_room;
}

void RoomEventsModel::setRoom(QMatrixClient::Room* room)
{
    if (m_room == room)
        return;

    beginResetModel();

    if (m_room) {
        m_room->disconnect(this);
    }

    m_room = room;

    if (m_room) {
        connect(m_room, &Room::destroyed, this,
            [this]() {
                Q_ASSERT_X(false, "Room::destroyed", "Room was destroyed. Should not happen!");
                setRoom(nullptr);
            });

        connect(m_room, &Room::aboutToAddNewMessages,
            this, &RoomEventsModel::onBeginInsertMessages);
        connect(m_room, &Room::aboutToAddHistoricalMessages,
            this, &RoomEventsModel::onBeginInsertOldMessages);
        connect(m_room, &Room::addedMessages,
            this, [this]() { endInsertRows(); });
        connect(m_room, &Room::replacedEvent,
            this, &RoomEventsModel::onReplacedMessage);

        connect(m_room, &Room::pendingEventAboutToAdd,
            this, [this] { beginInsertRows({}, 0, 0); ++m_pendingEvents; });
        connect(m_room, &Room::pendingEventAdded,
            this, [this] { endInsertRows(); });
        connect(m_room, &Room::pendingEventAboutToDiscard, this,
            [this](int i) { beginRemoveRows({}, i, i); --m_pendingEvents; });
        connect(m_room, &Room::pendingEventDiscarded,
            this, [this] { endRemoveRows(); });
        connect(m_room, &Room::pendingEventAboutToMerge,
            this, [this](RoomEvent*, int i) { onBeginSyncMessage(i); });
        connect(m_room, &Room::pendingEventMerged,
            this, [this]() { onEndSyncMessage(); });
        connect(m_room, &Room::pendingEventChanged,
            this, [this](int i) { updateRow(i); });

        m_pendingEvents = m_room->pendingEvents().size();
    } else {
        m_pendingEvents = 0;
    }

    endResetModel();

    emit roomChanged();
}

} // namespace Determinant
