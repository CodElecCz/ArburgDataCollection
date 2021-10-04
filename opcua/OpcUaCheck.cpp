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
    m_table(),
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
            nok = data.at(index).toUInt();
            isNok = true;
        }
        else if(v=="t085") //cycle time
        {
            cycleTime = data.at(index).toDouble();
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

        if(m_lastInsert.isValid()) //insert to database
        {
            QStringList headers;
            QStringList records;

            //datetime
            headers.append("\"datetime\"");
            records.append(QString("'%1'").arg(now.toString("yyyy-MM-dd hh:mm:ss.zzz")));

            foreach(auto v, var)
            {
                headers.append(QString("\"%1\"").arg(v));
            }

            foreach(auto d, data)
            {
                d = d.remove("\"");
                records.append(QString("'%1'").arg(d));
            }

            //ok parts
            int okResult = ok - m_ok;
            int nokResult = nok - m_nok;

            if(okResult<0 || nokResult<0)
            {
                qInfo().noquote() << QString("OK:NOK reseted: %1:%2 to %3:%4").arg(m_ok).arg(m_nok).arg(ok).arg(nok);

                okResult = ok;
                nokResult = nok;

                //store new values
                m_ok = ok;
                m_nok = nok;
            }                        

            if(okResult>0)
            {
                QStringList headersOk = headers;
                QStringList recordsOk = records;

                //status
                headersOk.append("\"status\"");
                recordsOk.append("1");

                //pieces
                headersOk.append("\"pieces\"");
                recordsOk.append(QString::number(okResult));

                res = insert(headersOk, recordsOk);
            }           

            if(nokResult>0)
            {
                QStringList headersNok = headers;
                QStringList recordsNok = records;

                //status
                headersNok.append("\"status\"");
                recordsNok.append("0");

                //pieces
                headersNok.append("\"pieces\"");
                recordsNok.append(QString::number(nokResult));

                res = insert(headersNok, recordsNok);
            }           
        }
        else //no insert in ini file
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
        qCritical() << "Not all params found for INSERT";
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

    if(m_table.isEmpty())
    {
        qCritical().noquote() << "No target database table!";
        return false;
    }
/*
 * INSERT INTO "660" ("datetime", "f052", "f052B", "f9007I", "t085", "f077", "f087", "status")
 * VALUES ('2021-09-30 09:06:20', '1020245', '', 'MATERIAL:ALCOM PA66 910/1 SLDS,084189 TOPENI FORMY:4x300`C DOBA SUSENI 4h80`C M-11160', '29.549999237060547', '1644', '0', 1)
 */
    QString squery = QString("INSERT INTO \"%1\" (%2) VALUES (%3)").arg(m_table).arg(headers.join(", ")).arg(records.join(", "));
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
