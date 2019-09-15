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

    QMatrixClient::Room *room() const;
    void setRoom(QMatrixClient::Room *room);

private:
    QMatrixClient::Room* m_room;
    QList<QMatrixClient::User*> m_users;

    void onUserAdded(QMatrixClient::User* user);
    void onUserRemoved(QMatrixClient::User *user);
    void onUserRenamed(QMatrixClient::User *user);

    void sendDataChange(QMatrixClient::User *user, const QVector<int> &roles);
};

} // namespace Det

#endif // DET_USERLISTMODEL_H
