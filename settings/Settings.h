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
    static inline char const       cVersion[] = "v1.0.2";

    static QString iniFilePath();
    static QString logFilePath();

    static void writeLastInsert(const QString& machineId, const QDateTime& lastInsert, uint32_t& ok, uint32_t &nok);
    static void readLastInsert(const QString& machineId, QDateTime& lastInsert, uint32_t& ok, uint32_t &nok);
};

#endif // SETTINGS_H
