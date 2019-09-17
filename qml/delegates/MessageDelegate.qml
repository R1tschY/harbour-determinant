import QtQuick 2.0
import Sailfish.Silica 1.0


Column {
    id: contentColumn
    width: parent.width

    property int textAlign: ownMessage ? Text.AlignRight : Text.AlignLeft

    Label {
        width: page.width - 2 * Theme.horizontalPageMargin
        x: Theme.horizontalPageMargin
        text: display
        textFormat: Text.StyledText
        color: Theme.primaryColor
        wrapMode: Text.Wrap
        horizontalAlignment: textAlign
        linkColor: Theme.secondaryColor
    }

    Label {
        width: page.width - 2 * Theme.horizontalPageMargin
        x: Theme.horizontalPageMargin
        text: author ? author.displayName : ""
        textFormat: Text.PlainText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        horizontalAlignment: textAlign
    }

    Label {
        width: page.width - 2 * Theme.horizontalPageMargin
        x: Theme.horizontalPageMargin
        text: eventType + " " + matrixType
        textFormat: Text.PlainText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        horizontalAlignment: textAlign
    }
}
