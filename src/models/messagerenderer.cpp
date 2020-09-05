#include "messagerenderer.h"

#include <QLoggingCategory>
#include <QRegularExpression>
#include <QString>
#include <QStringBuilder>

#include <connection.h>
#include <events/redactionevent.h>
#include <events/roomcreateevent.h>
#include <events/roomevent.h>
#include <events/roommemberevent.h>
#include <events/roommessageevent.h>
#include <events/roomtombstoneevent.h>
#include <events/roomcanonicalaliasevent.h>
#include <events/simplestateevents.h>
#include <events/encryptionevent.h>
#include <events/reactionevent.h>
#include <events/redactionevent.h>
#include <room.h>
#include <user.h>
#include <util.h>

namespace Det {

using namespace Quotient;

static Q_LOGGING_CATEGORY(logger, "determinant.messagerenderer");

QString MessageRenderer::renderEventText(
    bool isPending, const RoomEvent* event) const
{
    if (event->isRedacted()) {
        QString reason = event->redactedBecause()->reason();
        if (reason.isEmpty())
            return tr("🗑️ redacted message");
        else
            return tr("🗑️ redacted: %2").arg(reason);
    }

    QString result = visit(
        *event,
        [this, isPending](const RoomMessageEvent& evt) {
            return renderMessageText(isPending, evt);
        },
        [this](const RoomMemberEvent& evt) {
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
                                 evt.aliases())
                        .toHtmlEscaped());
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
        [](const RedactionEvent&) {
            return QStringLiteral("redacted event"); // should never be shown
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
        auto textContent = static_cast<const TextContent*>(content);
        QString mimeType = textContent->mimeType.name();
        if (mimeType == QStringLiteral("text/markdown")) {
            // TODO: cleanup HTML
            // TODO: detect links
            return renderMarkdown(textContent->body);
        } else if (mimeType == QStringLiteral("text/html")) {
            // TODO: cleanup HTML
            // TODO: detect links
            return textContent->body;
        }

        return prettyPrint(textContent->body);
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

    QString member = event.displayName();
    auto membership = event.membership();
    auto* prevContent = event.prevContent();

    if (member.isEmpty()) {
        if (prevContent && !prevContent->displayName.isEmpty()) {
            member = prevContent->displayName;
        } else {
            member = tr("<Unnamed user>");
        }
    }

    if (!prevContent || membership != prevContent->membership) {
        // membership changed
        switch (membership) {
        case RoomMemberEvent::MembershipType::Join:
            return tr("%1 has joined").arg(member);
        case RoomMemberEvent::MembershipType::Invite:
            return tr("%1 was invited").arg(member);
        case RoomMemberEvent::MembershipType::Ban:
            return tr("%1 is banned").arg(member);
        case RoomMemberEvent::MembershipType::Leave:
            if (!prevContent
                || prevContent->membership != RoomMemberEvent::MembershipType::Ban) {
                return tr("%1 has left").arg(member);
            }
            break;
        case RoomMemberEvent::MembershipType::Knock:
            return tr("%1 knocked").arg(member);
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
        qCWarning(logger).noquote()
            << member << "changed something:"
            << event.fullJson().value(QStringLiteral("content")).toObject()
            << event.fullJson().value(QStringLiteral("prev_content"))
               .toObject();
        return QStringLiteral("%1 changed something").arg(member);
    }

    return messages.join(QStringLiteral("\n"));
}

QString MessageRenderer::renderRoomCreated(const RoomCreateEvent& evt) const
{
    QString author = getAuthorHtmlDisplayName(false, &evt);
    if (evt.isUpgrade()) {
        QString version = evt.version();
        QString versionString = version.isEmpty() ? "1" : version;
        return tr("%1 upgraded room to version %2").arg(author, versionString.toHtmlEscaped());
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

bool MessageRenderer::isHidden(const RoomEvent* evt) const
{
// TODO: option to ignore removed messages
    //    if (evt->isRedacted()) {
//        return true;
//    }

    if (evt->isStateEvent() &&
            static_cast<const StateEventBase*>(evt)->repeatsState())
        return true;

    if (is<ReactionEvent>(*evt) || is<RedactionEvent>(*evt))
        return true;

    if (auto msgEvt = eventCast<const RoomMessageEvent>(evt)) {
        if (!msgEvt->replacedEvent().isEmpty() &&
                msgEvt->replacedEvent() != msgEvt->id()) {
            return true;
        }
    }

    const User* user = m_room->user(evt->senderId());
    if (m_room->connection()->isIgnored(user))
        return true;

    return false;
}

bool MessageRenderer::isPendingHidden(const PendingEventItem* evt) const
{
    auto status = evt->deliveryStatus();
    // TODO: edited pending event
    return (status & EventStatus::Hidden) || (status & EventStatus::Redacted);
}

static QString extractPreview(const QString& messageHtml)
{
    static QRegularExpression brRe("<br\\s*/?>", QRegularExpression::OptimizeOnFirstUsageOption);
    static QRegularExpression tagRe("<[^>]*>", QRegularExpression::OptimizeOnFirstUsageOption);

    QString result = messageHtml;

    // remove <mx-reply>*</mx-reply>
    if (messageHtml.startsWith(QStringLiteral("<mx-reply>"))) {
        int index = messageHtml.indexOf(QStringLiteral("</mx-reply>"));
        if (index >= 0) {
            result.remove(0, index + QStringLiteral("</mx-reply>").length());
        }
    }

    // remove <br> and everything after it
    QRegularExpressionMatch brMatch = brRe.match(messageHtml);
    if (brMatch.hasMatch()) {
        int start = brMatch.capturedStart();
        result.truncate(start);
    }

    // remove every HTML tag
    result.replace(tagRe, QString());

    return result;
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

        // FIXME: only first line, no HTML
        if (m_room->isDirectChat()) {
            return extractPreview(renderEventText(false, evt));
        } else if (evt->isStateEvent()) {
            return QStringLiteral("<i>") % extractPreview(renderEventText(false, evt)) % QStringLiteral("</i>");
        } else {
            const User* user = m_room->user(evt->senderId());
            return QStringLiteral("<b>")
                % m_room->roomMembername(user)
                % QStringLiteral("</b>: ")
                % extractPreview(renderEventText(false, evt));
        }
    }

    return QString();
}

QDateTime MessageRenderer::getLastActivity() const
{
    if (m_room->timelineSize() == 0) {
        return QDateTime();
    } else {
        return m_room->messageEvents().crbegin()->get()->originTimestamp();
    }
}

} // namespace Det
