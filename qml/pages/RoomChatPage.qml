import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1

Page {
    id: page

    property QtObject currentRoom

    allowedOrientations: Orientation.All

    SilicaListView {
        anchors.fill: parent

        verticalLayoutDirection: ListView.BottomToTop

        model: RoomEventsModel {
            room: currentRoom
        }

        delegate: ListItem {
            Column {
                Label {
                    text: display
                }
                Label {
                    text: author ? author.displayName : ""
                }
            }
        }
    }
}
