#ifndef CONVERSIONCONFIRMATIONDIALOG_H
#define CONVERSIONCONFIRMATIONDIALOG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class ConversionConfirmationDialog;
}

class ConversionConfirmationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConversionConfirmationDialog(QWidget *parent = 0);
    ConversionConfirmationDialog(QWidget *parent, QString smFile);
    ~ConversionConfirmationDialog();

private:
    void spawnTiMidity(QString smFile);
    void killTiMidity();

private slots:
    void spawnConvertingDialog();
    void beforeExit();

private:
    Ui::ConversionConfirmationDialog *ui;
    QProcess timidityProc;
    QString smFile;
};

#endif // CONVERSIONCONFIRMATIONDIALOG_H
