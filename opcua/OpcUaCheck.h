#ifndef OPCUA_CHECK_H
#define OPCUA_CHECK_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>

class OpcUaCheck : public QObject
{
    Q_OBJECT

public:
    OpcUaCheck(const QString& machineId, QObject* parent = nullptr);

    void setDatabase(QSqlDatabase database, const QString& table) { m_db = database; m_table = table; }

public slots:
    void opcuaView_dataChanged(const QStringList& var, const QStringList& data);

private:
    bool insert(const QStringList &headers, const QStringList &records);

private:
    QSqlDatabase    m_db;
    QString         m_table;
    QString         m_machineId;
    QDateTime       m_lastInsert;
    uint32_t        m_ok;
    uint32_t        m_nok;
};

#endif // OPCUA_CHECK_H
