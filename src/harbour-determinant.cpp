#include <memory>

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif
#include <QtQml>
#include <QQuickView>
#include <QGuiApplication>

#include <connection.h>
#include <sailfishapp.h>

#include "connectionsmanager.h"
#include "models/chatsmodel.h"
#include "models/roomeventsmodel.h"
#include "quotientintegration.h"
#include "store.h"
#include "humanize.h"

int main(int argc, char* argv[])
{
    using namespace Det;

    std::unique_ptr<QGuiApplication> app(SailfishApp::application(argc, argv));

    QuotientIntegration::registerTypes();

    qmlRegisterType<ChatsModel>("Determinant", 0, 1, "ChatsModel");
    qmlRegisterType<RoomEventsModel>("Determinant", 0, 1, "RoomEventsModel");
    qmlRegisterSingletonType<Humanize>(
        "Determinant", 0, 1, "Humanize",
        [](QQmlEngine*, QJSEngine*) -> QObject* { return new Humanize(); });

    ConnectionsManager connectionManager;
    connectionManager.load();

    QQuickView* view = SailfishApp::createView();

    QQmlContext* ctx = view->rootContext();
    ctx->setContextProperty("connection", connectionManager.connection());

    view->setSource(SailfishApp::pathToMainQml());
    view->showFullScreen();

    return app->exec();
}
