#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QLineEdit>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QToolButton>
#include <QWidget>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QWidget {
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget* parent = 0);
    ~ConfigDialog();

private:
    void setupConfigSelectorButtons();
    void connectCSBToPage(QToolButton* toolButton, QWidget* targetPageWidget);
    void restoreSettingsToUI();

private slots:
    void saveSettingsAndClose();

private:
    Ui::ConfigDialog* ui;
};

class CDChangePageSlotObject : public QObject {
    Q_OBJECT

public:
    CDChangePageSlotObject(QObject* parent, QStackedWidget* targetSW,
                           int targetPage);
    ~CDChangePageSlotObject();

public slots:
    void changePage();

private:
    QStackedWidget* targetSW;
    int targetPage;
};

class FileBrowseLineEditConnector : public QObject {
    Q_OBJECT

public:
    FileBrowseLineEditConnector(QWidget* parent, QPushButton* browseButton,
                                QLineEdit* lineEdit, bool directory,
                                bool absolute);
    ~FileBrowseLineEditConnector();
    void setAcceptFileTypes(QString& fileTypesString);

public slots:
    void browseFile();

private:
    QWidget* m_parent;
    bool m_absolute;
    bool m_directory;
    QString m_acceptFileTypes;
    QLineEdit* m_lineEdit;
};

#endif  // CONFIGDIALOG_HPP
