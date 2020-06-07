import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: roomThumbnail

    property string mediaId
    property string itemName
    property string itemId

    color: stringToColour(itemId)
    radius: Theme.paddingSmall
    width: Theme.itemSizeSmall
    height: Theme.itemSizeSmall

    function stringToColour(str) {
        return Qt.hsla(humanize.stringToHue(str), 0.5, 0.4, 0.5)
    }

    Label {
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        visible: !avatar
        text: itemName.charAt(0).toUpperCase()
        font.pixelSize: roomThumbnail.height * 0.7
    }

    Image {
        id: avatarImage
        visible: !!avatar
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall

        source: "image://mtx/" + avatar
        sourceSize: Qt.size(Theme.itemSizeSmall, Theme.itemSizeSmall)

        fillMode: Image.PreserveAspectCrop
    }

//                OpacityMask {
//                    visible: !!avatar
//                    anchors.fill: bug
//                    source: avatarImage
//                    maskSource: roomThumbnail
//                }
}
