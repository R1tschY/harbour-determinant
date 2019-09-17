#include "roomeventsmodel.h"

#include <QLoggingCategory>
#include <QRegularExpression>
#include <QString>
#include <QUrl>

#include <events/redactionevent.h>
#include <events/roomcreateevent.h>
#include <events/roommemberevent.h>
#include <events/roommessageevent.h>
#include <events/roomtombstoneevent.h>
#include <events/simplestateevents.h>
#include <user.h>

namespace Determinant {

using namespace QMatrixClient;

static Q_LOGGING_CATEGORY(logger, "determinant.roommodel")

    //
    // Model:
    //                    t <---|
    // +---------+--------------+
    // | Pending |   Messages   |
    // +---------+--------------+
    // Not merged New        Old
    //            maxTimelineIndex
    //                       minTimelineIndex
    //

    RoomEventsModel::RoomEventsModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int RoomEventsModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (!m_room || parent.isValid())
        return 0;

    return m_room->timelineSize() + m_pendingEvents;
}

QVariant RoomEventsModel::data(const QModelIndex& index, int role) const
{
    if (!m_room || !index.isValid())
        return QVariant();

    int row = index.row();
    if (row > m_room->timelineSize() + m_pendingEvents)
        return QVariant();

    if (m_pendingEvents != m_room->pendingEvents().size()) {
        qCDebug(logger) << "wrong pendingEvents count";
    }

    bool isPending = row < m_pendingEvents;

    const RoomEvent* evt;
    const PendingEventItem* pendingEvt;
    const TimelineItem* timelineEvt;
    if (isPending) {
        auto& evtRef = *(
            m_room->pendingEvents().crbegin() + row);
        pendingEvt = &evtRef;
        evt = evtRef.get();
    } else {
        auto& evtRef = *(
            m_room->messageEvents().crbegin() + (row - m_pendingEvents));
        timelineEvt = &evtRef;
        evt = evtRef.get();
    }

    switch (role) {
    case DisplayRole:
        return renderEventText(isPending, evt);

    case AuthorRole:
        return QVariant::fromValue(getAuthor(isPending, evt));

    case EventIdRole:
        break;

    case MatrixTypeRole:
        return evt->matrixType();

    case EventTypeRole:
        if (auto* msgEvt = eventCast<const RoomMessageEvent>(evt)) {
            QString rawMsgtype = msgEvt->rawMsgtype();
            if (!rawMsgtype.isEmpty()) {
                rawMsgtype.remove(0, 2);
                return rawMsgtype;
            }
            if (msgEvt->hasFileContent()) {
                return QStringLiteral("file");
            }
        }
        if (evt->isStateEvent()) {
            return QStringLiteral("state");
        }
        return QStringLiteral("other");

    case AnnotationRole:
        if (isPending)
            return pendingEvt->annotation();
        break;

    case TimeRole:
        return isPending
            ? pendingEvt->lastUpdated()
            : evt->timestamp();

    case HiddenRole:
        if (isPending)
            return pendingEvt->deliveryStatus() & EventStatus::Hidden;
        return false;

    case ContentTypeRole:
        return visit(
            *evt,
            [](const RoomMessageEvent& evt) {
                return evt.rawMsgtype();
            },
            QString());
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> RoomEventsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(DisplayRole, "display");
    roles.insert(EventIdRole, "eventId");
    roles.insert(MatrixTypeRole, "matrixType");
    roles.insert(EventTypeRole, "eventType");
    roles.insert(AnnotationRole, "annotation");
    roles.insert(TimeRole, "time");
    roles.insert(HiddenRole, "hidden");
    roles.insert(AuthorRole, "author");
    roles.insert(ContentTypeRole, "contentType");
    return roles;
}

QVariant RoomEventsModel::renderEventText(
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

QString RoomEventsModel::renderMessageText(
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

QString RoomEventsModel::renderMemberEvent(const RoomMemberEvent& event) const
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

QString RoomEventsModel::renderRoomCreated(const RoomCreateEvent& evt) const
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

User* RoomEventsModel::getAuthor(bool isPending, const RoomEvent* evt) const
{
    return isPending
            ? m_room->localUser()
            : m_room->user(evt->senderId());
}

QString RoomEventsModel::getAuthorDisplayName(
        bool isPending, const RoomEvent* evt) const
{
    return m_room->roomMembername(getAuthor(isPending, evt));
}

QString RoomEventsModel::getAuthorHtmlDisplayName(
        bool isPending, const RoomEvent* evt) const
{
    return getAuthorDisplayName(isPending, evt).toHtmlEscaped();
}

int RoomEventsModel::pendingEventsCount() const
{
    return m_pendingEvents;
}

void RoomEventsModel::onBeginInsertMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    beginInsertRows({}, m_pendingEvents, m_pendingEvents + events.size() - 1);
}

void RoomEventsModel::onBeginInsertOldMessages(RoomEventsRange events)
{
    Q_ASSERT(events.size() > 0);

    int size = m_room->timelineSize() + m_pendingEvents;
    beginInsertRows({}, size, size + events.size() - 1);
}

void RoomEventsModel::onBeginSyncMessage(int i)
{
    if (m_pendingEvents - 1 == i) {
        // trival case
        --m_pendingEvents;
        updateRow(m_pendingEvents);
        return;
    }

    Q_ASSERT(i >= 0 && i < m_pendingEvents);
    //Q_ASSERT(m_pendingEvents == m_room->pendingEvents().size());

    m_movingEvents = true;
    int index = m_pendingEvents - (i + 1);
    beginMoveRows({}, index, index, {}, m_pendingEvents - 1);
    --m_pendingEvents;
}

void RoomEventsModel::onEndSyncMessage()
{
    if (m_movingEvents) {
        m_movingEvents = false;
        endMoveRows();
    }

    //Q_ASSERT(m_pendingEvents == m_room->pendingEvents().size());
    updateRow(m_pendingEvents);
}

void RoomEventsModel::onReplacedMessage(
    const RoomEvent* newEvent, const RoomEvent* oldEvent)
{
    Q_ASSERT(newEvent->id() == oldEvent->id());
}

void RoomEventsModel::updateRow(int row, const QVector<int>& roles)
{
    auto modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex, roles);
}

void RoomEventsModel::updateRow(int row)
{
    auto modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex);
}

