# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-determinant

CONFIG += sailfishapp
CONFIG += link_pkgconfig

PKGCONFIG += sailfishsecrets sailfishapp

include(libquotient/libqmatrixclient.pri)

SOURCES += src/harbour-determinant.cpp \
    src/connectionsmanager.cpp \
    src/humanize.cpp \
    src/models/messagerenderer.cpp \
    src/models/roomlistmodel.cpp \
    src/models/roommembersmodel.cpp \
    src/quotientintegration.cpp \
    src/secretsservice.cpp \
    src/store.cpp \
    src/models/roomeventsmodel.cpp \
    src/thumbnailprovider.cpp

DISTFILES += qml/harbour-determinant.qml \
    TODO.txt \
    qml/components/Avatar.qml \
    qml/components/EventsView.qml \
    qml/components/MessageInput.qml \
    qml/cover/CoverPage.qml \
    qml/delegates/EventDelegate.qml \
    qml/delegates/MessageDelegate.qml \
    qml/delegates/StateMessageDelegate.qml \
    qml/pages/LoginPage.qml \
    qml/pages/RoomDetailsPage.qml \
    qml/pages/RoomPage.qml \
    qml/pages/StartupPage.qml \
    rpm/harbour-determinant.changes.in \
    rpm/harbour-determinant.changes.run.in \
    rpm/harbour-determinant.spec \
    rpm/harbour-determinant.yaml \
    translations/*.ts \
    harbour-determinant.desktop \
    qml/pages/RoomListPage.qml

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_DISABLE_DEPRECATED_BEFORE=0x050600
# DEFINES +=QT_USE_QSTRINGBUILDER # Problems with libquotient
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_DEBUG

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-determinant-de.ts

HEADERS += \
    src/connectionsmanager.h \
    src/humanize.h \
    src/models/messagerenderer.h \
    src/models/roomlistmodel.h \
    src/models/roommembersmodel.h \
    src/quotientintegration.h \
    src/secretsservice.h \
    src/store.h \
    src/models/roomeventsmodel.h \
    src/thumbnailprovider.h
