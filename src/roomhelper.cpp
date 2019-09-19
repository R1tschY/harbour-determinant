#include "roomhelper.h"

#include <events/roomevent.h>
#include <connection.h>

namespace Det {

using namespace QMatrixClient;


QString RoomHelper::getLastEvent() const
{
    auto begin = m_room->messageEvents().crbegin();
    auto end = m_room->messageEvents().crend();

    // TODO: pending events?
    for (auto i = begin; i < end; ++i) {
        // TODO: isHiddenEvent(evt)
        const RoomEvent* evt = i->get();

        if (evt->isRedacted())
            continue;

        const User* user = m_room->user(evt->senderId());
        if (m_connection->isIgnored(user))
            continue;

        if (m_room->isDirectChat() || evt->isStateEvent()) {
            return evt->id(); // TODO
        } else {
            return QStringLiteral("<b>")
                    % m_room->roomMembername(user)
                    % QStringLiteral("</b>: ")
                    % evt->id(); // TODO
        }
    }

    return QString();
}

QDateTime RoomHelper::getLastActivity() const
{
    if (m_room->timelineSize() == 0) {
        return QDateTime();
    } else {
        return m_room->messageEvents().crbegin()->get()->timestamp();
    }
}

} // namespace Det
