#include "Utils.h"

#ifdef Q_OS_WIN
#include "windows.h"

bool Utils::isWindows64() {
    // from https://stackoverflow.com/questions/23696749/how-to-detect-on-c-is-windows-32-or-64-bit
    BOOL f64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &f64) && f64;
}

#endif

