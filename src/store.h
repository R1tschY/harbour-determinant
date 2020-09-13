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

#ifndef STORE_H
#define STORE_H

#include <QObject>

namespace Det {

class Store : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userName
               READ userName
               WRITE setUserName
               NOTIFY userNameChanged)

    Q_PROPERTY(QString accessToken
               READ accessToken
               WRITE setAccessToken
               NOTIFY accessTokenChanged)

    Q_PROPERTY(QString homeServer
               READ homeServer
               WRITE setHomeServer
               NOTIFY homeServerChanged)
public:
    explicit Store(QObject *parent = nullptr);

    QString userName() const;
    void setUserName(const QString &userName);

    QString accessToken() const;
    void setAccessToken(const QString &accessToken);

    QString homeServer() const;
    void setHomeServer(const QString &homeServer);

signals:
    void userNameChanged();
    void accessTokenChanged();
    void homeServerChanged();

public slots:

private:
    QString m_userName;
    QString m_accessToken;
    QString m_homeServer;
};

} // namespace Det

#endif // STORE_H
