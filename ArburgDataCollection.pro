QT += opcua

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEPENDPATH += INCLUDEPATH

SOURCES += main.cpp \
    mainwindow.cpp \    
    opcuaviewer/opcuamodel.cpp \
    opcuaviewer/treeitem.cpp \
    opcuaviewer/certificatedialog.cpp

HEADERS += \
    mainwindow.h \    
    opcuaviewer/opcuamodel.h \
    opcuaviewer/treeitem.h \
    opcuaviewer/certificatedialog.h

FORMS += \
    mainwindow.ui \
    opcuaviewer/certificatedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
