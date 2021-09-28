#ifndef LOG_H
#define LOG_H

#include <QObject>

class Log : public QObject
{
    Q_OBJECT

public:
    Log();

    static Log& instance();

    void setMessage(const QString& message);

signals:
    void message(const QString& message);

};

#endif // LOG_H
