#include "workerthread.h"
#include <QThread>

WorkerThread::WorkerThread(QObject* parent)
    : QThread(parent), m_duration(0)
{
}

void WorkerThread::configure(const QString& type, int durationMs)
{
    m_type = type;
    m_duration = durationMs;
}

void WorkerThread::run()
{
    for (int i = 0; i <= 100; i += 10) {
        QThread::msleep(m_duration / 10);
        emit progressChanged(i);
        emit messageChanged(QString("%1 %2% 진행 중...").arg(m_type).arg(i));
    }
}
