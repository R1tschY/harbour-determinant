import QtQuick 2.0
import Sailfish.Silica 1.0
import Matrix 0.5
import "pages"

ApplicationWindow
{
    property int _syncCount: 0

    initialPage: Component { LoginPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations


    function resync() {
        _syncCount += 1

        connection.sync(30 * 1000)

        if ((_syncCount % 16) == 2) {
            console.log("Saving state: " + _syncCount + " to " + connection.stateSaveFile)
            connection.saveState(connection.stateSaveFile)
        }
    }

    Connections {
        target: connection

        onLoginError: {
            if (error.indexOf("Forbidden") !== -1) {
                passwordField.text = ""
                console.info("Wrong password given")
                loading = false
                // TODO: show error
            } else {
                console.error("Same other login error")
            }
        }

        onConnected: {
            store.user = connection.localUserId
            store.accessToken = connection.accessToken
            store.homeServer = connection.homeserver

//            connection.resolveError.connect(reconnect)
//            connection.reconnected.connect(resync)

            connection.loadState(connection.stateSaveFile)
            connection.sync()
        }

        onSyncDone: {
            console.log("onSyncDone")
            resync()
        }

        onSyncError: {
            console.log("onSyncError: " + error)
        }
    }
}
