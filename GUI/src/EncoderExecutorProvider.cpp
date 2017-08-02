#include "EncoderExecutorProvider.hpp"

#include "MultipleEncoderExecutor.hpp"

#include <QCoreApplication>
#include <QMetaEnum>
#include <QSettings>
#include "GlobalConstants.hpp"

#include "Utils.h"

QSharedPointer<EncoderExecutor> EncoderExecutorProvider::getCurrent() {
    QSettings s;
    QMetaEnum me = QMetaEnum::fromType<ConfigEnums::Output::FileType>();
    auto outputFormat = static_cast<ConfigEnums::Output::FileType>(me.keyToValue(s.value(ConfigKey::Output::FileType).toString().toUtf8().data()));
    switch (outputFormat) {
    case ConfigEnums::Output::FileType::ogg:
        qDebug().noquote() << Q_FUNC_INFO << ": Ogg Vorbis Encoder";
        return EncoderExecutorProvider::getOggVorbis();
        break;
    case ConfigEnums::Output::FileType::aac:
        qDebug().noquote() << Q_FUNC_INFO << ": QAAC Encoder";
        return EncoderExecutorProvider::getQAAC();
        break;
    case ConfigEnums::Output::FileType::oggAndAac:
    {
        auto ovee = EncoderExecutorProvider::getOggVorbis();
        auto qaacee = EncoderExecutorProvider::getQAAC();
        auto dualEE = QSharedPointer<MultipleEncoderExecutor>(new MultipleEncoderExecutor());
        dualEE->addEncoderExecutor(ovee);
        dualEE->addEncoderExecutor(qaacee);
        return dualEE;
    }
        break;
    default:
        return QSharedPointer<EncoderExecutor>(nullptr);
        break;
    }
}

QSharedPointer<OggVorbisEncoderExecutor> EncoderExecutorProvider::getOggVorbis() {
    auto ovee = QSharedPointer<OggVorbisEncoderExecutor>(new OggVorbisEncoderExecutor());
    QSettings s;
    if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
        ovee->setBinaryPath(QCoreApplication::applicationDirPath() +
                           "/oggenc2.exe");
    } else {
        ovee->setBinaryPath("oggenc");
    }
    QMetaEnum me =
        QMetaEnum::fromType<ConfigEnums::Encoder::OVQualityModeEnum>();
    auto qualityMode = static_cast<ConfigEnums::Encoder::OVQualityModeEnum>(
        me.keyToValue(s.value(ConfigKey::Encoder::OggVorbisQualityMode)
                          .toString()
                          .toUtf8()
                          .data()));
    if(qualityMode == ConfigEnums::Encoder::OVQualityModeEnum::normal){
        ovee->setQualityValue(4.0f);
    } else if(qualityMode == ConfigEnums::Encoder::OVQualityModeEnum::priorSize){
        ovee->setQualityValue(1.0f);
    } else if(qualityMode == ConfigEnums::Encoder::OVQualityModeEnum::custom){
        qreal qualityValue = s.value(ConfigKey::Encoder::OggVorbisQualityValue).toReal();
        ovee->setQualityValue(qualityValue);
    }

    return ovee;
}

QSharedPointer<QAACEncoderExecutor> EncoderExecutorProvider::getQAAC() {
    QSharedPointer<QAACEncoderExecutor> qaacEE = QSharedPointer<QAACEncoderExecutor>(new QAACEncoderExecutor());
    QSettings s;

    // this feature is enabled only in Windows
#ifdef Q_OS_WIN
    if(Utils::isWindows64()){
        qaacEE->setBinaryPath(QCoreApplication::applicationDirPath() + "/qaac/x64/qaac64.exe");
    } else {
        qaacEE->setBinaryPath(QCoreApplication::applicationDirPath() + "/qaac/x86/qaac.exe");
    }

    // if qaac encoder feature is not available, return null pointer
    if(!qaacEE->isQAACAvailable()){
        return QSharedPointer<QAACEncoderExecutor>(nullptr);
    }

    QMetaEnum me = QMetaEnum::fromType<ConfigEnums::Encoder::QAACQualityModeEnum>();
    auto qualityMode = static_cast<ConfigEnums::Encoder::QAACQualityModeEnum>(
                me.keyToValue(s.value(ConfigKey::Encoder::QAACQualityMode).toString().toUtf8().data()));
    switch(qualityMode){
    case ConfigEnums::Encoder::QAACQualityModeEnum::normal:
        qaacEE->setQualityValue(128.0f);
        break;
    case ConfigEnums::Encoder::QAACQualityModeEnum::priorSize:
        qaacEE->setQualityValue(64.0f);
        break;
    case ConfigEnums::Encoder::QAACQualityModeEnum::custom:
    {
        qreal abr = s.value(ConfigKey::Encoder::QAACAverageBitRate).toReal();
        qaacEE->setQualityValue(abr);
    }
        break;
    }

    return qaacEE;
#else
    throw std::runtime_error("EncoderExecutorProvider::getQAAC(): not supported on this OS");
#endif
}
