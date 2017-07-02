#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

class Utils
{
public:
#ifdef Q_OS_WIN
    static bool isWindows64();
#endif
};

#endif // UTILS_H
