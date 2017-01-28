#ifndef MULTICONVERTINGDIALOG_HPP
#define MULTICONVERTINGDIALOG_HPP

#include <QFuture>
#include <QFutureWatcher>
#include <QStringList>
#include <QWidget>
#include <QtConcurrent>

namespace Ui {
class MultiConvertingDialog;
}

class MultiConvertingDialog : public QWidget {
    Q_OBJECT

public:
    explicit MultiConvertingDialog(QWidget *parent = 0);
    MultiConvertingDialog(QStringList &smFileList, QWidget *parent = 0);
    ~MultiConvertingDialog();

private:
    Ui::MultiConvertingDialog *ui;
    QStringList fileNameList;
    QFuture<bool> results;
    QFutureWatcher<bool> watcher;

private:
    void showEvent(QShowEvent *event);

private slots:
    void onProgressValueChanged(int value);
    void onOneWorkerFinished(int index);
};

#endif  // MULTICONVERTINGDIALOG_HPP
