#include "threadprogressdialog.h"
#include "workerthread.h"

ThreadProgressDialog::ThreadProgressDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setFixedSize(300, 150);


    setWindowTitle("처리 중...");

    // 메시지
    messageLabel = new QLabel("처리 중...", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("color: #FF6A00;");
    // 내부 회전 스피너 위젯
    spinnerWidget = new QWidget(this);
    spinnerWidget->setFixedSize(64, 64);

    // 타이머로 회전 각도 변경
    spinnerTimer = new QTimer(this);
    connect(spinnerTimer, &QTimer::timeout, this, &ThreadProgressDialog::rotateSpinner);
    spinnerTimer->start(16); // 약 60fps

    // 커스텀 페인터
    spinnerWidget->installEventFilter(this); // paintEvent 대체 처리

    // 레이아웃 구성
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(spinnerWidget, 0, Qt::AlignCenter);
    layout->addWidget(messageLabel, 0, Qt::AlignCenter);
    setLayout(layout);

    // 백그라운드 쓰레드
    worker = new WorkerThread(this);
    connect(worker, &WorkerThread::progressChanged, this, &ThreadProgressDialog::updateProgress);
    connect(worker, &WorkerThread::messageChanged, this, &ThreadProgressDialog::updateMessage);
}

void ThreadProgressDialog::startTransaction(const QString& type, int durationMs)
{
    messageLabel->setText("시작 중...");
    spinnerAngle = 0;
    worker->configure(type, durationMs);
    worker->start();
    exec();  // 모달 실행
}

void ThreadProgressDialog::updateProgress(int value)
{
    if (value >= 100) {
        spinnerTimer->stop();
        accept();  // 다이얼로그 종료
    }
}

void ThreadProgressDialog::updateMessage(const QString& msg)
{
    messageLabel->setText(msg);
}

void ThreadProgressDialog::rotateSpinner()
{
    spinnerAngle = (spinnerAngle + 5) % 360;
    spinnerWidget->update();
}

// ───────────────────────────────
// paintEvent 핸들링
// ───────────────────────────────
bool ThreadProgressDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == spinnerWidget && event->type() == QEvent::Paint) {
        QPainter painter(spinnerWidget);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(spinnerWidget->width() / 2, spinnerWidget->height() / 2);
        painter.rotate(spinnerAngle);

        int numLines = 12;
        int radius = 24;
        for (int i = 0; i < numLines; ++i) {
            int alpha = 255 * (i + 1) / numLines;
            QColor color(0xFF, 0x6A, 0x00, alpha);  // ✅ 변경된 색상
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawEllipse(QPointF(0, -radius), 3, 3);
            painter.rotate(360.0 / numLines);
        }

        return true;
    }
    return QDialog::eventFilter(watched, event);
}
