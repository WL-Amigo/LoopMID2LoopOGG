#ifndef GLOBALCONSTANTS_HPP
#define GLOBALCONSTANTS_HPP

#include <QString>
#include <QtCore>
typedef const QString CONFIGKEY;

namespace GlobalConstants {

const QString CURRENT_VERSION = "1.1.0"; // current software version

const quint32 DEFAULT_SAMPLERATE = 44100; // default samplerate

}

// --- configuration key strings ---

namespace ConfigKey {

Q_NAMESPACE

// --- main window
CONFIGKEY OutputDirectory = "outputDirectory";
CONFIGKEY Preview = "preview";

// --- output
namespace Output {
CONFIGKEY FileType = "output/fileType";
CONFIGKEY Mode = "output/mode";
CONFIGKEY MaxSamplesAfterLoopEnd = "output/maxSamplesAfterLoopEnd";
CONFIGKEY FadeoutStartSec = "output/fadeoutStartSec";
CONFIGKEY FadeoutLengthSec = "output/fadeoutLengthSec";
CONFIGKEY LoopNumber = "output/loopNumber";
}

// --- TiMidity
namespace TiMidity {
CONFIGKEY ConfigFilePath = "timidity/configFilePath"; // relative
CONFIGKEY SoundfontDirPath = "timidity/soundfontDirectoryPath"; // relative
}

// --- Effect
namespace Effect {
CONFIGKEY ReverbMode = "effect/reverbMode";
CONFIGKEY ReverbLevel = "effect/reverbLevel";
}

// -- Encoder
namespace Encoder {
CONFIGKEY OggVorbisQualityMode = "encoder/oggvorbis/qualityMode";
CONFIGKEY OggVorbisQualityValue = "encoder/oggvorbis/qualityValue";
}
}

// --- configuration enums with meta information
namespace ConfigEnums {

namespace Output {
Q_NAMESPACE
enum class FileType{
    ogg,
    wav
};
Q_ENUM_NS(FileType)
enum class Mode{
    optimized,
    soundtrack
};
Q_ENUM_NS(Mode)
}

namespace Effect{
Q_NAMESPACE
enum class ReverbModeEnum {
  disenable,
  enable,
  enableGlobal,
};
Q_ENUM_NS(ReverbModeEnum)
}

namespace Encoder {
Q_NAMESPACE
enum class OVQualityModeEnum {
  normal,
  priorSize,
  custom,
};
Q_ENUM_NS(OVQualityModeEnum)
}

}

#endif // GLOBALCONSTANTS_HPP
