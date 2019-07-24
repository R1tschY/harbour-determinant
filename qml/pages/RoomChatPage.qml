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
            Column {
                Label {
                    text: display
                    textFormat: Text.StyledText
                    color: Theme.primaryColor
                }
                /*Label {
                    text: author ? author.displayName : ""
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                }*/
            }
        }
    }
}
