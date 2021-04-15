import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.2
import QtGraphicalEffects 1.0
import "../components"

Page {
    id: page
    objectName: "RoomListPage"

    property QtObject _connection: connection

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    function createAttentionLabel(joinState, notificationCount) {
        var result = ""

        if (joinState === "invite") {
            result += "!"
        }

        if (notificationCount > 0) {
            result += notificationCount
        }

        return result
    }

    SilicaListView {
        id: listView

        anchors.fill: parent

        model: SortFilterModel{
            sourceModel: RoomListModel {
                connection: _connection
            }

            invertFilter: true
            filterRole: "joinState"
            filterValue: "leave"

            sortAscending: false
            sortRole: "lastActivity"
        }

        header: PageHeader {
            title: qsTr("Rooms")
        }

        delegate: ListItem {
            id: delegate

            contentHeight: Theme.itemSizeSmall + 2 * Theme.paddingSmall
            x: Theme.paddingMedium
            width: parent.width - Theme.paddingMedium

            Avatar {
                id: roomThumbnail
                anchors {
                    left: parent.left
                    top: parent.top
                    topMargin: Theme.paddingSmall
                }

                mediaId: avatar
                itemName: displayName
                itemId: roomId
                size: Theme.itemSizeSmall
            }

            Rectangle {
                id: unreadLabel

                anchors {
                    left: roomThumbnail.right
                    leftMargin: Theme.paddingSmall
                    top: parent.top
                    topMargin: Theme.paddingSmall
                }

                radius: Theme.paddingSmall
                color: Theme.highlightBackgroundColor
                visible: notificationCount > 0 || joinState === "invite"
                width: unreadLabelText.contentWidth + 2 * Theme.paddingSmall
                height: unreadLabelText.contentHeight

                Label {
                    id: unreadLabelText
                    x: Theme.paddingSmall

                    truncationMode: TruncationMode.Fade
                    font.weight: Font.Bold
                    text: createAttentionLabel(joinState, notificationCount)
                    textFormat: Text.PlainText
                    color: Theme.primaryColor
                }
            }

            Label {
                id: nameLabel
                anchors {
                    left: unreadLabel.visible ? unreadLabel.right : roomThumbnail.right
                    leftMargin: Theme.paddingSmall
                    right: dateLabel.left
                    rightMargin: Theme.paddingMedium
                    top: parent.top
                    topMargin: Theme.paddingSmall
                }

                textFormat: Text.StyledText
                truncationMode: TruncationMode.Fade
                text: EmojiParser.parse(displayName, Theme.fontSizeMedium)
                color: delegate.highlighted
                       ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                id: dateLabel
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingMedium
                }

                truncationMode: TruncationMode.Fade
                text: humanize.formatDateTime(lastActivity)
                font.pixelSize: Theme.fontSizeSmall
                textFormat: Text.PlainText
                color: delegate.highlighted
                       ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                id: messageLabel
                anchors {
                    left: roomThumbnail.right
                    leftMargin: Theme.paddingSmall
                    top: nameLabel.bottom
                    right: parent.right
                    bottom: parent.bottom
                }

                truncationMode: TruncationMode.Fade
                text: joinState === "invite"
                    ? "<i>" + qsTr("Invitation") + "</i>"
                    : EmojiParser.parse(lastEvent, Theme.fontSizeSmall)
                textFormat: Text.StyledText
                font.pixelSize: Theme.fontSizeSmall
                color: delegate.highlighted
                       ? Theme.secondaryHighlightColor: Theme.secondaryColor
            }

            onClicked: pageStack.push(
                           Qt.resolvedUrl("RoomPage.qml"),
                           { currentRoom: room })
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Logout")
                onClicked: connection.logout()
            }
//            MenuItem {
//                text: "Public rooms"
//                onClicked: pageStack.push(
//                               Qt.resolvedUrl("PublicRoomListPage.qml"))
//            }
            //MenuLabel { text: "Informational label" }
        }

        VerticalScrollDecorator { flickable: listView }
    }
}
