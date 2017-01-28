#ifndef CONVERTINGWORKER_HPP
#define CONVERTINGWORKER_HPP

#include <QObject>
#include <QThread>

class ConvertingWorker : public QThread {
    Q_OBJECT
public:
    ConvertingWorker(QObject* parent, QString& filename,
                     QString& outputDirectory);
    void run() Q_DECL_OVERRIDE;

signals:
    void progress(int prog, int max);

private:
    QString filename;
    QString outputDirectory;
};

#endif  // CONVERTINGWORKER_HPP
