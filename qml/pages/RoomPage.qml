import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    property QtObject currentRoom: null

    allowedOrientations: Orientation.All

    function contentItemSource(eventType) {
        if (eventType === "state") {
            return "../delegates/StateMessageDelegate.qml"
        }

        return "../delegates/MessageDelegate.qml"
    }

    function openLink(url) {
        if (url.match(/^(@|https:\/\/matrix.to\/#\/@|matrix:user\/)/)) {

        } else if (url.match(/^(?:https?:\/\/|mailto:)/)) {
            // TODO: open Page to show URL and confirm
            // TODO: detect: https://matrix.to/#/@USER:SERVER
            // TODO: detect: https://matrix.to/#/!qcAsnnvBmpOPQJqViM:matrix.org/$-a8TY2U6FadTq3TP8rU6-NIDrhpxvglMl-egCfaaQvM?via=matrix.org
            console.info("open link external: " + url)
            Qt.openUrlExternally(url)
        } else {
            console.info("unknown scheme in URL: " + url)
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active && !pageStack.nextPage(page)) {
            pageStack.pushAttached(
                Qt.resolvedUrl("RoomDetailsPage.qml"),
                { "currentRoom": currentRoom })
        }
    }

    Component.onCompleted: {
        currentRoom.displayed = true
    }

    Component.onDestroyed: {
        currentRoom.displayed = false
    }

    EventsView {
        id: eventListView

        room: currentRoom
    }

    MessageInput {
        id: input

        room: currentRoom

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        onPostPlainText: {
            if (!currentRoom) return

            currentRoom.postPlainText(text)
            eventListView.positionViewAtBeginning()
        }
    }

    VerticalScrollDecorator { flickable: eventListView }
}
