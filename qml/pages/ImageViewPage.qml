import QtQuick 2.6
import Sailfish.Silica 1.0

Page {
    id: page

    property QtObject room
    property string eventId
    property alias source: image.source

    readonly property int maxSize: Math.max(Screen.width, Screen.height) * 5

    allowedOrientations: Orientation.All

    onOrientationChanged: container.refit()

    SilicaFlickable {
        id: container

        property int lastWidth
        property int lastHeight
        property real minimumScale: 1
        property real maximumScale: 5 // TODO: compute from image.implicitWidth/Height
        property int initialWidth
        property int initialHeight
        property real currentScale: 1

        anchors.fill: parent
        clip: true
        quickScrollEnabled: false
        interactive: currentScale !== 1

        PageBusyIndicator {
            running: image.status === Image.Loading
        }

        ViewPlaceholder {
            visible: image.status === Image.Error
            text: qsTr("Failed to load image")
        }

        Component.onCompleted: {
            contentWidth = width
            contentHeight = height
            lastWidth = width
            lastHeight = height
        }

        Image {
            id: image

            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            opacity: status === Image.Ready ? 1.0 : 0.0
            asynchronous: true
            autoTransform: true
            cache: false
            mipmap: true
            sourceSize.height: page.maxSize
            sourceSize.width: page.maxSize

            Behavior on opacity { FadeAnimation { } }

            onStatusChanged: {
                if (status === Image.Ready) {
                    container.initialWidth = image.paintedWidth
                    container.initialHeight = image.paintedHeight
                }
            }
        }

        PinchArea {
            id: pinchArea

            property real lastScale: 1

            anchors.fill: parent
            enabled: image.status === Image.Ready
            pinch.dragAxis: Pinch.NoDrag

            onPinchStarted: {
                lastScale = container.currentScale
            }

            onPinchUpdated: {
                container.scale(lastScale * pinch.scale, pinch.center)
            }

            onPinchFinished: {
                container.returnToBounds()
            }
        }

        VerticalScrollDecorator { flickable: container }
        HorizontalScrollDecorator { flickable: container }

        function clampScale(scale) {
            return Math.min(Math.max(scale, minimumScale), maximumScale)
        }

        function scale(newScale, scaleCenter) {
            newScale = clampScale(newScale)
            currentScale = newScale
            resizeContent(
                Math.max(initialWidth * newScale, width),
                Math.max(initialHeight * newScale, height),
                scaleCenter)
        }

        function refit() {
            var centerX = contentX + lastWidth / 2
            var centerY = contentY + lastHeight / 2
            resizeContent(
                Math.max(initialWidth * currentScale, height),
                Math.max(initialHeight * currentScale, width),
                Qt.size(centerX, centerY))
            returnToBounds()
            lastWidth = height
            lastHeight = width
        }
    }
}
