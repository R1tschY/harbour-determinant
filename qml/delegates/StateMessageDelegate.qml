import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.2

Column {
    id: contentColumn
    width: page.width

    Label {
        width: page.width - 2 * Theme.horizontalPageMargin
        x: Theme.horizontalPageMargin
        text: EmojiParser.parseText(display, Theme.fontSizeExtraSmall)
        textFormat: Text.StyledText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }
}
