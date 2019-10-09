#include "messagerenderer.h"

#include <QString>
#include <QRegularExpression>
#include <QLoggingCategory>

#include <events/redactionevent.h>
#include <events/roomcreateevent.h>
#include <events/roommemberevent.h>
#include <events/roommessageevent.h>
#include <events/roomtombstoneevent.h>
#include <events/simplestateevents.h>
#include <user.h>
#include <room.h>
#include <connection.h>

namespace Det {

using namespace QMatrixClient;

static Q_LOGGING_CATEGORY(logger, "determinant.messagerenderer")

QString MessageRenderer::renderEventText(
        bool isPending, const RoomEvent* event) const
{
    if (event->isRedacted()) {
        QString reason = event->redactedBecause()->reason();
        QString author = getAuthorHtmlDisplayName(isPending, event);
        if (reason.isEmpty())
            return tr("%1 redacted message").arg(author);
        else
            return tr("%1 redacted: %2").arg(author, reason);
    }

    QString result = visit(
        *event,
        [this, isPending](const RoomMessageEvent& evt) {
            return renderMessageText(isPending, evt);
        },
        [this, isPending](const RoomMemberEvent& evt) {
            return renderMemberEvent(evt);
        },
        QString());
    if (!result.isNull()) {
        return result;
    }

    result = visit(
        *event,
        [this, isPending](const RoomAliasesEvent& evt) {
            QString author = getAuthorHtmlDisplayName(isPending, &evt);
            return tr("%1 set room aliases on server %1 to %2")
                .arg(author, evt.stateKey().toHtmlEscaped(),
                     QLocale().createSeparatedList(
                         evt.aliases()).toHtmlEscaped());
        },
        [this, isPending](const RoomCanonicalAliasEvent& evt) {
            QString author = getAuthorHtmlDisplayName(isPending, &evt);
            auto alias = evt.alias();
            return alias.isEmpty()
                ? tr("%1 cleared room main alias").arg(author)
                : tr("%1 set room main alias to %2")
                  .arg(author, alias.toHtmlEscaped());
        },
        [this, isPending](const RoomNameEvent& evt) {
            QString author = getAuthorHtmlDisplayName(isPending, &evt);
            auto name = evt.name();
            return name.isEmpty()
                ? tr("%1 cleared room name").arg(author)
                : tr("%1 set room name to %2")
                  .arg(author, name.toHtmlEscaped());
        },
        [this, isPending](const RoomTopicEvent& evt) {
            QString author = getAuthorHtmlDisplayName(isPending, &evt);
            auto topic = evt.topic();
            return topic.isEmpty()
                ? tr("%1 removed topic").arg(author)
                : tr("%1 set topic to %2").arg(author, topic.toHtmlEscaped());
        },
        [this, isPending](const EncryptionEvent& evt) {
            QString author = getAuthorHtmlDisplayName(isPending, &evt);
            return tr("%1 activated end-to-end encryption").arg(author);
        },
        [this](const RoomCreateEvent& evt) {
            return renderRoomCreated(evt);
        },
        [this, isPending](const RoomTombstoneEvent& evt) {
            QString author = getAuthorHtmlDisplayName(isPending, &evt);
            return tr("upgraded room to version %1")
                .arg(evt.serverMessage().toHtmlEscaped());
        },
        QString());
    if (!result.isNull()) {
        return result;
    }

    return tr("Unsupported event: %1").arg(event->matrixType());
}

static QString renderMarkdown(const QString& content)
{
    static QRegularExpression boldRe(R"#(\*\*(.+?)\*\*)#");
    static QRegularExpression italicRe(R"#(\*(.+?)\*)#");
    // TODO: use markdown engine or more regexes
    // TODO: newlines
    QString result = content;
    result.replace(boldRe, "<b>\\1</b>");
    result.replace(italicRe, "<i>\\1</i>");
    return result;
}

QString MessageRenderer::renderMessageText(
        bool isPending, const RoomMessageEvent& event) const
{
    using namespace QMatrixClient::EventContent;

    auto* content = event.content();
    if (!content) {
        return event.plainBody().toHtmlEscaped();
    }

    auto type = event.msgtype();
    switch (type) {
    case RoomMessageEvent::MsgType::Text:
    case RoomMessageEvent::MsgType::Emote:
    case RoomMessageEvent::MsgType::Notice: {
        auto textContent = static_cast<TextContent*>(content);
        QString mimeType = textContent->mimeType.name();
        if (mimeType == QStringLiteral("text/markdown")) {
            // TODO: cleanup HTML
            return renderMarkdown(textContent->body);
        }
        if (mimeType == QStringLiteral("text/html")) {
            // TODO: cleanup HTML
            return textContent->body;
        }

        QString text = textContent->body.toHtmlEscaped();
        text.replace(QChar('\n'), QStringLiteral("<br/>"));
        return text;
    }

    case RoomMessageEvent::MsgType::Image:
    case RoomMessageEvent::MsgType::File:
    case RoomMessageEvent::MsgType::Location:
    case RoomMessageEvent::MsgType::Video:
    case RoomMessageEvent::MsgType::Audio:
    case RoomMessageEvent::MsgType::Unknown:
    default:
        break;
    }

    return tr("Unsupported message");
}


QString MessageRenderer::renderMemberEvent(const RoomMemberEvent& event) const
{
    QStringList messages;

    QString member = event.displayName().toHtmlEscaped();
    auto membership = event.membership();
    auto* prevContent = event.prevContent();
    if (!prevContent || membership != prevContent->membership) {
        // membership changed
        switch (membership) {
        case RoomMemberEvent::MembershipType::Join:
            messages.append(tr("%1 has joined").arg(member));
            break;
        case RoomMemberEvent::MembershipType::Invite:
            messages.append(tr("%1 was invited").arg(member));
            break;
        case RoomMemberEvent::MembershipType::Ban:
            messages.append(tr("%1 is banned").arg(member));
            break;
        case RoomMemberEvent::MembershipType::Leave:
            if (!prevContent
                    || prevContent->membership != RoomMemberEvent::MembershipType::Ban) {
                messages.append(tr("%1 has left").arg(member));
            }
            break;
        case RoomMemberEvent::MembershipType::Knock:
            messages.append(tr("%1 knocked").arg(member));
            break;
        case RoomMemberEvent::MembershipType::Undefined:
        default:
            // TODO:
            messages.append(tr("%1 has undefined state").arg(member));
            break;
        }
    }

    QString prevName = prevContent ? prevContent->displayName : QString();
    if (prevName != member) {
        if (prevName.isEmpty()) {
            // TODO?
            qCWarning(logger) << "User" << member
                              << "had not a name before";
        } else {
            // display name changed
            messages.append(
                tr("%1 has changed name to %2").arg(prevName, member));
        }
    }

    QUrl prevAvatar = prevContent ? prevContent->avatarUrl : QUrl();
    if (prevAvatar != event.avatarUrl()) {
        // avatar changed
        messages.append(
            tr("%1 has changed avatar").arg(member));
    }

    if (messages.isEmpty()) {
        // TODO:
        qCWarning(logger) << "No change detected";
        return QStringLiteral("No change detected");
    }

    return messages.join(QStringLiteral("<br/>"));
}

QString MessageRenderer::renderRoomCreated(const RoomCreateEvent& evt) const
{
    QString author = getAuthorHtmlDisplayName(false, &evt);
    if (evt.isUpgrade()) {
        QString version = evt.version();
        QString versionString = version.isEmpty() ? "1" : version;
        return tr("%1 upgraded room to version %2").arg(
            author, versionString.toHtmlEscaped());
    } else {
        return tr("%1 created room").arg(author);
    }
}

User* MessageRenderer::getAuthor(bool isPending, const RoomEvent* evt) const
{
    return isPending
            ? m_room->localUser()
            : m_room->user(evt->senderId());
}

QString MessageRenderer::getAuthorDisplayName(
        bool isPending, const RoomEvent* evt) const
{
    return m_room->roomMembername(getAuthor(isPending, evt));
}

QString MessageRenderer::getAuthorHtmlDisplayName(
        bool isPending, const RoomEvent* evt) const
{
    return getAuthorDisplayName(isPending, evt).toHtmlEscaped();
}

bool MessageRenderer::isHidden(const RoomEvent *evt) const
{
    if (evt->isRedacted())
        return true;

    const User* user = m_room->user(evt->senderId());
    if (m_room->connection()->isIgnored(user))
        return true;

    return false;
}

bool MessageRenderer::isPendingHidden(const PendingEventItem *evt) const
{
    return evt->deliveryStatus() & EventStatus::Hidden;
}

QString MessageRenderer::getLastEvent() const
{
    auto begin = m_room->messageEvents().crbegin();
    auto end = m_room->messageEvents().crend();

    // TODO: pending events?
    for (auto i = begin; i < end; ++i) {
        const RoomEvent* evt = i->get();

        if (isHidden(evt))
            continue;

        if (m_room->isDirectChat() || evt->isStateEvent()) {
            return renderEventText(false, evt);
        } else {
            const User* user = m_room->user(evt->senderId());
            return QStringLiteral("<b>")
                    % m_room->roomMembername(user)
                    % QStringLiteral("</b>: ")
                    % renderEventText(false, evt);
        }
    }

    return QString();
}

QDateTime MessageRenderer::getLastActivity() const
{
    if (m_room->timelineSize() == 0) {
        return QDateTime();
    } else {
        return m_room->messageEvents().crbegin()->get()->timestamp();
    }
}

} // namespace Det
