#ifndef ROOMMODEL_H
#define ROOMMODEL_H

#include <QAbstractListModel>

#include <events/roomevent.h>
#include <room.h>

namespace QMatrixClient {
class RoomMessageEvent;
class RoomMemberEvent;
class RoomCreateEvent;
} // namespace QMatrixClient

namespace Det {

class RoomEventsModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(QMatrixClient::Room* room
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
        HiddenRole,
        ContentTypeRole,
        ContentJsonRole,
    };

public:
    explicit RoomEventsModel(QObject* parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setRoom(QMatrixClient::Room* room);
    QMatrixClient::Room* room() const;
    int pendingEventsCount() const;

signals:
    void roomChanged();

private:
    QMatrixClient::Room* m_room = nullptr;
    int m_pendingEvents = 0;

    bool m_movingEvents = false;

    void onBeginInsertMessages(QMatrixClient::RoomEventsRange events);
    void onBeginInsertOldMessages(QMatrixClient::RoomEventsRange events);
    void onBeginSyncMessage(int i);
    void onEndSyncMessage();
    void onReplacedMessage(const QMatrixClient::RoomEvent* newEvent,
        const QMatrixClient::RoomEvent* oldEvent);

    void updateRow(int row, const QVector<int>& roles);
    void updateRow(int row);
    void updateEvent(const QString& eventId);
    int findEvent(const QString& eventId);
};

} // namespace Det

#endif // ROOMMODEL_H
