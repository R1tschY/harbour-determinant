/*
 * Copyright 2020 Richard Liebscher <richard.liebscher@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    return m_room->timelineSize() + pendingEventCount();
}

QVariant RoomEventsModel::data(const QModelIndex& idx, int role) const
{
    if (!m_room || !idx.isValid())
        return QVariant();

    int row = idx.row();
    int pendingEvents = pendingEventCount();
    if (row > m_room->timelineSize() + pendingEvents)
        return QVariant();


    bool isPending = row < pendingEvents;

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
            m_room->messageEvents().crbegin() + (row - pendingEvents));
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
        QModelIndex nextRow;
        int rows = m_room->timelineSize() + pendingEvents;
        for (auto r = row + 1; r < rows; ++r) {
            nextRow = index(r);
            if (!data(nextRow, HiddenRole).toBool()) {
                break;
            }
        }

        if (!nextRow.isValid() || nextRow.row() >= rows) {
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

void RoomEventsModel::onBeginInsertMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    int pendingEvents = pendingEventCount();
    beginInsertRows(
        {}, pendingEvents, pendingEvents + int(events.size()) - 1);
}

void RoomEventsModel::onBeginInsertOldMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    int size = m_room->timelineSize() + pendingEventCount();
    beginInsertRows({}, size, size + int(events.size()) - 1);
}

void RoomEventsModel::onBeginSyncMessage(int i)
{
    int pendingEvents = pendingEventCount();
    if (pendingEvents - 1 == i) {
        // trival case
        return;
    }

    Q_ASSERT(i >= 0 && i < pendingEvents);

    m_movingEvents = true;
    int index = pendingEvents - (i + 1);
    beginMoveRows({}, index, index, {}, pendingEvents - 1);
}

void RoomEventsModel::onEndSyncMessage()
{
    if (m_movingEvents) {
        m_movingEvents = false;
        endMoveRows();
    }

    int pendingEvents = pendingEventCount();

    //Q_ASSERT(m_pendingEvents == m_room->pendingEvents().size());
    updateRow(pendingEvents);
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
            + pendingEventCount();
}

Room* RoomEventsModel::room() const
{
    return m_room;
}

bool RoomEventsModel::fetching() const
{
    if (m_room) {
        return isJobRunning(m_room->eventsHistoryJob());
    } else {
        return false;
    }
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
            this, [this] { beginInsertRows({}, 0, 0); });
        connect(m_room, &Room::pendingEventAdded,
            this, [this] { endInsertRows(); });
        connect(m_room, &Room::pendingEventAboutToDiscard,
            this, [this](int i) { beginRemoveRows({}, i, i); });
        connect(m_room, &Room::pendingEventDiscarded,
            this, [this] { endRemoveRows(); });
        connect(m_room, &Room::pendingEventAboutToMerge,
            this, [this](const RoomEvent*, int i) { onBeginSyncMessage(i); });
        connect(m_room, &Room::pendingEventMerged,
            this, [this]() { onEndSyncMessage(); });
        connect(m_room, &Room::pendingEventChanged,
            this, [this](int i) { updateRow(i); });

        connect(m_room, &Room::eventsHistoryJobChanged,
            this, [this]() { emit fetchingChanged(); });
    }

    endResetModel();

    emit roomChanged();
}

} // namespace Det
