#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QLineEdit>
#include <QListWidgetItem>
#include <QMessageBox>
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
    void setupFileBrowseLineEdits();
    void connectCSBToPage(QToolButton* toolButton, QWidget* targetPageWidget);
    void restoreSettingsToUI();
    void setPlatformSpecificBehavior();

private slots:
    void saveSettingsAndClose();

private:
    Ui::ConfigDialog* ui;
    QMessageBox* m_HowToEnableAACTips;
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
    void setAcceptFileTypes(const QString& fileTypesString);

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
