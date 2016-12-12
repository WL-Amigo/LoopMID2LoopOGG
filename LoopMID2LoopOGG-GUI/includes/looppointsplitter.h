#ifndef LOOPPOINTSPLITTER_H
#define LOOPPOINTSPLITTER_H

#include <QVector>
#include <QSharedPointer>

class LoopPointSplitter
{
public:
    // default constructor
    LoopPointSplitter(quint32 sampleRate);

    // public executor
    bool exec();

private:
    // process methods
    quint32 getLoopEndSample();
    quint32 getLoopStartSample();
    void collectSettingsAt(QVector<QSharedPointer<MIDISettings>>& settingsDest);


private:
};

struct MIDISettings {
    quint8 bank;
    quint8 program;
};

#endif // LOOPPOINTSPLITTER_H
