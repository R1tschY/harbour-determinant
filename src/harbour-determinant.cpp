#include <memory>

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <connection.h>
#include <sailfishapp.h>

#include "chatsmodel.h"
#include "models/roomeventsmodel.h"
#include "quotientintegration.h"
#include "store.h"

int main(int argc, char* argv[])
{
    using namespace Determinant;

    std::unique_ptr<QGuiApplication> app(SailfishApp::application(argc, argv));

    QuotientIntegration::registerTypes();
    QMatrixClient::Connection connection;
    Store store;

    qmlRegisterType<ChatsModel>("Determinant", 0, 1, "ChatsModel");
    qmlRegisterType<RoomEventsModel>("Determinant", 0, 1, "RoomEventsModel");

    QQuickView* view = SailfishApp::createView();
    view->rootContext()->setContextProperty("connection", &connection);
    view->rootContext()->setContextProperty("store", &store);
    view->setSource(SailfishApp::pathToMainQml());
    view->showFullScreen();

    return app->exec();
}
