#include "Settings.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <QDateTime>

Settings::Settings()
{

}

QString Settings::iniFilePath()
{
    QString iniFilePath = QFileInfo(QCoreApplication::applicationFilePath()).filePath();
    iniFilePath.replace(".exe", ".ini");

    return iniFilePath;
}

QString Settings::logFilePath()
{
    QString logFilePath = QFileInfo(QCoreApplication::applicationFilePath()).filePath();
    logFilePath.replace(".exe", ".log");

    return logFilePath;
}

void Settings::writeLastInsert(const QString& machineId, const QDateTime& lastInsert)
{
    QSettings settings(Settings::iniFilePath(), QSettings::IniFormat);

    settings.beginGroup(machineId);
    settings.setValue("last_insert", lastInsert.toString("yyyy.MM.dd hh:mm:ss"));
    settings.endGroup();
}

void Settings::readLastInsert(const QString& machineId, QDateTime& lastInsert)
{
    QSettings settings(Settings::iniFilePath(), QSettings::IniFormat);

    settings.beginGroup(machineId);
    QString sdt = settings.value("last_insert").toString();
    lastInsert = QDateTime::fromString(sdt, "yyyy.MM.dd hh:mm:ss");
    settings.endGroup();
}
