#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QAtomicPointer>
#include <QImage>
#include <QObject>
#include <QQuickAsyncImageProvider>

namespace Quotient {
class Connection;
class MediaThumbnailJob;
} // namespace

namespace Det {

class ThumbnailResponse : public QQuickImageResponse {
    Q_OBJECT
public:
    ThumbnailResponse(Quotient::Connection* conn,
        const QString& mediaId, const QSize& requestedSize);

    QQuickTextureFactory* textureFactory() const override;
    QString errorString() const override;

public slots:
    void cancel() override;

private:
    Quotient::Connection* m_conn;

    QString m_mediaId;
    QSize m_requestedSize;

    Quotient::MediaThumbnailJob* m_job = nullptr;

    QString m_localPath;
    QString m_error;
    QImage m_image;

    void onJobFinished();

private slots:
    void doStart();
    void doCancel();
};

class ThumbnailProvider : public QObject, public QQuickAsyncImageProvider {
    Q_OBJECT
public:
    ThumbnailProvider(Quotient::Connection* m_connection, QObject* parent = nullptr);

    QQuickImageResponse* requestImageResponse(
        const QString& id, const QSize& requestedSize) override;

private:
    QAtomicPointer<Quotient::Connection> m_connection;
};

} // namespace Det

#endif // IMAGEPROVIDER_H
