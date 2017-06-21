#include "includes/MultiConvertingDialog.hpp"
#include "LoopOGGGenerator.hpp"
#include "ui_MultiConvertingDialog.h"

#include <QFileInfo>
#include <QModelIndex>
#include <QPushButton>
#include <QSettings>
#include <QTableWidgetItem>
#include <QVariant>

#include "TiMidityCommandBuilderProvider.h"
#include "EncoderExecutorProvider.hpp"

#ifdef QT_DEBUG
#include <QThread>
#include <random>
class TestWorker {
public:
    typedef bool result_type;
    TestWorker() {}
    result_type operator()(const QString &fileName) {
        std::random_device rd;
        QThread::msleep(1000 + (rd() % 2000));
        qDebug().noquote() << fileName << ": done";
        return true;
    }
};
#endif

class MultiConversionWorker {
public:
    typedef bool result_type;
    MultiConversionWorker() {}
    result_type operator()(const QString &fileName) {
        // TODO: refactor: duplicated code (from ConvertingWorker)
        QSettings s;
        qDebug() << "worker started";
        qDebug() << "filename:" << fileName;
        QString outputDirectory = s.value("outputDirectory").toString();
        qDebug() << "output directory:" << outputDirectory;
        QString appDirStr = QCoreApplication::applicationDirPath();
        TiMidityCommandBuilder tcb = TiMidityCommandBuilderProvider::getDefault();
        OggVorbisEncoderExecutor ee = EncoderExecutorProvider::getOggVorbis();
        if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
            return LoopOGGGenerator::convert(
                fileName, outputDirectory,
                appDirStr + "/TiMidity++/timidity.exe",
                tcb,
                &ee);
        } else {
            return LoopOGGGenerator::convert(
                fileName, outputDirectory, "timidity",
                tcb, &ee);
        }
    }
};

MultiConvertingDialog::MultiConvertingDialog(QStringList &smFileList,
                                             QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MultiConvertingDialog),
      results(),
      watcher(),
      fileNameList(smFileList) {
    ui->setupUi(this);

    // set items to tableview
    for (int idx = 0; idx < smFileList.size(); idx++) {
        ui->progressDetailTable->insertRow(idx);
        QFileInfo fi(smFileList.at(idx));
        ui->progressDetailTable->setItem(idx, 0,
                                         new QTableWidgetItem(fi.fileName()));
        auto progressItem = new QTableWidgetItem(
            QIcon(":/converting.png"), tr("pending or converting..."));

        ui->progressDetailTable->setItem(idx, 1, progressItem);
    }
    ui->progressDetailTable->resizeColumnToContents(0);

    // set progress bar attributes
    ui->progressBar->setMaximum(smFileList.size());
    ui->progressBar->setValue(0);

    // connect button behavior and disable button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &MultiConvertingDialog::close);
}

MultiConvertingDialog::~MultiConvertingDialog() { delete ui; }

void MultiConvertingDialog::showEvent(QShowEvent *event) {
    // setup worker and watcher
    this->results
        = QtConcurrent::mapped(this->fileNameList, MultiConversionWorker());
    this->watcher.setFuture(this->results);
    connect(&watcher, &QFutureWatcher<bool>::progressValueChanged, this,
            &MultiConvertingDialog::onProgressValueChanged);
    connect(&watcher, &QFutureWatcher<bool>::resultReadyAt, this,
            &MultiConvertingDialog::onOneWorkerFinished);
}

void MultiConvertingDialog::onProgressValueChanged(int value) {
    ui->progressBar->setValue(value);
    if (value == this->fileNameList.size()) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->stateLabel->setText(tr("Finished!"));
    }
}

void MultiConvertingDialog::onOneWorkerFinished(int index) {
    bool result = results.resultAt(index);
    if (result) {
        ui->progressDetailTable->item(index, 1)->setText(
            tr("Done!", "single work"));
        ui->progressDetailTable->item(index, 1)->setIcon(
            QIcon(":/succeeded.png"));
    } else {
        ui->progressDetailTable->item(index, 1)->setText(
            tr("Failed by some error...", "single work"));
        ui->progressDetailTable->item(index, 1)->setIcon(QIcon(":/failed.png"));
    }
}
