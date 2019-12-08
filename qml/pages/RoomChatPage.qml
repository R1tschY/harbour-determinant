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

    Component {
        id: sectionHeader
        SectionHeader {
            text: Humanize.formatDate(section)
            horizontalAlignment: Text.AlignHCenter
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

        //spacing: Theme.paddingMedium
        verticalLayoutDirection: ListView.BottomToTop
        flickableDirection: Flickable.VerticalFlick
        quickScroll: false
        clip: true

        model: RoomEventsModel {
            room: currentRoom
        }

        section {
            property: "date"
            criteria: ViewSection.FullString
        }

        // TODO: footer: LoadIndicator { visible: currentRoom && currentRoom.eventsHistoryJob }

        delegate: ListItem {
            id: listItem
            // LayoutMirroring.childrenInherit: true
            // LayoutMirroring.enabled: author && author.id === connection.localUserId

            property int ownMessage:
                author && author.id === connection.localUserId

            property bool sectionBoundary: (
                ListView.nextSection !== "" && ListView.nextSection !== ListView.section)
                || model.index === eventListView.count - 1

            contentHeight: contentLoader.y + contentLoader.height

            property var modelSection: listItem.ListView.section

            Loader {
                id: sectionHeaderLoader

                property var section: listItem.ListView.section

                sourceComponent: sectionBoundary ? sectionHeader : undefined
                width: parent.width
            }

            Loader {
                id: contentLoader
                source: contentItemSource(eventType)
                y: sectionBoundary
                   ? sectionHeaderLoader.y + sectionHeaderLoader.height
                   : 0
                width: parent.width
            }
        }

        function ensureHistoryContent() {
            if (!noMoreContent && contentY - originY < 5000)
                currentRoom.getPreviousContent(21);
        }

        onContentYChanged: {
            var item = eventListView.itemAt(0, contentY)
            if (item) {
                sectionOverlay.text = Humanize.formatDate(item.modelSection)
            }

            ensureHistoryContent()
        }
        onContentHeightChanged: ensureHistoryContent()
        Component.onCompleted: {
            sectionOverlay.text = Humanize.formatDate(
                eventListView.itemAt(0, contentY).modelSection)

            if (eventListView.count < 21 && !noMoreContent)
                currentRoom.getPreviousContent(21 - eventListView.count);
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
