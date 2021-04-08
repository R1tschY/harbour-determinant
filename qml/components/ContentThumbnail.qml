import QtQuick 2.6
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

Item {
    id: thumbnail

    property alias source: thumbnailImage.source
    property alias sourceSize: thumbnailImage.sourceSize
    property bool highlighted

    height: parent.width

    Image {
        id: thumbnailImage
        visible: false
        width: parent.width
        height: parent.width

        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        autoTransform: true
    }

    Rectangle {
        id: placeholder

        anchors.fill: thumbnailImage
        visible: thumbnailImage.status !== Image.Ready

        color: highlighted
            ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            : (Theme.colorScheme === Theme.DarkOnLight
                ? Theme.rgba(Theme.highlightColor, Theme.opacityFaint)
                : Theme.rgba(Theme.primaryColor, Theme.opacityFaint))
        radius: Theme.paddingLarge

        Image {
            visible: thumbnailImage.status === Image.Error
            anchors.centerIn: placeholder
            source: "image://theme/icon-m-warning"
        }
    }

    Rectangle {
        id: mask

        color: "black"
        visible: false
        opacity: 0.3
        anchors.fill: thumbnailImage
        radius: Theme.paddingLarge
    }

    OpacityMask {
        id: roundedThumbnail
        anchors.fill: thumbnailImage
        opacity: status === Image.Ready ? 1.0 : 0.0

        source: thumbnailImage
        maskSource: mask

        layer {
            enabled: thumbnail.highlighted
            effect: PressEffect { source: roundedThumbnail }
        }

        Behavior on opacity { FadeAnimation { } }
    }

    BusyIndicator {
        id: indicator
        size: BusyIndicatorSize.Medium
        anchors.centerIn: thumbnailImage
        running: thumbnailImage.status === Image.Loading
    }
}
