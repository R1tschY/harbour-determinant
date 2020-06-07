import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1
import QtGraphicalEffects 1.0
import "../components"

Page {
    id: page

    property var _connection: connection

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    SilicaListView {
        id: listView

        anchors.fill: parent

        model: ChatsModel {
            connection: _connection
        }

        header: PageHeader {
            title: qsTr("Chats")
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
                visible: unreadCount > 0
                width: unreadLabelText.contentWidth + 2 * Theme.paddingSmall
                height: unreadLabelText.contentHeight

                Label {
                    id: unreadLabelText
                    x: Theme.paddingSmall

                    truncationMode: TruncationMode.Fade
                    font.weight: Font.Bold
                    text: unreadCount
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

                textFormat: Text.PlainText
                truncationMode: TruncationMode.Fade
                text: displayName
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
                text: lastEvent
                textFormat: Text.StyledText
                font.pixelSize: Theme.fontSizeSmall
                color: delegate.highlighted
                       ? Theme.secondaryHighlightColor: Theme.secondaryColor
            }

            onClicked: pageStack.push(
                           Qt.resolvedUrl("RoomPage.qml"),
                           { currentRoom: room })
        }

        VerticalScrollDecorator { flickable: listView }
    }
}
