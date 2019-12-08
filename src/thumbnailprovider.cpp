#include "thumbnailprovider.h"

#include <QFileInfo>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QThread>
#include <connection.h>
#include <jobs/mediathumbnailjob.h>

namespace Det {

using namespace QMatrixClient;

static Q_LOGGING_CATEGORY(logger, "det.thumbnails");

ThumbnailResponse::ThumbnailResponse(
    Connection* conn, const QString& mediaId, const QSize& requestedSize)
    : m_conn(conn)
    , m_mediaId(mediaId)
    , m_requestedSize(requestedSize)
{
    Q_ASSERT(m_conn != nullptr);

    if (m_requestedSize.isEmpty()) {
        emit finished();
        return;
    }

    if (mediaId.count(QChar('/')) != 1) {
        m_error = tr("Media id `%1` does not have expected pattern: server/id")
                      .arg(mediaId);
        emit finished();
        return;
    }

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_localPath = QStringLiteral("%1/images/%2-%3x%4")
                      .arg(cacheDir, mediaId,
                          QString::number(requestedSize.width()),
                          QString::number(requestedSize.height()));

    moveToThread(m_conn->thread());
    QMetaObject::invokeMethod(this, SLOT(doStart()), Qt::QueuedConnection);
}

void ThumbnailResponse::doStart()
{
    Q_ASSERT(QThread::currentThread() == m_conn->thread());
    Q_ASSERT(m_job == nullptr);

    // must be done in connection thread else data race with onJobFinished
    if (QFileInfo::exists(m_localPath)) {
        if (m_image.load(m_localPath)) {
            emit finished();
            return;
        }
    }

    m_job = m_conn->getThumbnail(m_mediaId, m_requestedSize);
    connect(m_job, &MediaThumbnailJob::finished, this, &ThumbnailResponse::onJobFinished);
}

void ThumbnailResponse::doCancel()
{
    Q_ASSERT(QThread::currentThread() == m_conn->thread());

    if (m_job) {
        m_job->abandon();
    }
}

void ThumbnailResponse::onJobFinished()
{
    Q_ASSERT(QThread::currentThread() == m_conn->thread());
    Q_ASSERT(m_job != nullptr);
    Q_ASSERT(m_job->error() != BaseJob::Pending);

    if (m_job->error() == BaseJob::Success) {
        m_image = m_job->thumbnail();

        if (QFileInfo(m_localPath).dir().mkpath(".")) {
            m_image.save(m_localPath);
        } else {
            qCWarning(logger) << "Failed to create image cache directory"
                              << QFileInfo(m_localPath).filePath();
        }

    } else if (m_job->error() == BaseJob::Abandoned) {
        m_error = tr("Thumbnail request was canceled");
        qCDebug(logger) << "Canceled thumbnail response finished:" << m_mediaId;
    } else if (m_job->error() == BaseJob::Abandoned) {
        m_error = m_job->errorString();
        qCWarning(logger) << "Thumbnail request failed for" << m_mediaId << "-" << m_error;
    }

    m_job = nullptr;
    emit finished();
}

QQuickTextureFactory* ThumbnailResponse::textureFactory() const
{
    // always called after final edit in onJobFinished -> no mutex needed
    Q_ASSERT(m_job == nullptr);
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

QString ThumbnailResponse::errorString() const
{
    // always called after final edit in onJobFinished -> no mutex needed
    Q_ASSERT(m_job == nullptr);
    return m_error;
}

void ThumbnailResponse::cancel()
{
    QMetaObject::invokeMethod(this, SLOT(doCancel()), Qt::QueuedConnection);
}

ThumbnailProvider::ThumbnailProvider(QMatrixClient::Connection* m_connection, QObject* parent)
    : QObject(parent)
    , m_connection(m_connection)
{
}

QQuickImageResponse* ThumbnailProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    return new ThumbnailResponse(m_connection.load(), id, requestedSize);
}

} // namespace Det
