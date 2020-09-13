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
        Quotient::Connection* connection
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

    Quotient::Connection* connection() const { return m_connection; }
    void setConnection(Quotient::Connection* connection);

public slots:
    void resetError();

signals:
    void serverChanged();
    void errorChanged();
    void connectionChanged();
    void fetchingChanged();
    void totalRoomCountEstimateChanged();

private:
    std::deque<Quotient::PublicRoomsChunk> m_entries;
    Quotient::QueryPublicRoomsJob* m_job = nullptr;
    QString m_nextBatch;
    int m_totalRoomCountEstimate = -1;
    QString m_error;

    QString m_server;
    Quotient::Connection* m_connection;

    void abandonJob();
    void resetModel();
    void setError(const QString& error);
    void setTotalRoomCountEstimate(int value);

    void onFetched();
    void onJobEnd();
};

} // namespace Det

#endif // DET_PUBLICROOMLISTMODEL_H
