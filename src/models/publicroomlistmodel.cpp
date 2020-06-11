#include "publicroomlistmodel.h"

#include <QLoggingCategory>
#include <QStringBuilder>

namespace Det {

using namespace QMatrixClient;

static Q_LOGGING_CATEGORY(logger, "Det::PublicRoomListModel");

static QString UNKNOWN_NEXT_BATCH = QStringLiteral("");

static QString toMediaId(const QUrl& url)
{
    if (url.isEmpty()) {
        return QString();
    }
    if (url.scheme() != "mxc") {
        qCDebug(logger) << "Cannot can convert" << url << "to media id";
        return QString();
    }
    return url.host() % url.path();
}

PublicRoomListModel::PublicRoomListModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_nextBatch(UNKNOWN_NEXT_BATCH)
{
}

int PublicRoomListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return int(m_entries.size());
}

QVariant PublicRoomListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    size_t row = size_t(index.row());
    if (row >= m_entries.size())
        return QVariant();

    auto& entry = m_entries[row];
    switch (role) {
    case AliasesRole:
        return entry.aliases;
    case CanonicalAliasRole:
        return entry.canonicalAlias;
    case NameRole:
        return entry.name;
    case NumJoinedMembersRole:
        return entry.numJoinedMembers;
    case RoomIdRole:
        return entry.roomId;
    case TopicRole:
        return entry.topic;
    case WorldReadableRole:
        return entry.worldReadable;
    case GuestCanJoinRole:
        return entry.guestCanJoin;
    case AvatarMediaIdRole:
        return toMediaId(entry.avatarUrl);
    }
    return QVariant();
}

void PublicRoomListModel::fetchMore(const QModelIndex& parent)
{
    if (!canFetchMore(parent) || m_job != nullptr)
        return;

    Q_ASSERT(m_connection != nullptr);
    m_job = m_connection->callApi<QueryPublicRoomsJob>(
        ForegroundRequest, m_server, LIMIT, m_nextBatch);
    connect(m_job, &BaseJob::destroyed,
        this, &PublicRoomListModel::onJobEnd);
    connect(m_job, &BaseJob::success,
        this, &PublicRoomListModel::onFetched);
    emit fetchingChanged();
}

bool PublicRoomListModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return !m_nextBatch.isNull() && m_error.isEmpty() && m_connection;
}

QHash<int, QByteArray> PublicRoomListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(AliasesRole, "alias");
    roles.insert(CanonicalAliasRole, "canonicalAlias");
    roles.insert(NameRole, "name");
    roles.insert(NumJoinedMembersRole, "numJoinedMembers");
    roles.insert(RoomIdRole, "roomId");
    roles.insert(TopicRole, "topic");
    roles.insert(WorldReadableRole, "worldReadable");
    roles.insert(GuestCanJoinRole, "guestCanJoin");
    roles.insert(AvatarMediaIdRole, "avatarMediaId");
    return roles;
}

void PublicRoomListModel::setServer(const QString& server)
{
    if (m_server == server)
        return;

    m_server = server;
    resetModel();
    emit serverChanged();
}

void PublicRoomListModel::resetError()
{
    setError(QString());
}

void PublicRoomListModel::setConnection(QMatrixClient::Connection* connection)
{
    if (m_connection == connection)
        return;

    m_connection = connection;
    resetModel();
    emit connectionChanged();
}

void PublicRoomListModel::setError(const QString& error)
{
    if (m_error == error)
        return;

    m_error = error;
    emit errorChanged();
}

template <typename T>
static T unwrap_or(Omittable<T> t, T default_)
{
    if (t.omitted()) {
        return default_;
    } else {
        return std::move(t.value());
    }
}

void PublicRoomListModel::onFetched()
{
    const PublicRoomsResponse& data = m_job->data();

    int oldSize = int(m_entries.size());
    int newEntries = int(data.chunk.size());

    beginInsertRows({}, oldSize, oldSize + newEntries - 1);
    for (auto&& entry : data.chunk) {
        m_entries.emplace_back(std::move(entry));
    }
    if (data.nextBatch.isEmpty()) {
        m_nextBatch = QString(); // null string is sentinel
    } else {
        m_nextBatch = std::move(data.nextBatch);
    }
    setTotalRoomCountEstimate(unwrap_or(data.totalRoomCountEstimate, -1));
    endInsertRows();
    emit fetchingChanged();
}

void PublicRoomListModel::onJobEnd()
{
    m_job = nullptr;
    emit fetchingChanged();
}

void PublicRoomListModel::setTotalRoomCountEstimate(int value)
{
    if (value == m_totalRoomCountEstimate)
        return;

    m_totalRoomCountEstimate = value;

    emit totalRoomCountEstimateChanged();
}

void PublicRoomListModel::abandonJob()
{
    if (m_job) {
        m_job->disconnect(this);
        m_job->abandon();
        m_job = nullptr;
        emit fetchingChanged();
    }
}

void PublicRoomListModel::resetModel()
{
    abandonJob();

    beginResetModel();
    m_nextBatch = UNKNOWN_NEXT_BATCH;
    setTotalRoomCountEstimate(-1);
    setError(QString());
    m_entries.clear();
    endResetModel();
}

} // namespace Det
