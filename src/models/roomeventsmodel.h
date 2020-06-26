#ifndef ROOMMODEL_H
#define ROOMMODEL_H

#include <QAbstractListModel>

#include <events/roomevent.h>
#include <room.h>

namespace Quotient {
class RoomMessageEvent;
class RoomMemberEvent;
class RoomCreateEvent;
} // namespace Quotient

namespace Det {

class RoomEventsModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(Quotient::Room* room
            READ room WRITE setRoom NOTIFY roomChanged)

    enum {
        DisplayRole = Qt::UserRole,
        AuthorRole,
        AuthorDisplayNameRole,
        EventIdRole,
        MatrixTypeRole,
        EventTypeRole,
        EventStatusRole,
        AnnotationRole,
        DateTimeRole,
        DateRole,
        TimeRole,
        EditedRole,
        HiddenRole,
        ContentTypeRole,
        ContentJsonRole,
        ShowAuthorRole,
        ReadMarkerRole,
    };

public:
    explicit RoomEventsModel(QObject* parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setRoom(Quotient::Room* room);
    Quotient::Room* room() const;
    inline int pendingEventCount() const {
        return int(m_room->pendingEvents().size());
    }

signals:
    void roomChanged();

private:
    Quotient::Room* m_room = nullptr;

    bool m_movingEvents = false;

    void onBeginInsertMessages(Quotient::RoomEventsRange events);
    void onBeginInsertOldMessages(Quotient::RoomEventsRange events);
    void onBeginSyncMessage(int i);
    void onEndSyncMessage();
    void onReplacedMessage(const Quotient::RoomEvent* newEvent,
        const Quotient::RoomEvent* oldEvent);

    void updateRow(int row, const QVector<int>& roles);
    void updateRow(int row);
    void updateEvent(const QString& eventId);
    int findEvent(const QString& eventId);
};

} // namespace Det

#endif // ROOMMODEL_H
