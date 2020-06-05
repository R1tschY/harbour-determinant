import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1

Page {
    id: page

    property var _connection: connection

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    SilicaListView {
        id: listView

        anchors.fill: parent

        spacing: Theme.paddingMedium

        model: ChatsModel {
            connection: _connection
        }

        header: PageHeader {
            title: qsTr("Chats")
        }

        delegate: ListItem {
            id: delegate

            height: Theme.itemSizeSmall
            width: parent.width

            Label {
                id: nameLabel
                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: dateLabel.left
                    rightMargin: Theme.paddingMedium
                }

                truncationMode: TruncationMode.Fade
                text: unreadCount > 0
                      ? "<b>%1</b> %2".arg(unreadCount).arg(displayname)
                      : displayname
                color: delegate.highlighted
                       ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                id: dateLabel
                anchors {
                    top: parent.top
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
                    top: nameLabel.bottom
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                }

                truncationMode: TruncationMode.Fade
                text: lastEvent
                textFormat: Text.StyledText
                font.pixelSize: Theme.fontSizeSmall
                color: delegate.highlighted
                       ? Theme.secondaryHighlightColor: Theme.secondaryColor
            }

            onClicked: pageStack.push(
                           Qt.resolvedUrl("RoomChatPage.qml"),
                           { currentRoom: room })
        }

        VerticalScrollDecorator { flickable: listView }
    }
}
