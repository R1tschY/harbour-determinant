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

#ifndef MESSAGERENDERER_H
#define MESSAGERENDERER_H

#include <QCoreApplication>

namespace Quotient {
class Room;
class User;
class RoomEvent;
class RoomMessageEvent;
class RoomMemberEvent;
class RoomCreateEvent;
class PendingEventItem;
} // namespace Quotient

namespace Det {

class MessageRenderer {
    Q_DECLARE_TR_FUNCTIONS(MessageRenderer)
public:
    MessageRenderer(const Quotient::Room* room)
        : m_room(room)
    {
    }

    QString renderEventText(bool isPending, const Quotient::RoomEvent* event) const;
    QString renderMessageText(const Quotient::RoomMessageEvent& event) const;
    QString renderMemberEvent(const Quotient::RoomMemberEvent& event) const;
    QString renderRoomCreated(const Quotient::RoomCreateEvent& evt) const;

    Quotient::User* getAuthor(
        bool isPending, const Quotient::RoomEvent* evt) const;
    QString getAuthorDisplayName(
        bool isPending, const Quotient::RoomEvent* evt) const;
    QString getAuthorHtmlDisplayName(
        bool isPending, const Quotient::RoomEvent* evt) const;

    bool isHidden(const Quotient::RoomEvent* evt) const;
    bool isPendingHidden(const Quotient::PendingEventItem* evt) const;

    QString getLastEvent() const;
    QDateTime getLastActivity() const;

private:
    const Quotient::Room* m_room;
};

} // namespace Det

#endif // MESSAGERENDERER_H
