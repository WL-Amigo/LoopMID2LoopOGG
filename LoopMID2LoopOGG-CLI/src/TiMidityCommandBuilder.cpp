#include "TiMidityCommandBuilder.hpp"

#include <QSysInfo>
#include <QDebug>

TiMidityCommandBuilder::TiMidityCommandBuilder():
    m_configFilePath(""),
    m_soundfontDirPath(""),
    m_reverbType(TiMidityReverbType::FreeVerb),
    m_reverbValue(64),
    m_sampleRate(44100)
{

}

void TiMidityCommandBuilder::build(QStringList &target, QString inputFilePath, TiMidityDevice outputDevice, QString outputFilePath){
    // clear target string list
    target.clear();

    // build command lines
    target << "-c" << this->m_configFilePath;
    target << "-L" << this->m_soundfontDirPath;
    if(this->m_reverbType == TiMidityReverbType::None){
        target << "--reverb=d";
    } else if(this->m_reverbType == TiMidityReverbType::FreeVerb){
        target << "--reverb=f";
    } else if(this->m_reverbType == TiMidityReverbType::FreeVerbGlobal){
        target << QString::asprintf("--reverb=G,%d", this->m_reverbValue);
    }
    if(this->m_sampleRate != 44100){
        target << "-s" << QString::asprintf("%d", this->m_sampleRate);
    }
    if(outputDevice == TiMidityDevice::RIFFWave){
        target << "-Ow" << "-o" << outputFilePath;
    } else if(outputDevice == TiMidityDevice::WinSoundDevice){
        target << "-Od";
    } else if(outputDevice == TiMidityDevice::ALSA){
        target << "-Os";
    }
    target << "-B4,12"; // to prevent unproper playbacking
    target << inputFilePath;
    qDebug().noquote() << target;
}

TiMidityCommandBuilder* TiMidityCommandBuilder::setConfigFilePath(QString configFilePath){
    this->m_configFilePath = configFilePath;
    return this;
}

TiMidityCommandBuilder* TiMidityCommandBuilder::setSoundfontDirectoryPath(QString soundfontDirPath){
    this->m_soundfontDirPath = soundfontDirPath;
    return this;
}

TiMidityCommandBuilder* TiMidityCommandBuilder::setReverbType(TiMidityReverbType reverbType){
    this->m_reverbType = reverbType;
    return this;
}

TiMidityCommandBuilder* TiMidityCommandBuilder::setReverbValue(qint8 reverbValue){
    this->m_reverbValue = reverbValue;
    return this;
}

TiMidityCommandBuilder* TiMidityCommandBuilder::setSampleRate(qint32 sampleRate){
    this->m_sampleRate = sampleRate;
    return this;
}
