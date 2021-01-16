#ifndef DET_NOTIFICATIONSSERVICE_H
#define DET_NOTIFICATIONSSERVICE_H

#include <QObject>
#include <QHash>
#include <QString>
#include <unordered_map>
#include <memory>

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

    std::unordered_map<QString, std::unique_ptr<Notification>> m_notifications;
};

} // namespace Det

#endif // DET_NOTIFICATIONSSERVICE_H
