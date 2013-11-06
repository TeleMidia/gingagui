#ifndef QNPLUTIL_H
#define QNPLUTIL_H

#include <QStringList>
#include <QString>

class QnplUtil
{   
public:
    static QString VERSION;
    static QString CMD_PREFIX;
    static QString QUIT;
    static QString GINGA_KEY_PREFIX;
    static QString GINGA_CLICK_PREFIX;
    static QString GINGA_COMMAND_PREFIX;

    static QStringList split(QString parameters);
};

#endif // QNPLUTIL_H