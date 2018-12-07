#-------------------------------------------------
#
# Project created by QtCreator 2018-09-30T17:53:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TESt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    sources/main.cpp \
    sources/Game.cpp \
    sources/Window.cpp \
    sources/FightWindow.cpp \
    sources/BattleSim.cpp \
    sources/MenuWindow.cpp \
    sources/ItemImage.cpp

HEADERS += \
    headers/Game.h\
    headers/Window.h \
    headers/FightWindow.h \
    headers/BattleSim.h \
    headers/MenuWindow.h \
    headers/ItemImage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

FORMS += \
    mainwindow.ui \
    fightwindow.ui \
    menuwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GameLibrary/release/ -lGameLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GameLibrary/debug/ -lGameLibrary
else:unix: LIBS += -L$$OUT_PWD/../GameLibrary/ -lGameLibrary

INCLUDEPATH += $$PWD/../GameLibrary
DEPENDPATH += $$PWD/../GameLibrary

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GameLibrary/release/GameLibrary.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GameLibrary/debug/GameLibrary.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GameLibrary/libGameLibrary.a
