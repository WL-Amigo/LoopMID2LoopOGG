#include "EncoderExecutorProvider.hpp"

#include <QCoreApplication>
#include <QMetaEnum>
#include <QSettings>
#include "GlobalConstants.hpp"

OggVorbisEncoderExecutor EncoderExecutorProvider::getOggVorbis() {
    OggVorbisEncoderExecutor ovee;
    QSettings s;
    if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
        ovee.setBinaryPath(QCoreApplication::applicationDirPath() +
                           "/oggenc2.exe");
    } else {
        ovee.setBinaryPath("oggenc");
    }
    QMetaEnum me =
        QMetaEnum::fromType<ConfigEnums::Encoder::OVQualityModeEnum>();
    auto qualityMode = static_cast<ConfigEnums::Encoder::OVQualityModeEnum>(
        me.keyToValue(s.value(ConfigKey::Encoder::OggVorbisQualityMode)
                          .toString()
                          .toUtf8()
                          .data()));
    if(qualityMode == ConfigEnums::Encoder::OVQualityModeEnum::normal){
        ovee.setQualityValue(4.0f);
    } else if(qualityMode == ConfigEnums::Encoder::OVQualityModeEnum::priorSize){
        ovee.setQualityValue(1.0f);
    } else if(qualityMode == ConfigEnums::Encoder::OVQualityModeEnum::custom){
        qreal qualityValue = s.value(ConfigKey::Encoder::OggVorbisQualityValue).toReal();
        ovee.setQualityValue(qualityValue);
    }

    return ovee;
}
