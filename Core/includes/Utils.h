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

#ifdef QT_DEBUG
    static const bool IsDebug = true;
#else
    static const bool IsDebug = false;
#endif

#ifdef Q_OS_WIN
    static const bool IsWindows = true;
#else
    static const bool IsWindows = false;
#endif
};

#endif  // UTILS_H
