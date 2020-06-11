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

        model: PublicRoomListModel {
            connection: _connection
            server: ""
        }

        header: PageHeader {
            title: qsTr("Public Rooms")
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

                mediaId: avatarMediaId
                itemName: name
                itemId: roomId
                size: Theme.itemSizeSmall
            }

            Label {
                id: nameLabel
                anchors {
                    left: roomThumbnail.right
                    leftMargin: Theme.paddingSmall
                    right: userCountLabel.left
                    rightMargin: Theme.paddingMedium
                    top: parent.top
                    topMargin: Theme.paddingSmall
                }

                textFormat: Text.PlainText
                truncationMode: TruncationMode.Fade
                text: name || "<unnamed>"
                color: delegate.highlighted
                       ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                id: userCountLabel
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingMedium
                }

                truncationMode: TruncationMode.Fade
                text: numJoinedMembers
                font.pixelSize: Theme.fontSizeSmall
                color: delegate.highlighted
                       ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                id: topicLabel
                anchors {
                    left: roomThumbnail.right
                    leftMargin: Theme.paddingSmall
                    top: nameLabel.bottom
                    right: parent.right
                    bottom: parent.bottom
                }

                truncationMode: TruncationMode.Fade
                text: topic
                textFormat: Text.StyledText
                font.pixelSize: Theme.fontSizeSmall
                color: delegate.highlighted
                       ? Theme.secondaryHighlightColor: Theme.secondaryColor
            }
        }

        VerticalScrollDecorator { flickable: listView }
    }
}
