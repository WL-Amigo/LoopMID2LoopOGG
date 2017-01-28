#include "includes/MultiConvertingDialog.hpp"
#include "LoopOGGGenerator.hpp"
#include "ui_MultiConvertingDialog.h"

#include <QModelIndex>
#include <QPushButton>
#include <QSettings>
#include <QTableWidgetItem>
#include <QVariant>

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
        if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
            return LoopOGGGenerator::convert(
                fileName, outputDirectory,
                appDirStr + "/TiMidity++/timidity.exe",
                appDirStr + "/TiMidity++/sf2/SGM_v2.01.cfg",
                appDirStr + "/oggenc2.exe");
        } else {
            return LoopOGGGenerator::convert(
                fileName, outputDirectory, "timidity",
                appDirStr + "/sf2/SGM_v2.01.cfg", "oggenc");
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
        ui->progressDetailTable->setItem(
            idx, 0, new QTableWidgetItem(smFileList.at(idx)));
        ui->progressDetailTable->setItem(
            idx, 1, new QTableWidgetItem(tr("pending or converting...")));
    }

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
    this->results = QtConcurrent::mapped(this->fileNameList, TestWorker());
    this->watcher.setFuture(this->results);
    connect(&watcher, &QFutureWatcher<bool>::progressValueChanged, this,
            &MultiConvertingDialog::onProgressValueChanged);
    connect(&watcher, &QFutureWatcher<bool>::resultReadyAt, this,
            &MultiConvertingDialog::onOneWorkerFinished);
}

void MultiConvertingDialog::onProgressValueChanged(int value) {
    ui->progressBar->setValue(value);
    if (value == this->fileNameList.size())
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void MultiConvertingDialog::onOneWorkerFinished(int index) {
    ui->progressDetailTable->item(index, 1)->setText(tr("Done!"));
}
