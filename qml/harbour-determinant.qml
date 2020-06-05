import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Configuration 1.0
import Determinant 0.1
import "pages"

ApplicationWindow
{
    id: app

    property bool timeFormat24: timeFormatSetting.value === "24"

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

    initialPage: Component { LoginPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
