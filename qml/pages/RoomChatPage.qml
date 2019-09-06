import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1


Page {
    id: page

    property QtObject currentRoom: null

    allowedOrientations: Orientation.All

    function contentItemSource(contentType) {
        if (contentType === "m.text"
                || contentType === "m.notice"
                || contentType === "m.emote") {
            return "../delegates/MessageDelegate.qml"
        }

        return "../delegates/MessageDelegate.qml"
    }

    SilicaListView {
        readonly property bool noMoreContent:
            !currentRoom
            || currentRoom.eventsHistoryJob
            || currentRoom.allHistoryLoaded

        id: eventListView
        anchors {
            left: parent.left
            right: parent.right
            top: page.top
            bottom: input.top
        }

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

            property int textAlign: (author && author.id === connection.localUserId)
                                     ? Text.AlignRight
                                     : Text.AlignLeft

            contentHeight: contentColumn.height

            Column {
                id: contentColumn
                width: parent.width

                Loader {
                    id: contentLoader
                    source: contentItemSource(contentType)
                }

                Label {
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    x: Theme.horizontalPageMargin
                    text: author ? author.displayName : ""
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    horizontalAlignment: textAlign
                }
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
