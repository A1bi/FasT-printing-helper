#-------------------------------------------------
#
# Project created by QtCreator 2014-03-30T17:30:41
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FasT-printing-helper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    printeventfilter.cpp

HEADERS  += mainwindow.h \
    printeventfilter.h

FORMS    += mainwindow.ui

macx {
    QMAKE_INFO_PLIST = Info.plist

    OTHER_FILES += \
        Info.plist \
        de.lproj/InfoPlist.strings

    MAC_RESOURCES.files = de.lproj
    MAC_RESOURCES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += MAC_RESOURCES

    CONVERT_STRINGS.commands = "plutil -convert binary1 $${TARGET}.app/Contents/Resources/de.lproj/InfoPlist.strings"
    QMAKE_EXTRA_TARGETS += CONVERT_STRINGS
    POST_TARGETDEPS += CONVERT_STRINGS
}
