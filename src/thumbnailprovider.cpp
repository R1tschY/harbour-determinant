#include "thumbnailprovider.h"

#include <QFileInfo>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QThread>
#include <connection.h>
#include <jobs/mediathumbnailjob.h>

namespace Det {

using namespace Quotient;

static Q_LOGGING_CATEGORY(logger, "det.thumbnails");

ThumbnailResponse::ThumbnailResponse(
    Connection* conn, const QString& mediaId, const QSize& requestedSize)
    : m_conn(conn)
    , m_mediaId(mediaId)
    , m_requestedSize(requestedSize)
{
    Q_ASSERT(m_conn != nullptr);

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_localPath = QStringLiteral("%1/thumbnails/%2-%3x%4")
                      .arg(cacheDir, mediaId,
                          QString::number(requestedSize.width()),
                          QString::number(requestedSize.height()));

    moveToThread(m_conn->thread());
    QMetaObject::invokeMethod(this, "doStart", Qt::QueuedConnection);
}

void ThumbnailResponse::doStart()
{
    Q_ASSERT(QThread::currentThread() == m_conn->thread());
    Q_ASSERT(m_job == nullptr);

    qCDebug(logger) << "String request" << m_mediaId << m_requestedSize << m_localPath;

    if (m_requestedSize.isEmpty() || m_mediaId.isEmpty()) {
        emit finished();
        return;
    }

    if (m_mediaId.count(QChar('/')) != 1) {
        m_error = tr("Media id `%1` does not have expected pattern: server/id")
                      .arg(m_mediaId);
        qCDebug(logger) << "ERROR" << m_error;
        emit finished();
        return;
    }

    // must be done in connection thread else data race with onJobFinished
    qCDebug(logger) << "Path" << m_localPath << QFileInfo::exists(m_localPath);
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

    qCDebug(logger) << "Job finished" << m_localPath;

    if (m_job->error() == BaseJob::Success) {
        m_image = m_job->thumbnail();

        if (QFileInfo(m_localPath).dir().mkpath(".")) {
            if (!m_image.save(m_localPath, "PNG")) {
                qCWarning(logger) << "Failed to save fetched image";
            } else {
                qCDebug(logger) << "Saved fetched image to" << m_localPath;
            }
        } else {
            qCWarning(logger) << "Failed to create image cache directory"
                              << QFileInfo(m_localPath).filePath();
        }
    } else if (m_job->error() == BaseJob::Abandoned) {
        m_error = "Thumbnail request was canceled";
        qCDebug(logger) << "Canceled thumbnail response finished:" << m_mediaId;
    } else if (m_job->error() >= BaseJob::ErrorLevel) {
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

ThumbnailProvider::ThumbnailProvider(Connection* m_connection, QObject* parent)
    : QObject(parent)
    , m_connection(m_connection)
{
}

QQuickImageResponse* ThumbnailProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    return new ThumbnailResponse(m_connection.load(), id, requestedSize);
}

} // namespace Det
