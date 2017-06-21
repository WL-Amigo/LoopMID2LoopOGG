#include "includes/ConverterSpawnGraphicsView.hpp"

#include "includes/MultiConvertingDialog.hpp"
#include "includes/conversionconfirmationdialog.h"

#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QStringList>
#include <QUrl>

ConverterSpawnGraphicsView::ConverterSpawnGraphicsView(QWidget *parent) {
    setAcceptDrops(true);
}

QString getSMFFullPath(QString mimeText) {
    mimeText.replace('\n', "");
    mimeText.replace('\r', "");
    mimeText.replace("file://", "");
    mimeText = QUrl::fromPercentEncoding(mimeText.toUtf8());
    if (mimeText.indexOf(".mid", 0, Qt::CaseInsensitive)
        >= mimeText.size() - 5) {  // permit ".midi", ".MID" etc...
        return mimeText;
    }

    return "";
}

QStringList getSMFFullPathList(QString mimeText) {
    mimeText.replace('\r', '\n');
    mimeText.replace("\n\n", "\n");
    QStringList ret;
    QStringList splitedMIMEText = mimeText.split('\n');
    for (QString singleMT : splitedMIMEText) {
        QString fullPath = getSMFFullPath(singleMT);
        if (!fullPath.isEmpty()) ret.push_back(fullPath);
    }
    return ret;
}

void ConverterSpawnGraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        if (getSMFFullPathList(event->mimeData()->text()).size() > 0) {
            event->acceptProposedAction();
        }
    }
}

void ConverterSpawnGraphicsView::dropEvent(QDropEvent *event) {
    QStringList smfFullPathList = getSMFFullPathList(event->mimeData()->text());
    qDebug() << smfFullPathList;
    qDebug() << smfFullPathList.size();

    QSettings s;
    if (smfFullPathList.size() > 1) {  // multi file conversion
        MultiConvertingDialog *mcd = new MultiConvertingDialog(smfFullPathList);
        mcd->setAttribute(Qt::WA_DeleteOnClose);
        mcd->show();
        return;
    } else {  // single file conversion
        QString smfFullPath = smfFullPathList.at(0);
        if (s.value("preview").toBool()) {
            ConversionConfirmationDialog *ccd
                = new ConversionConfirmationDialog(
                    dynamic_cast<QWidget *>(this->parent()), smfFullPath);
            connect(ccd, &ConversionConfirmationDialog::finished, ccd,
                    &ConversionConfirmationDialog::deleteLater);
            ccd->show();
        } else {
            convertingDialog = new ConvertingDialog(
                dynamic_cast<QWidget *>(this->parent()), smfFullPath);
            connect(convertingDialog, &ConvertingDialog::finished,
                    convertingDialog, &QObject::deleteLater);
            convertingDialog->show();
        }
    }
}

void ConverterSpawnGraphicsView::destroyDialog() {}
