import QtQuick 2.0
import Sailfish.Silica 1.0
import Quotient 0.5
import Determinant 0.2
import Determinant.Qml 0.2
import "../components"

Item {
    property int textAlign: ownMessage ? Text.AlignRight : Text.AlignLeft
    default property alias content: contentColumn.children

    width: contentColumn.width
    height: contentColumn.height

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

            text: EmojiParser.parseText(authorDisplayName, Theme.fontSizeSmall)
            textFormat: Text.StyledText
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
