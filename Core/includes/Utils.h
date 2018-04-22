#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QtCore>

class Utils {
public:
#ifdef Q_OS_WIN
    static bool isWindows64();
#endif
    static QString getTemporaryDirName();
};

#endif  // UTILS_H
