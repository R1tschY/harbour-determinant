import QtQuick 2.0
import Sailfish.Silica 1.0


Item {
    id: contentColumn
    width: page.width * 0.8 - 2 * Theme.horizontalPageMargin
    x: ownMessage
        ? page.width * 0.2 + Theme.horizontalPageMargin
        : Theme.horizontalPageMargin
    height: ownMessage
        ? messageLabel.height
        : messageLabel.height + authorLabel.height

    property int textAlign: ownMessage ? Text.AlignRight : Text.AlignLeft

    Rectangle {
        anchors.fill: contentColumn
        color: Theme.primaryColor
        opacity: 0.1
        radius: Theme.paddingSmall
    }

    Label {
        id: messageLabel
        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall

        text: display
        textFormat: Text.StyledText
        color: Theme.primaryColor
        wrapMode: Text.Wrap
        horizontalAlignment: textAlign
        linkColor: Theme.secondaryColor
    }

    Label {
        id: authorLabel
        width: parent.width - 2 * Theme.paddingSmall
        x: Theme.paddingSmall
        anchors.top: messageLabel.bottom

        text: authorDisplayName
        textFormat: Text.PlainText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideMiddle
        visible: !ownMessage
    }

//    Label {
//        width: page.width - 2 * Theme.horizontalPageMargin
//        x: Theme.horizontalPageMargin
//        text: eventType + " " + matrixType
//        textFormat: Text.PlainText
//        font.pixelSize: Theme.fontSizeExtraSmall
//        color: Theme.secondaryColor
//        horizontalAlignment: textAlign
//    }
}