void RoomEventsModel::updateEvent(const QString& eventId)
{
    updateRow(findEvent(eventId));
}

int RoomEventsModel::findEvent(const QString& eventId)
{
    auto iter = m_room->findInTimeline(eventId);
    if (iter == m_room->timelineEdge()) {
        qCWarning(logger) << "Non-existing event" << eventId << "replaced";
        return -1;
    }

    return std::distance(m_room->messageEvents().rbegin(), iter)
        + m_pendingEvents;
}

Room* RoomEventsModel::room() const
{
    return m_room;
}

void RoomEventsModel::setRoom(QMatrixClient::Room* room)
{
    if (m_room == room)
        return;

    beginResetModel();

    if (m_room) {
        m_room->disconnect(this);
    }

    m_room = room;

    if (m_room) {
        connect(m_room, &Room::destroyed, this,
            [this]() {
                Q_ASSERT_X(false, "Room::destroyed", "Room was destroyed. Should not happen!");
                setRoom(nullptr);
            });

        connect(m_room, &Room::aboutToAddNewMessages,
            this, &RoomEventsModel::onBeginInsertMessages);
        connect(m_room, &Room::aboutToAddHistoricalMessages,
            this, &RoomEventsModel::onBeginInsertOldMessages);
        connect(m_room, &Room::addedMessages,
            this, [this]() { endInsertRows(); });
        connect(m_room, &Room::replacedEvent,
            this, &RoomEventsModel::onReplacedMessage);

        connect(m_room, &Room::pendingEventAboutToAdd,
            this, [this] { beginInsertRows({}, 0, 0); ++m_pendingEvents; });
        connect(m_room, &Room::pendingEventAdded,
            this, [this] { endInsertRows(); });
        connect(m_room, &Room::pendingEventAboutToDiscard, this,
            [this](int i) { beginRemoveRows({}, i, i); --m_pendingEvents; });
        connect(m_room, &Room::pendingEventDiscarded,
            this, [this] { endRemoveRows(); });
        connect(m_room, &Room::pendingEventAboutToMerge,
            this, [this](const RoomEvent*, int i) { onBeginSyncMessage(i); });
        connect(m_room, &Room::pendingEventMerged,
            this, [this]() { onEndSyncMessage(); });
        connect(m_room, &Room::pendingEventChanged,
            this, [this](int i) { updateRow(i); });

        m_pendingEvents = m_room->pendingEvents().size();
    } else {
        m_pendingEvents = 0;
    }

    endResetModel();

    emit roomChanged();
}

} // namespace Determinant
