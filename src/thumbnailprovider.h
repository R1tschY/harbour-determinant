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
