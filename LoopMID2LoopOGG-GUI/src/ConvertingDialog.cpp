#include "includes/ConvertingDialog.hpp"
#include "ui_convertingdialog.h"

#include <QPushButton>
#include <QDebug>
#include <QSettings>

ConvertingDialog::ConvertingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvertingDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
}

ConvertingDialog::ConvertingDialog(QWidget *parent, QString &filename) :
    QDialog(parent),
    ui(new Ui::ConvertingDialog),
    worker(nullptr)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);

    QSettings settings;
    Q_ASSERT(!settings.value("outputDirectory").isNull());
    QString outputDir = settings.value("outputDirectory").toString();
    worker = new ConvertingWorker(this, filename, outputDir);
    connect(worker, &ConvertingWorker::finished, this, &ConvertingDialog::whenFinished);
}

ConvertingDialog::~ConvertingDialog()
{
    delete ui;
    delete worker;
}

void ConvertingDialog::whenFinished(){
    qDebug() << "finished";
    ui->label->setText("変換完了!");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ConvertingDialog::showEvent(QShowEvent *showEvent){
    worker->start();
}
