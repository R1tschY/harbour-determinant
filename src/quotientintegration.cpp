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
#include <eventitem.h>

using namespace Quotient;

Q_DECLARE_METATYPE(SyncJob*)
Q_DECLARE_METATYPE(Room*)

namespace Det {

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

    qmlRegisterType<Connection>("Quotient", 0, 5, "Connection");
    qmlRegisterUncreatableType<EventStatus>(
        "Quotient", 0, 5, "EventStatus",
        "EventStatus cannot be created from QML");
}

} // namespace Det
