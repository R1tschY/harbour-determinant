import QtQuick 2.0
import Sailfish.Silica 1.0


Column {
    id: contentColumn
    width: parent.width

    Label {
        width: page.width - 2 * Theme.horizontalPageMargin
        x: Theme.horizontalPageMargin
        text: display
        textFormat: Text.PlainText
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }
}
