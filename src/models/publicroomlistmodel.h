#ifndef DET_PUBLICROOMLISTMODEL_H
#define DET_PUBLICROOMLISTMODEL_H

#include <deque>

#include <QAbstractListModel>
#include <QScopedPointer>

#include <connection.h>
#include <csapi/definitions/public_rooms_response.h>
#include <csapi/list_public_rooms.h>

namespace Det {

class PublicRoomListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(
        QMatrixClient::Connection* connection
            READ connection WRITE setConnection
                NOTIFY connectionChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)

public:
    enum Role {
        AliasesRole = Qt::UserRole,
        CanonicalAliasRole,
        NameRole,
        NumJoinedMembersRole,
        RoomIdRole,
        TopicRole,
        WorldReadableRole,
        GuestCanJoinRole,
        AvatarMediaIdRole,
    };

    static constexpr int LIMIT = 25;

    explicit PublicRoomListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void fetchMore(const QModelIndex& parent) override;
    bool canFetchMore(const QModelIndex& parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString server() const { return m_server; }
    void setServer(const QString& server);

    QString error() const { return m_error; }

    bool fetching() const { return m_job != nullptr; }

    QMatrixClient::Connection* connection() const { return m_connection; }
    void setConnection(QMatrixClient::Connection* connection);

public slots:
    void resetError();

signals:
    void serverChanged();
    void errorChanged();
    void connectionChanged();
    void fetchingChanged();
    void totalRoomCountEstimateChanged();

private:
    std::deque<QMatrixClient::PublicRoomsChunk> m_entries;
    QMatrixClient::QueryPublicRoomsJob* m_job = nullptr;
    QString m_nextBatch;
    int m_totalRoomCountEstimate = -1;
    QString m_error;

    QString m_server;
    QMatrixClient::Connection* m_connection;

    void abandonJob();
    void resetModel();
    void setError(const QString& error);
    void setTotalRoomCountEstimate(int value);

    void onFetched();
    void onJobEnd();
};

} // namespace Det

#endif // DET_PUBLICROOMLISTMODEL_H
