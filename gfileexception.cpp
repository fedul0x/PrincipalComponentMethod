#include "gfileexception.h"

#include <QString>

GFileException::GFileException()
{
    this->message = "CommonException";
}

GFileException::GFileException(QString message)
{
    this->message = message;
}

QString GFileException::getMessage()
{
    return this->message;
}
