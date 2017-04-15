#ifndef TIMIDITYCOMMANDBUILDERPROVIDER_H
#define TIMIDITYCOMMANDBUILDERPROVIDER_H

#include <QSettings>
#include "GlobalConstants.hpp"
#include "TiMidityCommandBuilder.hpp"

class TiMidityCommandBuilderProvider
{
public:
    TiMidityCommandBuilderProvider();
    static TiMidityCommandBuilder getDefault();
};

#endif // TIMIDITYCOMMANDBUILDERPROVIDER_H
