import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property bool loading: false

    id: page
    allowedOrientations: Orientation.All

    Connections {
        target: connection

        onLoginError: {
            if (error.indexOf("Forbidden") !== -1) {
                passwordField.text = ""
                console.info("Wrong password given")
                loading = false
                // TODO: show error
            } else {
                console.error("Same other login error")
            }
        }

        onConnected: {
            pageStack.replace(Qt.resolvedUrl("RoomListPage.qml"))
        }
    }

    Timer {
        id: timer
    }

    function delay(delayTime, cb) {
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }

    function login(user, pass, server) {
        if (!server) server = "https://matrix.org"
        connection.setHomeserver(server)
        connection.connectToServer(user, pass, connection.deviceId)
    }

    SilicaFlickable {
        anchors.fill: parent

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Matrix Login")
            }

            TextField {
                id: serverField
                label: "Server"
                placeholderText: label
                width: parent.width
                text: "https://matrix.org"

                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: usernameField.focus = true
            }

            TextField {
                id: usernameField
                label: "Username"
                placeholderText: label
                width: parent.width

                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: passwordField.focus = true
            }

            PasswordField {
                id: passwordField
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked:
                    login(
                        usernameField.text,
                        passwordField.text,
                        serverField.text)
            }
        }

        VerticalScrollDecorator { }
    }
}
