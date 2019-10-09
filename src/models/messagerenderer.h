#ifndef MESSAGERENDERER_H
#define MESSAGERENDERER_H

#include <QCoreApplication>

namespace QMatrixClient {
class Room;
class User;
class RoomEvent;
class RoomMessageEvent;
class RoomMemberEvent;
class RoomCreateEvent;
class PendingEventItem;
} // namespace QMatrixClient


namespace Det {

class MessageRenderer
{
    Q_DECLARE_TR_FUNCTIONS(MessageRenderer)
public:
    MessageRenderer(const QMatrixClient::Room* room)
        : m_room(room)
    { }

    QString renderEventText(
        bool isPending, const QMatrixClient::RoomEvent *event) const;
    QString renderMessageText(
        bool isPending, const QMatrixClient::RoomMessageEvent &event) const;
    QString renderMemberEvent(
        const QMatrixClient::RoomMemberEvent &event) const;
    QString renderRoomCreated(const QMatrixClient::RoomCreateEvent &evt) const;

    QMatrixClient::User *getAuthor(
        bool isPending, const QMatrixClient::RoomEvent *evt) const;
    QString getAuthorDisplayName(
        bool isPending, const QMatrixClient::RoomEvent *evt) const;
    QString getAuthorHtmlDisplayName(
        bool isPending, const QMatrixClient::RoomEvent *evt) const;

    bool isHidden(const QMatrixClient::RoomEvent* evt) const;
    bool isPendingHidden(const QMatrixClient::PendingEventItem* evt) const;

    QString getLastEvent() const;
    QDateTime getLastActivity() const;

private:
    const QMatrixClient::Room* m_room;
};

} // namespace Det

#endif // MESSAGERENDERER_H
