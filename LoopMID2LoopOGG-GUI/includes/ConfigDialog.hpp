#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QWidget>
#include <QString>
#include <QListWidgetItem>
#include <QStackedWidget>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

private:
    void setupConfigSelectorButtons();

private:
    Ui::ConfigDialog *ui;

};

class CDChangePageSlotObject : public QObject
{
    Q_OBJECT

public:
    CDChangePageSlotObject(QObject* parent, QStackedWidget* targetSW, int targetPage);
    ~CDChangePageSlotObject();

public slots:
    void changePage();

private:
    QStackedWidget* targetSW;
    int targetPage;


};

#endif // CONFIGDIALOG_HPP
