#include "includes/conversionconfirmationdialog.h"
#include "ui_conversionconfirmationdialog.h"

#include "includes/ConvertingDialog.hpp"

#include <QPushButton>
#include <QCoreApplication>
#include <QSysInfo>

ConversionConfirmationDialog::ConversionConfirmationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConversionConfirmationDialog)
{
    ui->setupUi(this);
}

ConversionConfirmationDialog::ConversionConfirmationDialog(QWidget *parent, QString smFile) :
    QDialog(parent),
    ui(new Ui::ConversionConfirmationDialog),
    timidityProc(),
    smFile(smFile)
{
    // setup UI from form data
    ui->setupUi(this);

    // connect signal/slots
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &ConversionConfirmationDialog::spawnConvertingDialog);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &ConversionConfirmationDialog::beforeExit);

    // spawn TiMidity++ as player
    spawnTiMidity(smFile);
}

ConversionConfirmationDialog::~ConversionConfirmationDialog()
{
    delete ui;
}

void ConversionConfirmationDialog::spawnTiMidity(QString smFile)
{
    // make command list
    QString appDirStr = QCoreApplication::applicationDirPath();
    QString timidityBinPath;
    QStringList command;
    if(QSysInfo::windowsVersion() != QSysInfo::WV_None){
        timidityBinPath = appDirStr + "/TiMidity++/timidity.exe";
        command << "-Os"; // output to sound device? (this is for Linux environment)
        command << "-c" << appDirStr + "/TiMidity++/sf2/SGM_v2.01.cfg";
        command << smFile;
    } else {
        timidityBinPath = "timidity";
        command << "-Os"; // output to ALSA PCM Device
        command << "-c" << appDirStr + "/sf2/SGM_v2.01.cfg";
        command << smFile;
    }

    // spawn TiMidity++ process
    timidityProc.start(timidityBinPath, command);
}

void ConversionConfirmationDialog::killTiMidity()
{
    // kill TiMidity++ process
    if(timidityProc.state() == QProcess::Running)
        timidityProc.kill();
}

void ConversionConfirmationDialog::spawnConvertingDialog()
{
    ConvertingDialog* cd = new ConvertingDialog(dynamic_cast<QWidget*>(this->parent()), this->smFile);
    connect(cd, &ConvertingDialog::finished, cd, &ConvertingDialog::deleteLater);

    // kill TiMidity++ process before starting to convert
    killTiMidity();

    this->close();
    cd->show();
}

void ConversionConfirmationDialog::beforeExit()
{
    // kill TiMidity++ process
    killTiMidity();
}

