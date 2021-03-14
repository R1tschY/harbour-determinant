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


#include <memory>

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif
#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include <connection.h>
#include <sailfishapp.h>

#include "connectionsmanager.h"
#include "humanize.h"
#include "models/publicroomlistmodel.h"
#include "models/roomeventsmodel.h"
#include "models/roomlistmodel.h"
#include "models/sortfiltermodel.h"
#include "quotientintegration.h"
#include "secretsservice.h"
#include "store.h"
#include "thumbnailprovider.h"
#include "qmlutils/stringutils.h"
#include "notificationsservice.h"
#include "applicationservice.h"

int main(int argc, char* argv[])
{
    using namespace Det;

    std::unique_ptr<QGuiApplication> app(SailfishApp::application(argc, argv));

    QuotientIntegration::registerTypes();
    StringUtils::registerType();

    qmlRegisterType<RoomListModel>("Determinant", 0, 1, "RoomListModel");
    qmlRegisterType<RoomEventsModel>("Determinant", 0, 1, "RoomEventsModel");
    qmlRegisterType<PublicRoomListModel>(
        "Determinant", 0, 1, "PublicRoomListModel");
    qmlRegisterType<Humanize>("Determinant", 0, 1, "Humanize");
    qmlRegisterType<SortFilterModel>("Determinant", 0, 1, "SortFilterModel");

    DBusApplicationService* applicationService = new DBusApplicationService(app.get());
    if (applicationService->deduplicate()) {
        return 0;
    }
    applicationService->registerOnDbus();

    SecretsService secretsService;
    ConnectionsManager connectionManager(&secretsService);
    connectionManager.load();

    new NotificationsService(connectionManager.connection());

    QQuickView* view = SailfishApp::createView();

    QQmlContext* ctx = view->rootContext();
    ctx->setContextProperty("connection", connectionManager.connection());
    ctx->setContextProperty("applicationService", applicationService);

    view->engine()->addImageProvider(
        QStringLiteral("mxc-thumbnail"),
        new ThumbnailProvider(connectionManager.connection(), view));

    view->setSource(SailfishApp::pathToMainQml());
    view->showFullScreen();

    return app->exec();
}
