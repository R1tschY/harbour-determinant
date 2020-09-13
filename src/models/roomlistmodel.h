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

#ifndef DETERMINANT_CHATSMODEL_H
#define DETERMINANT_CHATSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <vector>

#include <connection.h>
#include <room.h>

namespace Det {

class RoomListModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(
        Quotient::Connection* connection
            READ connection WRITE setConnection
                NOTIFY connectionChanged)

    enum {
        DisplayNameRole = Qt::DisplayRole,
        AvatarRole = Qt::DecorationRole,
        UnreadCountRole = Qt::UserRole,
        NotificationsCountRole,
        HighlightsCountRole,
        LastEventRole,
        LastActivityRole,
        RoomRole,
        RoomIdRole,
    };

public:
    explicit RoomListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(
        const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Quotient::Connection* connection() const;
    void setConnection(Quotient::Connection* connection);

signals:
    void connectionChanged();

private:
    Quotient::Connection* m_connection = nullptr;

    void addRoom(Quotient::Room* room);
    void connectToRoom(Quotient::Room* room);

    void onNewRoom(Quotient::Room* room);
    void onDeleteRoom(Quotient::Room* room);
    void onRoomChanged(Quotient::Room* room, const QVector<int>& roles);

    void onRoomMessage(Quotient::Room* room);

    int indexOfRoom(Quotient::Room* room) const;

    std::vector<Quotient::Room*> m_rooms;
    int rows() const { return int(m_rooms.size()); }
};

} // namespace Det

#endif // DETERMINANT_CHATSMODEL_H
