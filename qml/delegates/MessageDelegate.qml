import QtQuick 2.0
import Sailfish.Silica 1.0
import Quotient 0.5
import Determinant 0.1
import Determinant.Qml 0.2
import "../components"

Item {
    property int textAlign: ownMessage ? Text.AlignRight : Text.AlignLeft

    width: contentColumn.width
    height: contentColumn.height

//    Rectangle {
//        anchors.fill: contentColumn

//        color: Theme.primaryColor
//        opacity: ownMessage ? 0.05 : 0.1
//        radius: Theme.paddingSmall
//    }

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

    Avatar {
        id: avatarThumbnail

        anchors {
            left: parent.left
            leftMargin: Theme.paddingMedium
        }

        size: Theme.iconSizeMedium
        mediaId: author.avatarMediaId
        itemName: author.name
        itemId: author.id
        visible: showAuthor
    }

//    Rectangle {
//        anchors.fill: contentColumn
//        color: "red"
//        opacity: 0.3
//    }

    Column {
        //property int maxWidth: page.width - Theme.iconSizeMedium - 3 * Theme.paddingMedium

        id: contentColumn
        width: page.width - Theme.iconSizeMedium - 3 * Theme.paddingMedium
//        width: Math.min(Math.max(
//            messageLabel.implicitWidth, timeLabel.implicitWidth,
//            authorLabel.implicitWidth) + 2 * Theme.paddingSmall, maxWidth)
        anchors {
            left: ownMessage ? undefined : parent.left
            right: ownMessage ? parent.right: undefined
            leftMargin: 2 * Theme.paddingMedium + Theme.iconSizeMedium
            rightMargin: Theme.paddingMedium
        }

        Label {
            id: authorLabel
            width: parent.width - 2 * Theme.paddingSmall
            x: Theme.paddingSmall

            text: authorDisplayName
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            color: stringToColour(author.id)
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideMiddle
            visible: !ownMessage && showAuthor

            function stringToColour(str) {
                return Qt.hsla(humanize.stringToHue(str), 0.5, 0.5, 1.0)
            }
        }

        Label {
            id: messageLabel
            width: parent.width - 2 * Theme.paddingSmall
            x: Theme.paddingSmall

            text: customMessageCss + display
            textFormat: Text.RichText
            font.pixelSize: {
                var len = StringUtils.getEmoijChainLength(display)
                if (len === 0 || len > 6) {
                    return Theme.fontSizeSmall
                } else if (len > 4) {
                    return Theme.fontSizeMedium
                } else if (len > 2) {
                    return Theme.fontSizeLarge
                } else {
                    return Theme.fontSizeExtraLarge
                }
            }
            color: Theme.primaryColor
            wrapMode: Text.Wrap
            horizontalAlignment: textAlign

            onLinkActivated: page.openLink(link)
        }

        Label {
            id: timeLabel
            width: parent.width - 2 * Theme.paddingSmall
            x: Theme.paddingSmall

            text: ownMessage
                  ? (humanize.formatTime(time) + " • "
                     + formatEventStatus(eventStatus))
                  : humanize.formatTime(time) + (edited ? " 🖊️" : "")
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeExtraSmall
            horizontalAlignment: textAlign
        }
    }
}
