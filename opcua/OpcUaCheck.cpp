#include "OpcUaCheck.h"
#include "settings/Settings.h"

#include <QDebug>
#include <QException>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>

OpcUaCheck::OpcUaCheck(const QString& machineId, QObject* parent) :
    QObject(parent),    
    m_db(),
    m_machineId(machineId),
    m_ok(0),
    m_nok(0)
{    
    Settings::readLastInsert(machineId, m_lastInsert, m_ok, m_nok);

    if(!m_lastInsert.isValid())
    {
        qWarning() << "Ini section for last insert is empty";
    }
}

void OpcUaCheck::opcuaView_dataChanged(const QStringList& var, const QStringList& data)
{
    if(var.size() != data.size())
    {
        qCritical() << "Variable and data list mismash";
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    uint32_t ok = 0;
    bool isOk = false;
    uint32_t nok = 0;
    bool isNok = false;
    double cycleTime = 0.0;
    bool isCycleTime = false;
    QString item;
    bool isItem = false;
    QString itemDesc;
    bool isItemDesc = false;
    QString info;
    bool isInfo = false;

    int index = 0;
    foreach(auto v, var)
    {
        if(v=="f077") //ok
        {
            ok = data.at(index).toUInt();
            isOk = true;
        }
        else if(v=="f087") //nok
        {
            cycleTime = data.at(index).toUInt();
            isNok = true;
        }
        else if(v=="t085") //cycle time
        {
            nok = data.at(index).toDouble();
            isCycleTime = true;
        }
        else if(v=="f052") //item
        {
            item = data.at(index);
            isItem = true;
        }
        else if(v=="f052B") //item desc
        {
            itemDesc = data.at(index);
            isItemDesc = true;
        }
        else if(v=="f9007I") //info
        {
            info = data.at(index);
            isInfo = true;
        }
        index++;
    }

    if(isOk && isNok && isCycleTime && isItem && isItemDesc && isInfo)
    {
        //qInfo().noquote() << "INSERT (" << data.join("; ") << ")";
        bool res = false;

        if(m_lastInsert.isValid())
        {
            //insert to database
            QStringList headers;
            QStringList records;

            //datetime
            headers.append("datetime");
            records.append(QString("'%1'").arg(now.toString("yyyy-MM-dd hh:mm:ss")));

            headers.append(var);
            foreach(auto d, data)
            {
                d = d.remove("\"");
                records.append(QString("'%1'").arg(d));
            }

            //status
            headers.append("status");
            records.append("1");

            res = insert(headers, records);
        }
        else
            res = true;

        if(res)
        {
            //write data
            Settings::writeLastInsert(m_machineId, now, ok, nok);
            m_lastInsert = now;
            m_ok = ok;
            m_nok = nok;
        }
    }
    else
    {
        qWarning() << "Not all params found for INSERT";
    }
}

bool OpcUaCheck::insert(const QStringList& headers, const QStringList& records)
{
    if(!m_db.isValid())
    {
        qCritical().noquote() << "No database driver!";
        return false;
    }

    QSqlQuery query(m_db);
/*
 * INSERT INTO data (datetime,f052,f052B,f9007I,t085,f077,f087,status)
 *      VALUES (2021-09-29 17:48:31,'1020245','','MATERIAL:ALCOM PA66 910/1 SLDS,084189 TOPENI FORMY:4x300`C DOBA SUSENI 4h80`C M-11160','29.540000915527344','1972','0',1)
 */
    QString squery = QString("INSERT INTO data (%1) VALUES (%2)").arg(headers.join(", ")).arg(records.join(", "));
    bool ok = query.exec(squery);
    if(!ok)
    {
        //log query
        qInfo().noquote() << squery;

        //log error
        qCritical().noquote() << "Last query error: " << query.lastError().text();

        return false;
    }

    return true;
}
