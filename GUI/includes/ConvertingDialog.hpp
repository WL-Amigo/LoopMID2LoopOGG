#ifndef CONVERTINGDIALOG_HPP
#define CONVERTINGDIALOG_HPP

#include <QDialog>

#include "ConvertingWorker.hpp"

namespace Ui {
class ConvertingDialog;
}

class ConvertingDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConvertingDialog(QWidget *parent = 0);
    ConvertingDialog(QWidget *parent, QString &filename);
    ~ConvertingDialog();

private:
    Ui::ConvertingDialog *ui;
    ConvertingWorker *worker;
    void showEvent(QShowEvent *showEvent);

private slots:
    void whenFinished();
};

#endif  // CONVERTINGDIALOG_HPP
