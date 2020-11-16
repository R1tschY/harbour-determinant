import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    property alias header: header.title
    property alias text: label.text

    SilicaFlickable {
        id: flickable

        anchors.fill: parent

        Column {
            id: column

            width: page.width

            PageHeader {
                id: header
                title: qsTr("Error")
            }

            Label {
                id: label
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                }

                color: Theme.secondaryHighlightColor
                wrapMode: Text.WordWrap
            }
        }

        VerticalScrollDecorator { flickable: flickable }
    }
}
