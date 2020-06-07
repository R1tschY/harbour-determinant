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
    property string showUnreadMarkerId: "<???>"

    id: eventsView
    anchors {
        left: parent.left
        right: parent.right
        top: page.top
        bottom: input.top
    }

    spacing: Theme.paddingMedium
    verticalLayoutDirection: ListView.BottomToTop
    flickableDirection: Flickable.VerticalFlick
    quickScroll: false
    clip: true

    model: RoomEventsModel {
        room: eventsView.room
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
        if (!noMoreContent && contentY - originY < 5000)
            room.getPreviousContent(21);
    }

    onContentYChanged: {
        var item = eventsView.itemAt(0, contentY)
        if (item) {
            sectionOverlay.text = humanize.formatDate(item.modelSection)
        }

        ensureHistoryContent()
    }
    onContentHeightChanged: ensureHistoryContent()

    Component.onCompleted: {
        // set inital section overlay
        var item = eventsView.itemAt(0, contentY)
        if (item) {
            sectionOverlay.text = humanize.formatDate(item.modelSection)
        }

        // load history when needed
        if (eventsView.count < 21 && !noMoreContent)
            room.getPreviousContent(21 - eventsView.count);

        // read marker
        if (room.unreadCount >= 0) {
            showUnreadMarkerId = room.readMarkerEventId
        } else {
            showUnreadMarkerId = "<???>"
        }

        eventsView.positionViewAtIndex(
            room.unreadCount - 1, ListView.End)
        room.markAllMessagesAsRead()
    }

    Rectangle {
        color: Theme.primaryColor
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
            color: Theme.highlightColor
            y: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeSmall
        }
    }
}
