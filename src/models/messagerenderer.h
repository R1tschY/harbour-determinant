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

    QString renderEventText(
        bool isPending, const Quotient::RoomEvent* event) const;
    QString renderMessageText(
        bool isPending, const Quotient::RoomMessageEvent& event) const;
    QString renderMemberEvent(
        const Quotient::RoomMemberEvent& event) const;
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
