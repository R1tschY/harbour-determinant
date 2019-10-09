#include "chatsmodel.h"

#include <QDebug>
#include <algorithm>

#include <util.h>
#include "messagerenderer.h"

namespace Det {

using namespace QMatrixClient;
using namespace Det;

ChatsModel::ChatsModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ChatsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_rooms.size();
}

QVariant ChatsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= int(m_rooms.size()))
        return QVariant();

    Room* room = m_rooms[index.row()];
    MessageRenderer renderer(room);

    switch (role) {
    case DisplayNameRole:
        return room->displayName();
    case ImageRole:
        return room->avatarMediaId();
    case UnreadCountRole:
        return room->unreadCount();
    case NotificationsCountRole:
        return room->notificationCount();
    case HighlightsCountRole:
        return room->highlightCount();
    case LastEventRole:
        return renderer.getLastEvent();
    case LastActivityRole:
        return renderer.getLastActivity();
    case RoomRole:
        return QVariant::fromValue(room);
    }

    return QVariant();
}

Connection* ChatsModel::connection() const
{
    return m_connection;
}

void ChatsModel::setConnection(Connection* connection)
{
    if (m_connection == connection)
        return;

    beginResetModel();

    if (m_connection) {
        disconnect(m_connection);

        for (Room* room : m_rooms)
            room->disconnect(this);
        m_rooms.clear();
    }

    m_connection = connection;

    if (m_connection) {
        connect(m_connection, &Connection::loggedOut,
            this, [=] { setConnection(nullptr); });
        connect(m_connection, &Connection::newRoom,
            this, &ChatsModel::onNewRoom);
        connect(m_connection, &Connection::aboutToDeleteRoom,
            this, &ChatsModel::onDeleteRoom);

        qDebug() << connection->roomMap().size() << connection->directChats().size();
        for (Room* room : connection->roomMap())
            addRoom(room);
    }

    endResetModel();

    emit connectionChanged();
}

void ChatsModel::onNewRoom(Room* room)
{
    Q_ASSERT(room != nullptr);
    qDebug() << room->displayName();

    beginInsertRows(QModelIndex(), m_rooms.size(), m_rooms.size());
    addRoom(room);
    endInsertRows();
}

void ChatsModel::onDeleteRoom(Room* room)
{
    qDebug() << room->displayName();

    size_t i = indexOfRoom(room);
    Q_ASSERT(i != m_rooms.size());
    if (i != m_rooms.size()) {
        beginRemoveRows(QModelIndex(), i, i);
        m_rooms.erase(m_rooms.cbegin() + i);
        endRemoveRows();
    }
}

void ChatsModel::onRoomChanged(Room* room, const QVector<int>& roles)
{
    qDebug() << room->displayName();

    size_t i = indexOfRoom(room);
    Q_ASSERT(i != m_rooms.size());
    if (i != m_rooms.size()) {
        emit dataChanged(index(i), index(i), roles);
    }
}

void ChatsModel::onRoomMessage(Room *room)
{
    onRoomChanged(room, { LastEventRole, LastActivityRole });
}

size_t ChatsModel::indexOfRoom(Room* room) const
{
    Q_ASSERT(room != nullptr);

    return std::distance(
                m_rooms.begin(), std::find(m_rooms.begin(), m_rooms.end(), room));
}

void ChatsModel::addRoom(Room* room)
{
    qDebug() << room->displayName();
    Q_ASSERT(room != nullptr);

    if (room) {
        m_rooms.push_back(room);
        connectToRoom(room);
    }
}

void ChatsModel::connectToRoom(Room* room)
{
    qDebug() << room->displayName();
    Q_ASSERT(room != nullptr);

    connect(room, &Room::displaynameChanged,
        this, [=] { onRoomChanged(room, { DisplayNameRole }); });
    //connect(room, &Room::joinStateChanged,
    //        this, [=]{ onRoomChanged(room, {}); });
    connect(room, &Room::avatarChanged,
        this, [=] { onRoomChanged(room, { ImageRole }); });
    connect(room, &Room::unreadMessagesChanged,
        this, [=] { onRoomChanged(room, { UnreadCountRole }); });
    connect(room, &Room::notificationCountChanged,
        this, [=] { onRoomChanged(room, { NotificationsCountRole }); });
    connect(room, &Room::highlightCountChanged,
        this, [=] { onRoomChanged(room, { HighlightsCountRole }); });
    connect(room, &Room::addedMessages,
        this, [=] { onRoomMessage(room); });
    connect(room, &Room::pendingEventMerged,
        this, [=] { onRoomMessage(room); });
    connect(room, &Room::pendingEventChanged,
        this, [=] { onRoomMessage(room); });
    connect(room, &Room::pendingEventAdded,
        this, [=] { onRoomMessage(room); });
}

QHash<int, QByteArray> ChatsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(DisplayNameRole, "displayname");
    roles.insert(ImageRole, "avatar");
    roles.insert(UnreadCountRole, "unreadCount");
    roles.insert(NotificationsCountRole, "notificationCount");
    roles.insert(HighlightsCountRole, "highlightCount");
    roles.insert(LastEventRole, "lastEvent");
    roles.insert(LastActivityRole, "lastActivity");
    roles.insert(RoomRole, "room");
    return roles;
}

} // namespace Det
