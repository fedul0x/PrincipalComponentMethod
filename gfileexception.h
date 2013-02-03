#ifndef GFILEEXCEPTION_H
#define GFILEEXCEPTION_H

#include <QString>

class GFileException
{
private:
    QString message;
public:
    GFileException();
    GFileException(QString message);
    QString getMessage();
};

#endif // GFILEEXCEPTION_H
