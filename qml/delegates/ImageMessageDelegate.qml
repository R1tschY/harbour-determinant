import QtQuick 2.0
import Sailfish.Silica 1.0
import Quotient 0.5
import Determinant 0.1
import Determinant.Qml 0.2
import "../components"

MessageDelegateBase {
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


    ContentThumbnail {
        readonly property bool hasThumbnail: contentJson.info.thumbnail_info

        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        mediaId: urlToMediaId(hasThumbnail ? contentJson.info.thumbnail_url : contentJson.info.url)
        sourceSize: infoToSize(hasThumbnail ? contentJson.info.thumbnail_info : contentJson.info)
    }

    Label {
        id: timeLabel

        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        text: humanize.humanizeBytes(contentJson.info.size) + " • " +
            (ownMessage
              ? (humanize.formatTime(time) + " • "
                 + formatEventStatus(eventStatus))
              : humanize.formatTime(time) + (edited ? " 🖊️" : ""))
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        horizontalAlignment: textAlign
    }
}
