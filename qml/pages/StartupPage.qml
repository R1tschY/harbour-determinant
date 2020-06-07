import QtQuick 2.0
import Sailfish.Silica 1.0

Page {

    PageBusyIndicator {
        running: true
    }

    Timer {
        id: timer
        repeat: false
        interval: 100

        onTriggered: {
            console.log(connection.localUserId)
            if (connection.localUserId) {
              pageStack.replace(Qt.resolvedUrl("RoomListPage.qml"), {},
                                PageStackAction.Immediate)
            } else {
              pageStack.replace(Qt.resolvedUrl("LoginPage.qml"), {},
                                PageStackAction.Immediate)
            }
        }

        Component.onCompleted: {
            timer.start()
        }
    }
}
