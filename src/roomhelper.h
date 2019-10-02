#ifndef ROOMHELPER_H
#define ROOMHELPER_H

#include <room.h>

class QDateTime;


namespace Det {

class RoomHelper
{
public:
    RoomHelper(QMatrixClient::Room* room)
        : m_room(room)
    { }

    QString getLastEvent() const;
    QDateTime getLastActivity() const;

private:
    QMatrixClient::Room* m_room;
};

} // namespace Det

#endif // ROOMHELPER_H
