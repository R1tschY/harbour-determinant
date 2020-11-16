import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

Rectangle {
    id: roomThumbnail

    property string mediaId
    property string itemName
    property string itemId
    property int size: Theme.itemSizeSmall

    color: stringToColour(itemId)
    radius: Theme.paddingSmall
    width: size
    height: size

    function stringToColour(str) {
        return Qt.hsla(humanize.stringToHue(str), 0.5, 0.4, 0.5)
    }

    Label {
        id: fallback
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        visible: avatarImage.status !== Image.Ready
        text: itemName.charAt(0).toUpperCase()
        font.pixelSize: roomThumbnail.height * 0.7
    }

    Image {
        id: avatarImage
        visible: false
        width: size
        height: size

        source: !!mediaId ? ("image://mtx/" + mediaId) : ""
        sourceSize: Qt.size(size, size)

        fillMode: Image.PreserveAspectCrop
        asynchronous: true
    }

    BusyIndicator {
        id: indicator
        size: BusyIndicatorSize.Medium
        anchors.centerIn: avatarImage
        running: avatarImage.status === Image.Loading
    }

    Rectangle {
        id: mask

        color: "black"
        visible: false
        anchors.fill: avatarImage
        radius: Theme.paddingSmall
    }

    OpacityMask {
        anchors.fill: avatarImage

        source: avatarImage
        maskSource: mask
    }
}
