#include "includes/ConfigDialog.hpp"
#include "ui_ConfigDialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QDebug>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    // setup config selector buttons
    setupConfigSelectorButtons();
    ui->pagesWidget->setCurrentIndex(0);

    // connect behavior
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &ConfigDialog::close);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setupConfigSelectorButtons(){
    // connect each config selector button to page
    CDChangePageSlotObject* cpso = new CDChangePageSlotObject(this, ui->pagesWidget, ui->pagesWidget->indexOf(ui->CPOutput));
    connect(ui->CSBOutput, &QToolButton::clicked, cpso, &CDChangePageSlotObject::changePage);
    connect(this, &ConfigDialog::destroyed, cpso, &CDChangePageSlotObject::deleteLater);

    cpso = new CDChangePageSlotObject(this, ui->pagesWidget, ui->pagesWidget->indexOf(ui->CPAbout));
        connect(ui->CSBAbout, &QToolButton::clicked, cpso, &CDChangePageSlotObject::changePage);
        connect(this, &ConfigDialog::destroyed, cpso, &CDChangePageSlotObject::deleteLater);
}


// ChangePageSlotObject

CDChangePageSlotObject::CDChangePageSlotObject(QObject* parent, QStackedWidget *targetSW, int targetPage):
    QObject(parent),
    targetSW(targetSW),
    targetPage(targetPage)
{

}

CDChangePageSlotObject::~CDChangePageSlotObject(){

}

void CDChangePageSlotObject::changePage(){
    targetSW->setCurrentIndex(targetPage);
    qDebug().noquote() << QString::asprintf("change page: %d", targetPage);
}
