#include "detroom.h"

namespace Det {

using namespace Quotient;

DetRoom::DetRoom(
        Quotient::Connection* connection, QString id, Quotient::JoinState initialJoinState)
    : Room(connection, id, initialJoinState)
{

}

} // namespace Det
