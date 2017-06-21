#include "includes/TiMidityCommandBuilderProvider.h"
#include <QMetaEnum>
#include <QCoreApplication>
#include <QDir>

TiMidityCommandBuilderProvider::TiMidityCommandBuilderProvider()
{

}

TiMidityCommandBuilder TiMidityCommandBuilderProvider::getDefault(){
    TiMidityCommandBuilder tcb;
    QSettings s;
    QMetaEnum me = QMetaEnum::fromType<ConfigEnums::Effect::ReverbModeEnum>();
    auto reverbMode = static_cast<ConfigEnums::Effect::ReverbModeEnum>(me.keyToValue(s.value(ConfigKey::Effect::ReverbMode).toString().toUtf8().data()));
    TiMidityReverbType tmReverbType;
    if(reverbMode == ConfigEnums::Effect::ReverbModeEnum::disenable){
        tmReverbType = TiMidityReverbType::None;
    } else if(reverbMode == ConfigEnums::Effect::ReverbModeEnum::enable){
        tmReverbType = TiMidityReverbType::FreeVerb;
    } else if(reverbMode == ConfigEnums::Effect::ReverbModeEnum::enableGlobal){
        tmReverbType = TiMidityReverbType::FreeVerbGlobal;
    }

    tcb.setConfigFilePath(QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + s.value(ConfigKey::TiMidity::ConfigFilePath).toString()))
            ->setSoundfontDirectoryPath(QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + s.value(ConfigKey::TiMidity::SoundfontDirPath).toString()))
            ->setReverbType(tmReverbType)
            ->setReverbValue(s.value(ConfigKey::Effect::ReverbLevel).toInt());
    return tcb;
}
