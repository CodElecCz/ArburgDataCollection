QT += core gui sql opcua

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    CentralView.cpp \
    browser/BrowserView.cpp \
    browser/ConnectionWidget.cpp \
    browser/QsqlConnectionDialog.cpp \
    log/Log.cpp \
    log/LogView.cpp \
    MainWindow.cpp \
    opcua/OpcUaCheck.cpp \
    opcua/table/OpcUaTableItem.cpp \
    opcua/table/OpcUaTableModel.cpp \
    opcua/tree/OpcUaItemModelSort.cpp \
    opcua/OpcUaView.cpp \
    opcua/certificate/certificatedialog.cpp \
    opcua/tree/OpcUaItemModel.cpp \
    opcua/tree/OpcUaTreeItem.cpp \
    settings/Settings.cpp \
    settings/SettingsView.cpp \
    support/MainWindowExt.cpp \
    support/controls/StatusIndicator.cpp \
    support/controls/WaitingSpinner.cpp \
    support/dialog/WaitDialog.cpp \
    support/style/CustomStyle.cpp

HEADERS += \
    CentralView.h \
    browser/BrowserView.h \
    browser/ConnectionWidget.h \
    browser/QsqlConnectionDialog.h \
    log/Log.h \
    log/LogView.h \
    MainWindow.h \
    opcua/OpcUaCheck.h \
    opcua/table/OpcUaTableItem.h \
    opcua/table/OpcUaTableModel.h \
    opcua/tree/OpcUaItemModelSort.h \
    opcua/OpcUaView.h \
    opcua/certificate/certificatedialog.h \
    opcua/tree/OpcUaItemModel.h \
    opcua/tree/OpcUaTreeItem.h \
    settings/Settings.h \
    settings/SettingsView.h \
    support/MainWindowExt.h \
    support/controls/StatusIndicator.h \
    support/controls/WaitingSpinner.h \
    support/dialog/WaitDialog.h \
    support/style/CustomStyle.h

FORMS += \
    CentralView.ui \
    browser/BrowserView.ui \
    browser/QsqlConnectionDialog.ui \
    log/LogView.ui \
    MainWindow.ui \
    opcua/OpcUaView.ui \
    opcua/certificate/certificatedialog.ui \
    settings/SettingsView.ui \
    support/about/AboutDialog.ui

RESOURCES += \
    resources/style/DarkStyle.qrc \
    resources/style/LightStyle.qrc \
    resources/resource.qrc

RC_FILE += \
    resources/resource.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
