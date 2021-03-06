import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    property string user
    property string password
    property string server

    onStatusChanged: {
        if (status === PageStatus.Active) {
            connection.setHomeserver(server)
            connection.connectToServer(user, password, connection.deviceId)
        }
    }

    Connections {
        target: connection

        onLoginError: {
            pageStack.replace(Qt.resolvedUrl("ErrorPage.qml"), {
                "header": qsTr("Login Error"),
                "text": message
            }, PageStackAction.Immediate)
        }

        onResolveError: {
            pageStack.replace(Qt.resolvedUrl("ErrorPage.qml"), {
                "header": qsTr("Login Error"),
                "text": error
            }, PageStackAction.Immediate)
        }

        onConnected: {
            pageStack.replaceAbove(null, Qt.resolvedUrl("RoomListPage.qml"))
        }
    }

    PageBusyIndicator {
        id: busyIndicator
        size: BusyIndicatorSize.Large
        running: true
    }

    Label {
        width: page.width
        text: qsTr("Logging in …")
        anchors.top: busyIndicator.bottom
        anchors.topMargin: Theme.paddingLarge
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        color: Theme.secondaryHighlightColor
        font.pixelSize: Theme.fontSizeLarge
    }
}
