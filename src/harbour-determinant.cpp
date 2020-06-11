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
#include "quotientintegration.h"
#include "secretsservice.h"
#include "store.h"
#include "thumbnailprovider.h"

int main(int argc, char* argv[])
{
    using namespace Det;

    std::unique_ptr<QGuiApplication> app(SailfishApp::application(argc, argv));

    QuotientIntegration::registerTypes();

    qmlRegisterType<RoomListModel>("Determinant", 0, 1, "ChatsModel");
    qmlRegisterType<RoomEventsModel>("Determinant", 0, 1, "RoomEventsModel");
    qmlRegisterType<PublicRoomListModel>(
        "Determinant", 0, 1, "PublicRoomListModel");
    qmlRegisterType<Humanize>("Determinant", 0, 1, "Humanize");

    SecretsService secretsService;
    ConnectionsManager connectionManager(&secretsService);
    connectionManager.load();

    QQuickView* view = SailfishApp::createView();

    QQmlContext* ctx = view->rootContext();
    ctx->setContextProperty("connection", connectionManager.connection());

    view->engine()->addImageProvider(
        QStringLiteral("mtx"),
        new ThumbnailProvider(connectionManager.connection(), view));

    view->setSource(SailfishApp::pathToMainQml());
    view->showFullScreen();

    return app->exec();
}
