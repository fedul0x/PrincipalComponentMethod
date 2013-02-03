#include "gexception.h"
#include <QString>

GException::GException()
{
    this->message = "CommonException";
}

GException::GException(QString message)
{
    this->message = message;
}

QString GException::getMessage()
{
    return this->message;
}
