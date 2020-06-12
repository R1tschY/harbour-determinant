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
