#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>

enum MessageType
{
    Info = 0,
    Warning,
    Error,
    Connect,
    Disconnect,
    Enable,
    Disable
};

class Settings
{
public:
    Settings();

    static inline char const       cOrganizationName[] = "CodElec";
    static inline char const       cApplicationName[] = "ARBURG DataCollection";
    static inline char const       cVersion[] = "v1.0.0";

    static QString iniFilePath();
    static QString logFilePath();

    static void writeLastInsert(const QString& machineId, const QDateTime& lastInsert);
    static void readLastInsert(const QString& machineId, QDateTime& lastInsert);
};

#endif // SETTINGS_H
