#include "includes/ConverterSpawnGraphicsView.hpp"
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>

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
    qDebug() << smfFullPath;
    convertingDialog = new ConvertingDialog(this, smfFullPath);
    connect(convertingDialog, &ConvertingDialog::finished, convertingDialog, &QObject::deleteLater);

    convertingDialog->show();
}

void ConverterSpawnGraphicsView::destroyDialog() {

}
