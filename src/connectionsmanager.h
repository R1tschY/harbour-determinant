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

#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#include <QObject>
#include <QVector>
#include <connection.h>

namespace Det {

class SecretsService;

class ConnectionsManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectionsManager(
        SecretsService* secretsService, QObject* parent = nullptr);
    ~ConnectionsManager();

    /**
     * @brief logins in if login data exists
     */
    Q_SCRIPTABLE void load();

    Q_SCRIPTABLE void login(const QString& userId,
        const QString& password,
        const QString& homeServer);

    Q_SCRIPTABLE void logout();

    Q_SCRIPTABLE Quotient::Connection* connection()
    {
        return &m_connection;
    }

signals:
    void error(const QString& message);

private:
    Quotient::Connection m_connection;
    SecretsService* m_secretsService = nullptr;
    int m_syncTimeout = 120 * 1000;
    int m_syncCounter = 0;

    void onLoginError(const QString& message, const QString& details);
    void onConnected();
    void onLoggedOut();
    void onSyncDone();
    void onSyncError(QString message, QString details);

    void saveLogin();
};

} // namespace Det

#endif // CONNECTIONSMANAGER_H
