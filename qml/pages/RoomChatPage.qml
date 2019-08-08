import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1

Page {
    id: page

    property QtObject currentRoom

    allowedOrientations: Orientation.All

    SilicaListView {
        id: eventListView
        anchors.fill: parent

        verticalLayoutDirection: ListView.BottomToTop

        header: PageHeader {
           description: currentRoom.id + " / " + currentRoom.name + " / " + eventListView.count
        }

        model: RoomEventsModel {
            room: currentRoom
        }

        delegate: ListItem {
            // LayoutMirroring.enabled: author && author.id === connection.localUserId
            // LayoutMirroring.childrenInherit: true

            property int textAlign: (author && author.id === connection.localUserId)
                                     ? Text.AlignRight
                                     : Text.AlignLeft

            Column {
                width: parent.width
                Label {
                    width: parent.width - 2* Theme.horizontalPageMargin
                    x: Theme.horizontalPageMargin
                    text: display
                    textFormat: Text.StyledText
                    color: Theme.primaryColor
                    horizontalAlignment: textAlign;
                }
                Label {
                    width: parent.width - 2* Theme.horizontalPageMargin
                    x: Theme.horizontalPageMargin
                    text: author ? author.displayName : ""
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    horizontalAlignment: textAlign;
                }
            }
        }
    }
}
