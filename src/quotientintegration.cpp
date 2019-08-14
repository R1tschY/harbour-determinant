#include "quotientintegration.h"

#include <QtQml>

#include <connection.h>
#include <csapi/joining.h>
#include <csapi/message_pagination.h>
#include <jobs/syncjob.h>
#include <room.h>
#include <user.h>

//#include <models/messageeventmodel.h>
//#include <models/roomlistmodel.h>
#include <settings.h>
//#include <matrixhelper.h>

using namespace QMatrixClient;

Q_DECLARE_METATYPE(SyncJob*)
Q_DECLARE_METATYPE(Room*)

namespace Determinant {

void QuotientIntegration::registerTypes()
{
    qmlRegisterType<SyncJob>();
    qRegisterMetaType<SyncJob*>("SyncJob*");

    qmlRegisterType<JoinRoomJob>();
    qRegisterMetaType<JoinRoomJob*>("JoinRoomJob*");

    qmlRegisterType<GetRoomEventsJob>();
    qRegisterMetaType<GetRoomEventsJob*>("GetRoomEventsJob*");

    //qmlRegisterType<LeaveRoomJob>();
    //qRegisterMetaType<LeaveRoomJob*>("LeaveRoomJob*");

    qmlRegisterType<Room>();
    qRegisterMetaType<Room*>("Room*");

    qmlRegisterType<User>();
    qRegisterMetaType<User*>("User*");

    qmlRegisterType<Connection>("Matrix", 0, 5, "Connection");
    //qmlRegisterType<MessageEventModel>("Matrix", 1, 0, "MessageEventModel");
    //qmlRegisterType<RoomListModel>("Matrix", 1, 0, "RoomListModel");
}

} // namespace Determinant
