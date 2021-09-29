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

void Settings::writeLastInsert(const QString& machineId, const QDateTime& lastInsert, uint32_t& ok, uint32_t &nok)
{
    QSettings settings(Settings::iniFilePath(), QSettings::IniFormat);

    settings.beginGroup(machineId);
    settings.setValue("last_insert", lastInsert.toString("yyyy.MM.dd hh:mm:ss"));
    settings.setValue("ok", ok);
    settings.setValue("nok", nok);
    settings.endGroup();
}

void Settings::readLastInsert(const QString& machineId, QDateTime& lastInsert, uint32_t& ok, uint32_t &nok)
{
    QSettings settings(Settings::iniFilePath(), QSettings::IniFormat);

    settings.beginGroup(machineId);
    QString sdt = settings.value("last_insert").toString();
    lastInsert = QDateTime::fromString(sdt, "yyyy.MM.dd hh:mm:ss");

    ok = settings.value("ok", 0).toUInt();
    nok = settings.value("nok", 0).toUInt();

    settings.endGroup();
}
