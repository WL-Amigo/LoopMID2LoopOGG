#ifndef CONVERTERSPAWNGRAPHICSVIEW_H
#define CONVERTERSPAWNGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include "ConvertingDialog.hpp"

class ConverterSpawnGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    ConverterSpawnGraphicsView(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void destroyDialog();

private:
    ConvertingDialog* convertingDialog;
};

#endif // CONVERTERSPAWNGRAPHICSVIEW_H
