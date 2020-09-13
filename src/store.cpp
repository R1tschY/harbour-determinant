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

#include "store.h"

namespace Det {

Store::Store(QObject *parent) : QObject(parent)
{

}

QString Store::userName() const
{
    return m_userName;
}

void Store::setUserName(const QString &userName)
{
    m_userName = userName;
}

QString Store::accessToken() const
{
    return m_accessToken;
}

void Store::setAccessToken(const QString &accessToken)
{
    m_accessToken = accessToken;
}

QString Store::homeServer() const
{
    return m_homeServer;
}

void Store::setHomeServer(const QString &homeServer)
{
    m_homeServer = homeServer;
}

} // namespace Det
