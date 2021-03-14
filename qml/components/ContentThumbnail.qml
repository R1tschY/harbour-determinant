import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

Item {
    id: thumbnail

    property string mediaId
    property alias sourceSize: thumbnailImage.sourceSize

    height: parent.width

    Image {
        id: thumbnailImage
        visible: false
        width: parent.width
        height: parent.width

        source: !!mediaId ? ("image://mxc-thumbnail/" + mediaId) : ""

        fillMode: Image.PreserveAspectCrop
        asynchronous: true
    }

    BusyIndicator {
        id: indicator
        size: BusyIndicatorSize.Medium
        anchors.centerIn: thumbnailImage
        running: thumbnailImage.status === Image.Loading
    }

    Rectangle {
        id: mask

        color: "black"
        visible: false
        anchors.fill: thumbnailImage
        radius: Theme.paddingLarge
    }

    OpacityMask {
        anchors.fill: thumbnailImage

        source: thumbnailImage
        maskSource: mask
    }
}
