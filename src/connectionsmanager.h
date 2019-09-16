#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#include <QObject>
#include <QVector>
#include <connection.h>

namespace Determinant {

class ConnectionsManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectionsManager(QObject* parent = nullptr);

    /**
     * @brief logins in if login data exists
     */
    Q_SCRIPTABLE void load();

    Q_SCRIPTABLE void login(const QString& userId,
        const QString& password,
        const QString& homeServer);

    Q_SCRIPTABLE QMatrixClient::Connection* connection()
    {
        return &m_connection;
    }

signals:
    void error(const QString& message);

private:
    QMatrixClient::Connection m_connection;
    int m_syncTimeout = 30 * 1000;
    int m_syncCounter = 0;

    void onLoginError(const QString& message, const QString& details);
    void onConnected();
    void onSyncDone();
    void onSyncError(QString message, QString details);

    void saveLogin();
    void sync();
};

} // namespace Determinant

#endif // CONNECTIONSMANAGER_H