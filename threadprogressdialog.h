#ifndef THREADPROGRESSDIALOG_H
#define THREADPROGRESSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QVBoxLayout>
#include <QEvent>

class WorkerThread;

class ThreadProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThreadProgressDialog(QWidget *parent = nullptr);
    void startTransaction(const QString& type, int durationMs);

private slots:
    void updateProgress(int value);
    void updateMessage(const QString& msg);
    void rotateSpinner();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;  // ✅ 선언 추가

private:
    QLabel* messageLabel;
    QWidget* spinnerWidget;
    QTimer* spinnerTimer;
    int spinnerAngle = 0;
    WorkerThread* worker;
};

#endif // THREADPROGRESSDIALOG_H
