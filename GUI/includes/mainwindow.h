#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ConvertingDialog.hpp"
#include "includes/ConfigDialog.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onBrowseButtonPressed();
    void onPreviewCheckBoxChanged();

private:
    Ui::MainWindow *ui;
    ConvertingDialog *convertingDialog;
    ConfigDialog *configDialog;
};

#endif  // MAINWINDOW_H
