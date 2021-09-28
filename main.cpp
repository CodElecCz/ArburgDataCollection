/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "MainWindow.h"
#include "settings/Settings.h"
#include "log/Log.h"

#include <QtCore>
#include <QtWidgets>
#include <QCoreApplication>
#include <QDebug>

// Get the default Qt message handler.
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Handle the messages!
    QString dt = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    QString stype;
    switch (type)
    {
    case QtMsgType::QtInfoMsg:
        stype = QString("Info");
        break;
    case QtMsgType::QtDebugMsg:
        stype = QString("Debug");
        break;
    case QtMsgType::QtWarningMsg:
        stype = QString("Warning");
        break;
    case QtMsgType::QtCriticalMsg:
        stype = QString("Critical");
        break;
    case QtMsgType::QtFatalMsg:
        stype = QString("Fatal");
        break;
    }

    //to file
    QFile file(Settings::logFilePath());
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&file);      

    QString logMsg = QString("%1 [%2] %3").arg(dt).arg(stype).arg(msg);
    ts << logMsg << endl;
    file.close();

    //to application
    Log::instance().message(logMsg);

    //check file size
    QFileInfo fileInfo(Settings::logFilePath());
    if(fileInfo.size()>1024*1024)
    {
        QStringList pathList = Settings::logFilePath().split(".");

        QString suffix = pathList.last();
        pathList.removeLast();

        QFileInfo newFileInfo;

        int index = 0;
        do
        {
            newFileInfo.setFile(pathList.join(".") + "_" + QString::number(index,10) + "." + suffix);
            index++;
        }while(newFileInfo.exists());

        file.rename(newFileInfo.absoluteFilePath());
    }

    // Call the default handler
    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}


int main(int argc, char *argv[])
{   
    int ret = 0;

    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QApplication app(argc, argv);

    qInstallMessageHandler(customMessageHandler);

    QCoreApplication::setOrganizationName(Settings::cOrganizationName);
    QCoreApplication::setApplicationName(Settings::cApplicationName);
    QCoreApplication::setApplicationVersion(Settings::cVersion);       

    qInfo() << "Application start, ver: " << Settings::cVersion;

    try
    {
        MainWindow mainWindow;
        mainWindow.setWindowTitle(QString("%1 (%2)").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()));
        mainWindow.show();

        app.exec();
    }
    catch (QException& e)
    {
        qCritical() << "Exception: " << e.what();

        return EXIT_FAILURE;
    }
    catch(...)
    {
        qCritical() << "Unknown exception";

        return EXIT_FAILURE;
    }

    qInfo() << "Application close, return code: " << ret;

    return ret;
}
