import QtQuick 2.0
import Sailfish.Silica 1.0
import Determinant 0.1

Page {
    id: page

    property QtObject currentRoom: null

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: currentRoom.displayName
            }
            Label {
                anchors {
                    left: content.left
                    right: content.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }

                text: currentRoom.topic
                wrapMode: Text.Wrap
                color: Theme.highlightColor
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeSmall
            }

            DetailItem {
                label: qsTr("Aliases")
                value: currentRoom.aliases.join(', ')
            }
            DetailItem {
                label: qsTr("Uses Encryption")
                value: currentRoom.usesEncryption ? "yes" : "no"
            }

            DetailItem {
                label: qsTr("Tags")
                value: currentRoom.tagNames.join(', ')
            }
            DetailItem {
                label: qsTr("Favourite")
                value: currentRoom.isFavourite ? "yes" : "no"
            }
            DetailItem {
                label: qsTr("LowPriority")
                value: currentRoom.isLowPriority ? "yes" : "no"
            }

            DetailItem {
                label: qsTr("Members")
                value: currentRoom.memberCount
            }
            DetailItem {
                label: qsTr("Joined members")
                value: currentRoom.joinedCount
            }
            DetailItem {
                label: qsTr("Invited members")
                value: currentRoom.invitedCount
            }
            DetailItem {
                label: qsTr("Total members")
                value: currentRoom.totalMemberCount
            }

            SectionHeader {
                text: qsTr("Extended")
            }

            DetailItem {
                label: qsTr("Matrix Id")
                value: currentRoom.id
            }
            DetailItem {
                label: qsTr("Matrix room version")
                value: currentRoom.version
            }
            DetailItem {
                label: qsTr("Is unstable")
                value: currentRoom.isUnstable ? "yes" : "no"
            }
            DetailItem {
                label: qsTr("Name")
                value: currentRoom.name
            }
        }

        VerticalScrollDecorator { }
    }
}
