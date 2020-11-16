import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    function login(user, pass, server) {
        if (!server) {
            server = "https://matrix.org"
        }

        pageStack.push(Qt.resolvedUrl("LoginProgressPage.qml"), {
            "user": user, "password": pass, "server": server
        })
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

            Rectangle {
                color: "white"
                height: Theme.itemSizeExtraLarge + 2 * Theme.paddingMedium
                width: parent.width

                Image {
                    id: logo
                    source: "qrc:det/icons/matrix-logo.svg"
                    y: Theme.paddingMedium
                    height: Theme.itemSizeExtraLarge
                    width: parent.width

                    fillMode: Image.PreserveAspectFit
                    asynchronous: true

                    // Dirty SVG scaling trick
                    sourceSize: Qt.size(
                        hiddenImg.sourceSize.width * Theme.itemSizeExtraLarge / hiddenImg.sourceSize.height, Theme.itemSizeExtraLarge)
                    Image {
                        id: hiddenImg
                        source: parent.source
                        width: 0
                        height: 0
                    }
                }
            }

            TextField {
                id: serverField
                label: qsTr("Server")
                placeholderText: label
                width: parent.width
                text: "https://matrix.org"

                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: usernameField.focus = true
                inputMethodHints: Qt.ImhUrlCharactersOnly
                    | Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
                validator: RegExpValidator {
                    regExp: /(https?:\/\/.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)/g
                }
            }

            TextField {
                id: usernameField
                label: qsTr("Username")
                placeholderText: label
                width: parent.width

                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: passwordField.focus = true
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhLowercaseOnly
                    | Qt.ImhNoPredictiveText
            }

            PasswordField {
                id: passwordField

                EnterKey.enabled: text.length > 0
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
