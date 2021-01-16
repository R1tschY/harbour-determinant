#ifndef DET_NOTIFICATIONSSERVICE_H
#define DET_NOTIFICATIONSSERVICE_H

#include <QObject>
#include <QHash>
#include <QString>
#include <unordered_map>
#include <memory>
#include <QVariant>

#include "stdhash.h"

class Notification;

namespace Quotient {
class Connection;
class Room;
}

namespace Det {

class NotificationsService : public QObject
{
    Q_OBJECT
public:
    explicit NotificationsService(Quotient::Connection *connection);
    ~NotificationsService();

    Notification* createNotification();


signals:
    void openRoomWithId(const QString& roomId);

private:
    void onNewRoom(Quotient::Room* room);
    void onRoomLeft(Quotient::Room* room, Quotient::Room* prev);
    void onNotificationCountChanged();
    void onNotificationClicked();

    void updateNotification(Quotient::Room *room);
    Notification* insert(const QString& roomId, std::unique_ptr<Notification> notification);

    std::unordered_map<QString, std::unique_ptr<Notification>> m_notifications;
    QVariant m_appRemoteAction;
};

} // namespace Det

#endif // DET_NOTIFICATIONSSERVICE_H
