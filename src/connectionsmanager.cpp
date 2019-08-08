#include "connectionsmanager.h"

#include <QLoggingCategory>
#include <QSettings>

namespace Determinant {
using namespace QMatrixClient;

static Q_LOGGING_CATEGORY(logger, "determinant.conectionsmanager");

static QString DEFAULT_HOME_SERVER = QStringLiteral("https://matrix.org");

ConnectionsManager::ConnectionsManager(QObject* parent)
    : QObject(parent)
{
    connect(
        &m_connection, &Connection::connected,
        this, &ConnectionsManager::onConnected);

    connect(
        &m_connection, &Connection::syncDone,
        this, &ConnectionsManager::onSyncDone);

    connect(
        &m_connection, &Connection::syncError,
        this, &ConnectionsManager::onSyncError);

    connect(&m_connection, &Connection::loggedOut, this, [] {
        qCDebug(logger) << "Log out";
    });
    connect(&m_connection, &Connection::loginError, this, [](QString message, QString details) {
        qCCritical(logger) << "Login error:" << message << "/" << details;
    });

    connect(&m_connection, &Connection::networkError, this, [](QString message, QString details, int retriesTaken, int nextRetryInMilliseconds) {
        qCCritical(logger) << "Login error:" << message << "/" << details;
    });
}

void ConnectionsManager::load()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("login"));

    QUrl server = settings.value(QStringLiteral("server")).toUrl();
    QString accessToken = settings.value(QStringLiteral("accessToken")).toString();
    QString userId = settings.value(QStringLiteral("userId")).toString();
    QString deviceId = settings.value(QStringLiteral("deviceId")).toString();
    if (!server.isEmpty() && !accessToken.isEmpty() && !userId.isEmpty()) {
        qCInfo(logger) << "Login with token";
        m_connection.setHomeserver(server);
        m_connection.connectWithToken(userId, accessToken, deviceId);
        return;
    }

    if (!server.isEmpty() || !accessToken.isEmpty() || !userId.isEmpty()) {
        qCWarning(logger) << "stored login information incomplete";
    } else {
        qCDebug(logger) << "No login information";
    }
}

void ConnectionsManager::login(
    const QString& userId,
    const QString& password,
    const QString& homeServer)
{
    if (userId.isEmpty()) {
        qCCritical(logger) << "user id for login is empty";
    }

    m_connection.setHomeserver(
        homeServer.isEmpty() ? DEFAULT_HOME_SERVER : homeServer);

    m_connection.connectToServer(
        userId, password, m_connection.deviceId());
}

void ConnectionsManager::onLoginError(
    const QString& message, const QString& details)
{
    emit error(tr("Login error: %1").arg(message));
}

void ConnectionsManager::onConnected()
{
    qCDebug(logger) << "Connected";

    saveLogin();

    m_connection.loadState();

    sync();
}

void ConnectionsManager::onSyncDone()
{
    qCDebug(logger) << "Sync done";

    sync();
}

void ConnectionsManager::onSyncError(QString message, QString details)
{
    qCCritical(logger) << "Sync error:" << message << "/" << details;
}

void ConnectionsManager::saveLogin()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("login"));

    settings.setValue(QStringLiteral("server"), m_connection.homeserver());
    settings.setValue(QStringLiteral("accessToken"), m_connection.accessToken());
    settings.setValue(QStringLiteral("userId"), m_connection.userId());
    settings.setValue(QStringLiteral("deviceId"), m_connection.deviceId());
    settings.sync();
}

void ConnectionsManager::sync()
{
    ++m_syncCounter;

    m_connection.sync(m_syncTimeout);

    if ((m_syncCounter % 16) == 2) {
        qCDebug(logger) << "Save state to" << m_connection.stateCachePath();
        m_connection.saveState();
    }
}

} // namespace Determinant
