import QtQuick 2.0
import Sailfish.Silica 1.0
import Quotient 0.5
import Determinant 0.1
import Determinant.Qml 0.2
import "../components"

Item {
    property int textAlign: ownMessage ? Text.AlignRight : Text.AlignLeft
    default property alias content: contentColumn.children

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

    Rectangle {
        anchors.fill: contentColumn
        color: listItem.highlighted
            ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            : (Theme.colorScheme === Theme.DarkOnLight
                ? Theme.rgba(Theme.highlightColor, Theme.opacityFaint)
                : Theme.rgba(Theme.primaryColor, Theme.opacityFaint))
        opacity: listItem.highlighted
                 ? Theme.opacityHigh
                 : (ownMessage ? Theme.opacityFaint : Theme.opacityHigh)
        radius: Theme.paddingMedium
    }

    Column {
        //property int maxWidth: page.width - Theme.iconSizeMedium - 3 * Theme.paddingMedium

        id: contentColumn
        width: page.width - Theme.iconSizeMedium - 3 * Theme.paddingMedium
//        width: Math.max(
//            messageLabel.implicitWidth, timeLabel.implicitWidth) + 2 * Theme.paddingSmall
        anchors {
            left: ownMessage ? undefined : parent.left
            right: ownMessage ? parent.right : undefined
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

        // here base classes can add content
    }
}
