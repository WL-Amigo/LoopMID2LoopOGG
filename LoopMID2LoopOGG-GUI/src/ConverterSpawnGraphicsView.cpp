#include "includes/ConverterSpawnGraphicsView.hpp"
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QUrl>

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
    QString smfFullPathDecoded = QUrl::fromPercentEncoding(smfFullPath.toLocal8Bit());
    qDebug() << smfFullPath;
    qDebug() << QUrl::fromPercentEncoding(smfFullPath.toLocal8Bit());
    convertingDialog = new ConvertingDialog(dynamic_cast<QWidget*>(this->parent()), smfFullPathDecoded);
    connect(convertingDialog, &ConvertingDialog::finished, convertingDialog, &QObject::deleteLater);

    convertingDialog->show();
}

void ConverterSpawnGraphicsView::destroyDialog() {

}
