import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page
    objectName: "RoomPage"

    property QtObject currentRoom: null

    allowedOrientations: Orientation.All

    function contentItemSource(eventType) {
        switch (eventType) {
            case "state": return "../delegates/StateMessageDelegate.qml"
            case "image": return "../delegates/ImageMessageDelegate.qml"
            default: return "../delegates/TextMessageDelegate.qml"
        }
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


    EventsView {
        id: eventListView

        anchors {
            left: parent.left
            right: parent.right
            top: page.top
            bottom: input.top
        }

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
