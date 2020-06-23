#include "roomeventsmodel.h"

#include <QLoggingCategory>
#include <QRegularExpression>
#include <QString>
#include <QUrl>

#include <events/redactionevent.h>
#include <events/roomcreateevent.h>
#include <events/roommemberevent.h>
#include <events/roommessageevent.h>
#include <events/roomtombstoneevent.h>
#include <events/simplestateevents.h>
#include <user.h>

#include "messagerenderer.h"

namespace Det {

using namespace Quotient;

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

QVariant RoomEventsModel::data(const QModelIndex& idx, int role) const
{
    if (!m_room || !idx.isValid())
        return QVariant();

    int row = idx.row();
    if (row > m_room->timelineSize() + m_pendingEvents)
        return QVariant();

    if (m_pendingEvents != int(m_room->pendingEvents().size())) {
        qCDebug(logger) << "wrong pendingEvents count";
    }

    bool isPending = row < m_pendingEvents;

    const RoomEvent* evt = nullptr;
    const PendingEventItem* pendingEvt = nullptr;
    const TimelineItem* timelineEvt = nullptr;
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

    MessageRenderer renderer(m_room);

    switch (role) {
    case DisplayRole:
        return renderer.renderEventText(isPending, evt);

    case AuthorRole:
        return QVariant::fromValue(renderer.getAuthor(isPending, evt));

    case AuthorDisplayNameRole:
        return renderer.getAuthorDisplayName(isPending, evt);

    case EventIdRole:
        return evt->id();

    case MatrixTypeRole:
        return evt->matrixType();

    case EventTypeRole:
        if (auto* msgEvt = eventCast<const RoomMessageEvent>(evt)) {
            QString rawMsgtype = msgEvt->rawMsgtype();
            if (!rawMsgtype.isEmpty()) {
                rawMsgtype.remove(0, 2);
                return rawMsgtype;
            }
            if (msgEvt->hasFileContent()) {
                return QStringLiteral("file");
            }
        }
        if (evt->isStateEvent()) {
            return QStringLiteral("state");
        }
        return QStringLiteral("other");

    case EventStatusRole:
        if (isPending)
            return pendingEvt->deliveryStatus();
        return EventStatus::ReachedServer;

    case AnnotationRole:
        if (isPending)
            return pendingEvt->annotation();
        break;

    case DateTimeRole:
        return isPending
            ? pendingEvt->lastUpdated()
            : evt->originTimestamp();

    case TimeRole:
        return isPending
            ? pendingEvt->lastUpdated().time()
            : evt->originTimestamp().time();

    case DateRole:
        return isPending
            ? pendingEvt->lastUpdated().date()
            : evt->originTimestamp().date();

    case EditedRole:
        return isPending
                ? (pendingEvt->deliveryStatus() & EventStatus::Replaced)
                : evt->isReplaced();

    case HiddenRole:
        if (isPending)
            return renderer.isPendingHidden(pendingEvt);
        else
            return renderer.isHidden(evt);

    case ContentTypeRole:
        if (auto msgEvt = eventCast<const RoomMessageEvent>(evt)) {
            return msgEvt->rawMsgtype();
        }
        return QString();

    case ContentJsonRole:
        return evt->contentJson();

    case ReadMarkerRole:
        return m_room->readMarkerEventId() == evt->id();

    case ShowAuthorRole: {
        if (isPending) {
            return true;
        }

        QModelIndex nextRow;
        int rows = m_room->timelineSize() + m_pendingEvents;
        for (auto r = row + 1; r < rows; ++r) {
            nextRow = index(r);
            if (!data(nextRow, HiddenRole).toBool()) {
                break;
            }
        }

        if (!nextRow.isValid()) {
            return true;
        }

        if (data(nextRow, EventTypeRole) == QStringLiteral("state")) {
            return true;
        }

        return data(idx, AuthorRole) != data(nextRow, AuthorRole);
    }
    };

    return QVariant();
}

QHash<int, QByteArray> RoomEventsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(DisplayRole, "display");
    roles.insert(EventIdRole, "eventId");
    roles.insert(MatrixTypeRole, "matrixType");
    roles.insert(EventTypeRole, "eventType");
    roles.insert(AnnotationRole, "annotation");
    roles.insert(EventStatusRole, "eventStatus");
    roles.insert(DateTimeRole, "dateTime");
    roles.insert(TimeRole, "time");
    roles.insert(DateRole, "date");
    roles.insert(EditedRole, "edited");
    roles.insert(HiddenRole, "hidden");
    roles.insert(AuthorRole, "author");
    roles.insert(AuthorDisplayNameRole, "authorDisplayName");
    roles.insert(ContentTypeRole, "contentType");
    roles.insert(ContentJsonRole, "contentJson");
    roles.insert(ReadMarkerRole, "readMarker");
    roles.insert(ShowAuthorRole, "showAuthor");
    return roles;
}

int RoomEventsModel::pendingEventsCount() const
{
    return m_pendingEvents;
}

void RoomEventsModel::onBeginInsertMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    beginInsertRows(
        {}, m_pendingEvents, m_pendingEvents + int(events.size()) - 1);
}

void RoomEventsModel::onBeginInsertOldMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    int size = m_room->timelineSize() + m_pendingEvents;
    beginInsertRows({}, size, size + int(events.size()) - 1);
}

void RoomEventsModel::onBeginSyncMessage(int i)
{
    if (m_pendingEvents - 1 == i) {
        // trival case
        --m_pendingEvents;
        updateRow(m_pendingEvents);
        return;
    }

    Q_ASSERT(i >= 0 && i < m_pendingEvents);
    //Q_ASSERT(m_pendingEvents == m_room->pendingEvents().size());

    m_movingEvents = true;
    int index = m_pendingEvents - (i + 1);
    beginMoveRows({}, index, index, {}, m_pendingEvents - 1);
    --m_pendingEvents;
}

void RoomEventsModel::onEndSyncMessage()
{
    if (m_movingEvents) {
        m_movingEvents = false;
        endMoveRows();
    }

    //Q_ASSERT(m_pendingEvents == m_room->pendingEvents().size());
    updateRow(m_pendingEvents);
}

void RoomEventsModel::onReplacedMessage(
    const RoomEvent* _newEvent, const RoomEvent* oldEvent)
{
    updateEvent(oldEvent->id());
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
        qCWarning(logger) << "Non-existing event" << eventId;
        return -1;
    }

    return std::distance(m_room->messageEvents().rbegin(), iter)
        + m_pendingEvents;
}

Room* RoomEventsModel::room() const
{
    return m_room;
}

void RoomEventsModel::setRoom(Room* room)
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
            this, [this](const RoomEvent*, int i) { onBeginSyncMessage(i); });
        connect(m_room, &Room::pendingEventMerged,
            this, [this]() { onEndSyncMessage(); });
        connect(m_room, &Room::pendingEventChanged,
            this, [this](int i) { updateRow(i); });

        m_pendingEvents = int(m_room->pendingEvents().size());
    } else {
        m_pendingEvents = 0;
    }

    endResetModel();

    emit roomChanged();
}

} // namespace Det
