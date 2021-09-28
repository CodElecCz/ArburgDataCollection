QT += opcua

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEPENDPATH += INCLUDEPATH

SOURCES += main.cpp \
    mainwindow.cpp \     \
    opcua/opcuaclient.cpp \
    opcua/viewer/certificatedialog.cpp \
    opcua/viewer/opcuamodel.cpp \
    opcua/viewer/treeitem.cpp

HEADERS += \
    mainwindow.h \     \
    opcua/opcuaclient.h \
    opcua/viewer/certificatedialog.h \
    opcua/viewer/opcuamodel.h \
    opcua/viewer/treeitem.h

FORMS += \
    mainwindow.ui \
    opcua/opcuaclient.ui \
    opcua/viewer/certificatedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    opcua/viewer/opcuaviewer.png
