#include "includes/ConverterSpawnGraphicsView.hpp"

#include "includes/conversionconfirmationdialog.h"

#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QUrl>
#include <QSettings>

ConverterSpawnGraphicsView::ConverterSpawnGraphicsView(QWidget *parent)
{
    setAcceptDrops(true);
}

QString getSMFFullPath(QString mimeText){
    mimeText.replace('\n', "");
    mimeText.replace('\r', "");
    mimeText.replace("file://", "");
    if(mimeText.indexOf(".mid", 0, Qt::CaseInsensitive) >= mimeText.size() - 5){ // permit ".midi", ".MID" etc...
        return mimeText;
    }

    return "";
}

void ConverterSpawnGraphicsView::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasText()){
        if(getSMFFullPath(event->mimeData()->text()).size() > 0){
            event->acceptProposedAction();
        }
    }
}

void ConverterSpawnGraphicsView::dropEvent(QDropEvent *event) {
    QString smfFullPath = getSMFFullPath(event->mimeData()->text());
    QString smfFullPathDecoded = QUrl::fromPercentEncoding(smfFullPath.toUtf8());
    qDebug() << smfFullPath;
    qDebug() << QUrl::fromPercentEncoding(smfFullPath.toUtf8());

    QSettings s;
    if(s.value("preview").toBool()){
        ConversionConfirmationDialog* ccd = new ConversionConfirmationDialog(dynamic_cast<QWidget*>(this->parent()), smfFullPathDecoded);
        connect(ccd, &ConversionConfirmationDialog::finished, ccd, &ConversionConfirmationDialog::deleteLater);
        ccd->show();
    } else {
        convertingDialog = new ConvertingDialog(dynamic_cast<QWidget*>(this->parent()), smfFullPathDecoded);
        connect(convertingDialog, &ConvertingDialog::finished, convertingDialog, &QObject::deleteLater);
        convertingDialog->show();
    }


}

void ConverterSpawnGraphicsView::destroyDialog() {

}
