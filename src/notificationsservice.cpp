#include "notificationsservice.h"

#include <QLoggingCategory>

#include <notification.h>
#include <connection.h>
#include <room.h>

#include "applicationservice.h"

namespace Det {

using namespace Quotient;

static Q_LOGGING_CATEGORY(logger, "determinant.notificationsservice");

static const QString ROOM_HINT = QStringLiteral("x-determinat-room-id");

NotificationsService::~NotificationsService() = default;

NotificationsService::NotificationsService(Connection *connection)
    : QObject(connection)
{
    m_appRemoteAction = Notification::remoteAction(
                QStringLiteral("app"), QStringLiteral("raise app"),
                DBUS_SERVICE_NAME, DBUS_APPLICATION_PATH, DBUS_APPLICATION_INTERFACE,
                QStringLiteral("raise"), { });

    connect(connection, &Connection::newRoom,
            this, &NotificationsService::onNewRoom);
    connect(connection, &Connection::leftRoom,
            this, &NotificationsService::onRoomLeft);

    for (auto n : Notification::notifications()) {
        std::unique_ptr<Notification> notification { qobject_cast<Notification*>(n) };

        QString roomId = notification->hintValue(ROOM_HINT).toString();
        if (!roomId.isEmpty()) {
            Notification* n = insert(roomId, std::move(notification));
            n->publish(); // publish possible upgrades
        }
    }

    for (auto room : connection->allRooms()) {
        onNewRoom(room);
    }
}

void NotificationsService::onNewRoom(Room *room)
{
    connect(
        room, &Room::notificationCountChanged,
        this, &NotificationsService::onNotificationCountChanged);
    updateNotification(room);
}

void NotificationsService::onRoomLeft(Room *room, Room*)
{
    auto iter = m_notifications.find(room->id());
    if (iter != m_notifications.end()) {
        Notification* notification = iter->second.get();
        notification->close();
        m_notifications.erase(iter);
    }
}

void NotificationsService::onNotificationCountChanged()
{
    Room* room = qobject_cast<Room*>(sender());
    Q_ASSERT(room != nullptr);
    if (room == nullptr)
        return;

    updateNotification(room);
}

void NotificationsService::onNotificationClicked()
{
    Notification* notification =  qobject_cast<Notification*>(sender());
    Q_ASSERT(notification != nullptr);
    if (notification == nullptr)
        return;

    QString roomId = notification->hintValue(ROOM_HINT).toString();
    if (!roomId.isEmpty()) {
        emit openRoomWithId(roomId);
    }
}

void NotificationsService::updateNotification(Room *room)
{
    int notificationCount = room->notificationCount();

    Notification* notification;
    auto iter = m_notifications.find(room->id());
    if (iter != m_notifications.end()) {
        notification = iter->second.get();

        if (notificationCount == 0 || room->displayed()) {
            notification->close();
            m_notifications.erase(iter);
            return;
        }
    } else {
        if (notificationCount == 0 || room->displayed()) {
            return;
        }

        notification = new Notification(this);
        notification->setHintValue(ROOM_HINT, room->id());
        insert(room->id(), std::unique_ptr<Notification>(notification));
    }

    QString body = tr("%1 messages", nullptr, room->notificationCount())
            .arg(room->notificationCount());
    QString summary = room->displayName();

    // TODO: setTimestamp with last notification event
    // TODO: display text for last message in summary
    notification->setPreviewSummary(summary);
    notification->setPreviewBody(body);
    notification->setSummary(summary);
    notification->setBody(body);
    notification->setItemCount(room->notificationCount());
    notification->publish();
}

Notification* NotificationsService::insert(
        const QString& roomId, std::unique_ptr<Notification> notification)
{
    // update old or new notifications
    notification->setAppName(QStringLiteral("Determinant"));
    notification->setRemoteActions({
        Notification::remoteAction(
            QStringLiteral("default"), QStringLiteral("activate"),
            DBUS_SERVICE_NAME, DBUS_APPLICATION_PATH, DBUS_APPLICATION_INTERFACE,
            QStringLiteral("activateRoom"), { roomId }),
       m_appRemoteAction
    });

    Notification* result = notification.get();
    m_notifications.emplace(roomId, std::move(notification));
    return result;
}

} // namespace Det
