import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1


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

    onStatusChanged: {
        if (status === PageStatus.Active && !pageStack.nextPage(page)) {
            pageStack.pushAttached(
                Qt.resolvedUrl("RoomDetailsPage.qml"),
                { "currentRoom": currentRoom })
        }
    }

    SilicaListView {
        readonly property bool noMoreContent:
            !currentRoom
            || !!currentRoom.eventsHistoryJob
            || !!currentRoom.allHistoryLoaded

        id: eventListView
        anchors {
            left: parent.left
            right: parent.right
            top: page.top
            bottom: input.top
        }

        spacing: Theme.paddingMedium
        verticalLayoutDirection: ListView.BottomToTop
        flickableDirection: Flickable.VerticalFlick
        clip: true

        model: RoomEventsModel {
            room: currentRoom
        }

        // TODO: footer: LoadIndicator { visible: currentRoom && currentRoom.eventsHistoryJob }

        delegate: ListItem {
            // LayoutMirroring.childrenInherit: true
            // LayoutMirroring.enabled: author && author.id === connection.localUserId

            property int ownMessage:
                author && author.id === connection.localUserId

            contentHeight: contentLoader.height

            Loader {
                id: contentLoader
                source: contentItemSource(eventType)
            }
        }

        function ensureHistoryContent() {
            if (!noMoreContent && contentY - 5000 < originY)
                currentRoom.getPreviousContent(21);
        }

        onContentYChanged: ensureHistoryContent()
        onContentHeightChanged: ensureHistoryContent()
    }

    Row {
        id: input

        readonly property bool nonEmptyInput: inputEdit.text.trim().length !== 0

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        TextArea {
            id: inputEdit
            width: parent.width - Theme.itemSizeSmall
            placeholderText: qsTr("Message")
            label: currentRoom.displayName
            labelVisible: true
            hideLabelOnEmptyField: false
        }


        IconButton {
            id: inputButton
            icon.source: "image://theme/icon-m-send"
            enabled: input.nonEmptyInput
//            icon.source: nonEmptyInput
//                           ? "image://theme/icon-m-send"
//                           : "image://theme/icon-m-attach"

            onClicked: {
                if (!currentRoom) return
                if (!input.nonEmptyInput) return

                currentRoom.postPlainText(inputEdit.text.trim())
                inputEdit.text = ""
            }
        }
    }

    VerticalScrollDecorator { flickable: eventListView }
}
