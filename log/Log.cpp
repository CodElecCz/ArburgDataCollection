#include "Log.h"

Log::Log()
{

}

Log& Log::instance()
{
    static Log instance;
    return instance;
}

void Log::setMessage(const QString& msg)
{
    emit message(msg);
}
