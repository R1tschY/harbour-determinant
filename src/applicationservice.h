#ifndef DET_DBUSAPPLICATIONSERVICE_H
#define DET_DBUSAPPLICATIONSERVICE_H

#include <QObject>

namespace Det {

extern const QString DBUS_SERVICE_NAME;
extern const QString DBUS_APPLICATION_INTERFACE;
extern const QString DBUS_APPLICATION_PATH;

class DBusApplicationService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "de.richardliebscher.determinant.Application")

public:
    explicit DBusApplicationService(QObject *parent = nullptr);

    void registerOnDbus();
    static bool deduplicate();

public slots:
    Q_SCRIPTABLE void activateRoom(const QString& roomId);
    Q_SCRIPTABLE void markRoomAsRead(const QString& roomId);
    Q_SCRIPTABLE void raise();

signals:
    void activateRoomInternal(const QString& roomId);
    void markRoomAsReadInternal(const QString& roomId);
    void raiseInternal();
};

} // namespace Det

#endif // DET_DBUSAPPLICATIONSERVICE_H
