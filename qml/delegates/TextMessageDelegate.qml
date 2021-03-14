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

    Label {
        id: messageLabel
        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        readonly property int fontSize: {
            var len = StringUtils.getEmoijChainLength(display)
            if (len === 0 || len > 10) {
                return Theme.fontSizeSmall
            } else if (len > 5) {
                return Theme.fontSizeLarge
            } else if (len > 3) {
                return Theme.fontSizeExtraLarge
            } else {
                return Theme.fontSizeExtraLarge * 1.5
            }
        }

        text: emojiParser.parse(display, fontSize * 1.1)
        textFormat: Text.StyledText
        font.pixelSize: fontSize
        color: Theme.primaryColor
        wrapMode: Text.Wrap
        horizontalAlignment: textAlign
        linkColor: Theme.highlightColor

        onLinkActivated: page.openLink(link)
    }

    Label {
        id: timeLabel

        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        text: (ownMessage
              ? (humanize.formatTime(time) + " • "
                 + formatEventStatus(eventStatus))
              : humanize.formatTime(time) + (edited ? " 🖊️" : ""))
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        horizontalAlignment: textAlign
    }
}
