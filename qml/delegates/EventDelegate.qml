import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    id: listItem
    // LayoutMirroring.childrenInherit: true
    // LayoutMirroring.enabled: author && author.id === connection.localUserId

    property int ownMessage:
        author && author.id === connection.localUserId

    property bool sectionBoundary: (
        ListView.nextSection !== "" && ListView.nextSection !== ListView.section)
        || model.index === eventsView.count - 1

    contentHeight:
        contentLoader.y + contentLoader.height + Theme.paddingSmall * 2 +
        (readMarker ? Theme.fontSizeSmall : 0)

    property var modelSection: listItem.ListView.section
    property bool readMarker:
        eventsView.showUnreadMarkerId === eventId

    Loader {
        id: sectionHeaderLoader

        property var section: listItem.ListView.section

        sourceComponent: sectionBoundary ? sectionHeader : undefined
        width: parent.width
    }

    Loader {
        id: contentLoader
        source: contentItemSource(eventType)
        y: sectionBoundary
           ? sectionHeaderLoader.y + sectionHeaderLoader.height + Theme.paddingSmall
           : Theme.paddingSmall
        width: parent.width
    }

    // Unread marker
    Label {
        id: unreadMarkerLabel
        visible: readMarker
        anchors {
            top: contentLoader.bottom
            horizontalCenter: parent.horizontalCenter
        }

        text: qsTr("New messages")
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall

        Rectangle {
            y: Theme.fontSizeSmall
            x: -unreadMarkerLabel.x + Theme.paddingMedium
            height: 1
            width: page.width - 2 * Theme.paddingMedium
            color: Theme.highlightBackgroundColor
        }
    }
}
