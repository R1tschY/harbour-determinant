#include <memory>

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <connection.h>
#include <sailfishapp.h>

#include "chatsmodel.h"
#include "connectionsmanager.h"
#include "models/roomeventsmodel.h"
#include "quotientintegration.h"
#include "store.h"

int main(int argc, char* argv[])
{
    using namespace Det;

    std::unique_ptr<QGuiApplication> app(SailfishApp::application(argc, argv));

    QuotientIntegration::registerTypes();
    ConnectionsManager connectionManager;

    qmlRegisterType<ChatsModel>("Determinant", 0, 1, "ChatsModel");
    qmlRegisterType<RoomEventsModel>("Determinant", 0, 1, "RoomEventsModel");

    connectionManager.load();

    QQuickView* view = SailfishApp::createView();
    view->rootContext()->setContextProperty("connection", connectionManager.connection());
    view->setSource(SailfishApp::pathToMainQml());
    view->showFullScreen();

    return app->exec();
}
