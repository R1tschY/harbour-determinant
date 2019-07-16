#ifndef DETERMINANT_CHATSMODEL_H
#define DETERMINANT_CHATSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <vector>

#include <connection.h>
#include <room.h>

namespace Determinant {

class ChatsModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(
        QMatrixClient::Connection* connection
            READ connection WRITE setConnection
                NOTIFY connectionChanged)

    enum {
        DisplayNameRole = Qt::DisplayRole,
        ImageRole = Qt::DecorationRole,
        UnreadCountRole = Qt::UserRole,
        NotificationsCountRole,
        HighlightsCountRole,
        RoomRole,
    };

public:
    explicit ChatsModel(QObject* parent = nullptr);

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

    size_t indexOfRoom(QMatrixClient::Room* room) const;

    std::vector<QMatrixClient::Room*> m_rooms;
};

} // namespace Determinant

#endif // DETERMINANT_CHATSMODEL_H
