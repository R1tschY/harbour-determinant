import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    id: input

    property QtObject room
    property alias text: inputEdit.text

    readonly property bool nonEmptyInput: inputEdit.text.trim().length !== 0

    signal postPlainText(string text)

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
            if (!input.nonEmptyInput) return

            input.postPlainText(input.text.trim())

            input.text = ""
        }
    }
}
