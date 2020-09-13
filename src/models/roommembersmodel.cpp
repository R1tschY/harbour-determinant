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

#include "roommembersmodel.h"

namespace Det {

using namespace Quotient;

RoomMembersModel::RoomMembersModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int RoomMembersModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_users.size();
}

QVariant RoomMembersModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_users.size())
        return QVariant();

    User* user = m_users[row];

    switch (role) {
    case NameRole:
        return user->name(m_room);

    case MemberNameRole:
        return m_room->roomMembername(user);

    case JoinStateRole:
        return int(m_room->memberJoinState(user));

    case AvatarRole:
        return user->avatarMediaId(m_room);

    case UserIdRole:
        return user->id();

    case ObjectRole:
        return QVariant::fromValue(user);
    }

    return QVariant();
}

QHash<int, QByteArray> RoomMembersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(MemberNameRole, "memberName");
    roles.insert(JoinStateRole, "joinState");
    roles.insert(AvatarRole, "avatar");
    roles.insert(UserIdRole, "userId");
    roles.insert(ObjectRole, "object");
    return roles;
}

Room* RoomMembersModel::room() const
{
    return m_room;
}

void RoomMembersModel::setRoom(Room* room)
{
    if (m_room == room)
        return;

    beginResetModel();

    if (m_room) {
        m_room->disconnect(this);
    }
    m_users.clear();

    m_room = room;

    if (m_room) {
        connect(m_room, &Room::userAdded, this, &RoomMembersModel::onUserAdded);
        connect(m_room, &Room::userRemoved, this, &RoomMembersModel::onUserRemoved);
        connect(m_room, &Room::memberRenamed,
            this, [this](User* usr) { sendDataChange(usr, { NameRole }); });
        // TODO: AvatarChanged is missing
        // TODO: MemberNameChanged is missing
        // TODO: JoinStateChanged is missing

        m_users = m_room->users();
    }

    endResetModel();
}

void RoomMembersModel::onUserAdded(User* user)
{
    beginInsertRows(QModelIndex(), m_users.size(), m_users.size());
    m_users.append(user);
    endInsertRows();
}

void RoomMembersModel::onUserRemoved(User* user)
{
    int index = m_users.indexOf(user);
    if (index < 0)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_users.removeAt(index);
    endRemoveRows();
}

void RoomMembersModel::onUserRenamed(User* user)
{
    sendDataChange(user, { NameRole });
}

void RoomMembersModel::sendDataChange(User* user, const QVector<int>& roles)
{
    int row = m_users.indexOf(user);
    if (row < 0)
        return;

    auto rowIndex = index(row);
    emit dataChanged(rowIndex, rowIndex, roles);
}

} // namespace Det
