#ifndef DET_DETROOM_H
#define DET_DETROOM_H

#include <QObject>
#include <room.h>

namespace Det {

class DetRoom : public Quotient::Room
{
    Q_OBJECT
public:
    explicit DetRoom(
            Quotient::Connection* connection, QString id, Quotient::JoinState initialJoinState);

};

} // namespace Det

#endif // DET_DETROOM_H
