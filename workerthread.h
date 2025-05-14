#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QString>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject* parent = nullptr);
    void configure(const QString& type, int durationMs);

signals:
    void progressChanged(int);
    void messageChanged(const QString&);

protected:
    void run() override;

private:
    QString m_type;
    int m_duration;
};

#endif // WORKERTHREAD_H
