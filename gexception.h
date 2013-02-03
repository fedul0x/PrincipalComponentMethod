#ifndef GEXCEPTION_H
#define GEXCEPTION_H
#include <QString>

class GException
{
private:
    QString message;
public:
    GException();
    GException(QString message);
    QString getMessage();
};

#endif // GEXCEPTION_H
