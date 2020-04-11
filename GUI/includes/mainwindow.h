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
    Q_PROPERTY(int outputAmp READ outputAmp WRITE setOutputAmp NOTIFY outputAmpChanged)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int outputAmp() const;
    void setOutputAmp(int newOutputAmp);

private slots:
    void onBrowseButtonPressed();
    void onPreviewCheckBoxChanged();
    void onOutputAmpValueChanged(int newOutputAmp);

signals:
    void outputAmpChanged(int newOutputAmp);

private:
    Ui::MainWindow *ui;
    ConvertingDialog *convertingDialog;
    ConfigDialog *configDialog;
};

#endif  // MAINWINDOW_H
