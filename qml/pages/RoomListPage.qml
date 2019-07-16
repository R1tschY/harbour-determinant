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
        model: ChatsModel {
            connection: _connection
        }

        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Chats")
        }

        delegate: BackgroundItem {
            id: delegate

            Label {
                x: Theme.horizontalPageMargin
                text: displayname
                anchors.verticalCenter: parent.verticalCenter
                color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
            }
            onClicked: pageStack.push(
                           Qt.resolvedUrl("RoomChatPage.qml"),
                           { currentRoom: room })
        }

        VerticalScrollDecorator { flickable: listView }
    }
}
