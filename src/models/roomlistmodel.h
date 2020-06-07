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
        QMatrixClient::Connection* connection
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

    QMatrixClient::Connection* connection() const;
    void setConnection(QMatrixClient::Connection* connection);

signals:
    void connectionChanged();

private:
    QMatrixClient::Connection* m_connection = nullptr;

    void addRoom(QMatrixClient::Room* room);
    void connectToRoom(QMatrixClient::Room* room);

    void onNewRoom(QMatrixClient::Room* room);
    void onDeleteRoom(QMatrixClient::Room* room);
    void onRoomChanged(QMatrixClient::Room* room, const QVector<int>& roles);

    void onRoomMessage(QMatrixClient::Room* room);

    int indexOfRoom(QMatrixClient::Room* room) const;

    std::vector<QMatrixClient::Room*> m_rooms;
    int rows() const { return int(m_rooms.size()); }
};

} // namespace Det

#endif // DETERMINANT_CHATSMODEL_H
