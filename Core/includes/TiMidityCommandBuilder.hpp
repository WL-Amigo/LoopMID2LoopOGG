#ifndef TIMIDITYCOMMANDBUILDER_HPP
#define TIMIDITYCOMMANDBUILDER_HPP

#include <QStringList>

enum class TiMidityDevice{
    RIFFWave,
    WinSoundDevice,
    ALSA
};

enum class TiMidityReverbType{
    None,
    FreeVerb,
    FreeVerbGlobal
};

class TiMidityCommandBuilder
{
protected:
    QString m_configFilePath;
    QString m_soundfontDirPath;
    TiMidityReverbType m_reverbType;
    qint8 m_reverbValue;
    qint32 m_sampleRate;

public:
    TiMidityCommandBuilder();
    void build(QStringList &target, QString inputFilePath, TiMidityDevice outputDevice, QString outputFilePath = "");

    TiMidityCommandBuilder* setConfigFilePath(QString configFilePath);
    TiMidityCommandBuilder* setSoundfontDirectoryPath(QString soundfontDirPath);
    TiMidityCommandBuilder* setReverbType(TiMidityReverbType reverbType);
    TiMidityCommandBuilder* setReverbValue(qint8 reverbValue);
    TiMidityCommandBuilder* setSampleRate(qint32 sampleRate);
};

#endif // TIMIDITYCOMMANDBUILDER_HPP
