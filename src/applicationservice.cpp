#include "applicationservice.h"

#include <QDBusConnection>
#include <QLoggingCategory>
#include <QDBusInterface>
#include <QDBusError>

namespace Det {

const QString DBUS_SERVICE_NAME =
        QStringLiteral("de.richardliebscher.Determinant");
const QString DBUS_APPLICATION_INTERFACE =
        QStringLiteral("de.richardliebscher.determinant.Application");
const QString DBUS_APPLICATION_PATH =
        QStringLiteral("/de/richardliebscher/determinant/Application");

static Q_LOGGING_CATEGORY(logger, "determinant.notificationsservice");

DBusApplicationService::DBusApplicationService(QObject *parent) : QObject(parent)
{

}

void DBusApplicationService::registerOnDbus()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerObject(
            DBUS_APPLICATION_PATH,
            this,
            QDBusConnection::ExportScriptableSlots)) {
        qCCritical(logger)
            << "Registering" << DBUS_APPLICATION_INTERFACE
            << "on" << DBUS_APPLICATION_PATH
            << "failed:"
            << sessionBus.lastError();
    }
}

bool DBusApplicationService::deduplicate()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService(DBUS_SERVICE_NAME)) {
        qCInfo(logger) << "Other application instance is already running.";

        QDBusInterface remoteApp(
            DBUS_SERVICE_NAME, DBUS_APPLICATION_PATH, DBUS_APPLICATION_INTERFACE, sessionBus);
        remoteApp.call(QStringLiteral("raise"));
        return true;
    }

    return false;
}

void DBusApplicationService::activateRoom(const QString &roomId)
{
    emit activateRoomInternal(roomId);
}

void DBusApplicationService::markRoomAsRead(const QString &roomId)
{
    emit markRoomAsReadInternal(roomId);
}

void DBusApplicationService::raise()
{
    emit raiseInternal();
}

} // namespace Det
