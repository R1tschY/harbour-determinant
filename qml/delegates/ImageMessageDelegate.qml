import QtQuick 2.0
import Sailfish.Silica 1.0
import Quotient 0.5
import Determinant 0.1
import Determinant.Qml 0.2
import "../components"

MessageDelegateBase {
    id: imageMsg

    readonly property bool hasThumbnail: contentJson.info.thumbnail_info
    readonly property bool isUpload: fileTransferInfo && fileTransferInfo.isUpload
    readonly property bool isCompleted: fileTransferInfo && fileTransferInfo.completed
    readonly property bool isInProgress: fileTransferInfo && fileTransferInfo.started
    readonly property bool isFailed: fileTransferInfo && fileTransferInfo.failed
    readonly property bool needsDownload: !isUpload && !isCompleted

    function formatEventStatus(eventStatus) {
        if (eventStatus & EventStatus.Redacted) {
            return "✝"
        }
        if (eventStatus & EventStatus.Hidden) {
            return "❏"
        }
        if (eventStatus === EventStatus.ReachedServer) {
            return "✓"
        }
        if (eventStatus === EventStatus.SendingFailed) {
            return "✗"
        }
        return "◌"
    }

    function urlToMediaId(url) {
        if (!url)
            return ""

        var parts = url.split("/")
        if (parts.length === 4 && parts[0] === "mxc:") {
            return parts[2] + "/" + parts[3]
        }
        return ""
    }

    function infoToSize(info) {
        if (info) {
            return Qt.size(info.w, info.h)
        }
    }

    Item {
        height: Theme.paddingSmall
        width: parent.width
    }

    ContentThumbnail {
        id: thumbnail

        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        highlighted: listItem.highlighted

        source: (imageMsg.isUpload)
            ? fileTransferInfo.localPath
            : hasThumbnail
                ? ("image://mxc-thumbnail/" + urlToMediaId(contentJson.info.thumbnail_url))
                : ""
        sourceSize: Qt.size(500, 500)

        Item {
            width: Theme.itemSizeMedium
            height: Theme.itemSizeMedium
            anchors.centerIn: parent
            visible: needsDownload

            Rectangle {
                anchors.fill: parent

                radius: width
                color: Theme.overlayBackgroundColor
                opacity: Theme.opacityOverlay
            }

            Icon {
                id: actionImage
                source: isInProgress
                        ? "image://theme/icon-m-cancel"
                            : isFailed
                            ? "image://theme/icon-m-update"
                            : "image://theme/icon-m-cloud-download"

                anchors.centerIn: parent
                highlighted: listItem.highlighted
            }

            ProgressCircle {
                value: fileTransferInfo && fileTransferInfo.total > 0
                       ? (fileTransferInfo.progress / fileTransferInfo.total)
                       : 0
                visible: imageMsg.isInProgress

                anchors.centerIn: parent
            }
        }

        MouseArea {
            id: actionButton
            anchors.fill: thumbnail

            onClicked: {
                if (imageMsg.isInProgress) {
                    room.cancelFileTransfer(eventId)
                } else if (imageMsg.isCompleted || imageMsg.isUpload) {
                    var props = {
                        room: room,
                        eventId: eventId,
                        source: fileTransferInfo.localPath
                    }
                    pageStack.push(Qt.resolvedUrl("../pages/ImageViewPage.qml"), props)
                } else {
                    room.downloadFile(eventId)
                }
            }
        }
    }

    Label {
        id: timeLabel

        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        text: humanize.humanizeBytes(contentJson.info.size) + " • " + humanize.humanizeBytes(contentJson.info.thumbnail_info.size) + " • " +
            (ownMessage
              ? (humanize.formatTime(time) + " • "
                 + formatEventStatus(eventStatus))
              : humanize.formatTime(time) + (edited ? " 🖊️" : ""))
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        horizontalAlignment: textAlign
    }
}
