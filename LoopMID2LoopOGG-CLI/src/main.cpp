#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include "tclap/CmdLine.h"
#include "MIDIInfoCollector.hpp"
#include "LoopMIDIModifier.hpp"
#include "RIFFWaveEditor.h"
#include "LoopOGGGenerator.hpp"

int main(int argc, char *argv[])
{
    try {
        // initialize TCLAP Interface
        TCLAP::CmdLine cmd("LoopMID to LoopOGG Converter - Command Line Interface", ' ', "0.1");

        // define command line arguments
        TCLAP::ValueArg<std::string> inputFileNameArg("i", "input", "Standard MIDI File (for RPG Maker 2000) to convert", true, "", "string");
        cmd.add(inputFileNameArg);

        // parse and get arguments
        cmd.parse( argc, argv );
        QString inputFileName = QString::fromStdString(inputFileNameArg.getValue());
        std::string inputFileNameSS = inputFileName.toStdString();

        // run convert process
//        LoopOGGGenerator::convert(inputFileName,
//        "/home/wlamigo-data/ubin/timidity++-bin/bin/timidity",
//        "/home/wlamigo-data/Music/sf2/SGM_v2.01.cfg",
//        "oggenc");

        // testing
        MIDIInfoCollector mic(inputFileNameSS);
        mic.test();

    } catch (TCLAP::ArgException &e){
        qDebug() << "error: " << e.error().c_str() << " for arg " << e.argId().c_str();
    }

    return 0;
}
