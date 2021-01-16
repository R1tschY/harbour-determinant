import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Configuration 1.0
import Determinant 0.1
import "pages"

ApplicationWindow
{
    id: app

    property bool timeFormat24: timeFormatSetting.value === "24"
    property string customMessageCss: getCustomMessageCss()

    initialPage: Component { StartupPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    ConfigurationValue {
        id: timeFormatSetting
        defaultValue: "12"
        key: "/sailfish/i18n/lc_timeformat24h"
    }

    Humanize {
        id: humanize
        timeFormat: timeFormat24
            ? Humanize.TimeFormat24Hours : Humanize.TimeFormat12Hours
    }

    Connections {
        target: connection
        onLoggedOut: pageStack.replace(Qt.resolvedUrl("pages/LoginPage.qml"))
    }

    Connections {
        target: applicationService
        //onRaiseInternal: app.raise()
        onActivateRoomInternal: {
            console.log("ActivateRoom: " + roomId  );
            openRoom(roomId)
        }
    }

    function getCustomMessageCss() {
        return "<style>
            a { color: " + Theme.highlightColor + "; }
            code { font-size: " + Theme.fontSizeExtraSmall + "px; }
        </style>"
    }

    function openRoom(roomId) {
        if (typeof pageStack === "undefined")
            return; // happens when using QmlLive

        console.log("opening room " + roomId)

        app.activate()
        pageStack.completeAnimation()

        var roomPage = pageStack.find(function(page) {
            return page.objectName === "RoomPage" && page.currentRoom.id === roomId
        })
        if (roomPage !== null) {
            return
        }

        var room = connection.room(roomId);
        if (room === null) {
            console.error("Room with id '" + roomId + "' not found")
            return;
        }

        var roomListPage = pageStack.find(function(page) {
            return page.objectName === "RoomListPage"
        })
        if (roomListPage !== null) {
            pageStack.pop(roomListPage, PageStackAction.Immediate);
            pageStack.push(
                Qt.resolvedUrl("pages/RoomPage.qml"), { currentRoom: room },
                PageStackAction.Immediate)
        } else {
            console.error("No room list found")
        }
    }
}
