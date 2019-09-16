import QtQuick 2.0
import Sailfish.Silica 1.0

Label {
    width: page.width - 2 * Theme.horizontalPageMargin
    x: Theme.horizontalPageMargin
    text: display
    textFormat: Text.StyledText
    font.pixelSize: Theme.fontSizeSmall
    color: Theme.primaryColor
    wrapMode: Text.Wrap
    horizontalAlignment: textAlign
}
