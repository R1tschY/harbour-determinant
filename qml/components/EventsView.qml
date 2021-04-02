import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1
import "../delegates"

SilicaListView {
    property QtObject room
    readonly property bool noMoreContent:
        !room
        || !!room.eventsHistoryJob
        || !!room.allHistoryLoaded
    property string showUnreadMarkerId: ""
    property bool _completed: false

    id: eventsView

    verticalLayoutDirection: ListView.BottomToTop
    flickableDirection: Flickable.VerticalFlick
    quickScroll: false
    clip: true

    model: SortFilterModel {
        id: filteredModel

        sourceModel: RoomEventsModel {
            room: eventsView.room
        }

        filterRole: "hidden"
        filterValue: false
    }

    section {
        property: "date"
        criteria: ViewSection.FullString
    }

    Component {
        id: sectionHeader
        SectionHeader {
            text: humanize.formatDate(section)
            horizontalAlignment: Text.AlignHCenter
        }
    }

    // TODO: footer: LoadIndicator { visible: currentRoom && currentRoom.eventsHistoryJob }

    delegate: EventDelegate { }

    function ensureHistoryContent() {
        if (!_completed)
            return;

        if (!noMoreContent && contentY - originY < 5000)
            room.getPreviousContent(21);
    }

    function checkToMarkAsRead() {
        // has unread messages
        if (room.unreadCount <= 0)
            return;
        // is at latest message
        if (!atYEnd)
            return;
        // application is in foreground
        if (Qt.application.state !== Qt.ApplicationActive)
            return;
        if (!_completed)
            return;

        console.log("Room: markAllMessagesAsRead", room.unreadCount)
        room.markAllMessagesAsRead()
    }

    function updateDateOverlay() {
        if (!_completed)
            return;

        var item = eventsView.itemAt(0, contentY)
        if (item) {
            sectionOverlay.text = humanize.formatDate(item.modelSection)
        }
    }

    function updateUnreadMarker() {
        if (room.unreadCount > 0) {
            showUnreadMarkerId = room.readMarkerEventId
            eventsView.positionViewAtIndex(room.unreadCount - 1, ListView.End)
        } else {
            showUnreadMarkerId = ""
        }
    }

    onContentYChanged: {
        updateDateOverlay()
        ensureHistoryContent()
    }
    onContentHeightChanged: {
        updateDateOverlay()
        ensureHistoryContent()
    }

    onAtYEndChanged: checkToMarkAsRead()

    Connections {
        target: room
        onNotificationCountChanged: checkToMarkAsRead()
    }

    Connections {
        target: Qt.application
        onStateChanged: {
            if (Qt.application.state === Qt.ApplicationActive) {
                updateUnreadMarker()
                checkToMarkAsRead()
            }
        }
    }

    Binding {
        target: room
        property: "displayed"
        value: Qt.application.state === Qt.ApplicationActive && pageStack.currentPage === page
    }

    Component.onCompleted: {
        // load history when needed
        if (eventsView.count < 21 && !noMoreContent)
            room.getPreviousContent(21 - eventsView.count);

        // read marker
        updateUnreadMarker()

        eventsView.positionViewAtIndex(room.unreadCount - 1, ListView.End)
        _completed = true

        checkToMarkAsRead()
        updateDateOverlay()
    }

    Rectangle {
        color: Theme.highlightBackgroundColor
        opacity: 0.8
        width: Theme.itemSizeExtraLarge
        height: sectionOverlay.contentHeight
        y: Theme.itemSizeSmall / 4
        radius: Theme.itemSizeSmall
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            id: sectionOverlay
            anchors.fill: parent
            width: parent.width

            horizontalAlignment: Text.AlignHCenter
            color: Theme.primaryColor
            y: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeSmall
        }
    }
}
