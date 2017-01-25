#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QWidget>

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
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_HPP
