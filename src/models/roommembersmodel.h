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

#ifndef DET_USERLISTMODEL_H
#define DET_USERLISTMODEL_H

#include <QList>
#include <QAbstractListModel>
#include <room.h>
#include <user.h>

namespace Det {

class RoomMembersModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole,
        MemberNameRole,
        JoinStateRole,
        UserIdRole,
        AvatarRole,
        ObjectRole
    };

    explicit RoomMembersModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

    Quotient::Room *room() const;
    void setRoom(Quotient::Room *room);

private:
    Quotient::Room* m_room;
    QList<Quotient::User*> m_users;

    void onUserAdded(Quotient::User* user);
    void onUserRemoved(Quotient::User *user);
    void onUserRenamed(Quotient::User *user);

    void sendDataChange(Quotient::User *user, const QVector<int> &roles);
};

} // namespace Det

#endif // DET_USERLISTMODEL_H
