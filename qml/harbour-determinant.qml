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

    function getCustomMessageCss() {
        return "<style>
            a { color: " + Theme.highlightColor + "; }
            code { font-size: " + Theme.fontSizeExtraSmall + "px; }
        </style>"
    }
}
