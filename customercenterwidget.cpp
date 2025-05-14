#include "customercenterwidget.h"
#include "chatbotwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>
#include <QLabel>

CustomerCenterWidget::CustomerCenterWidget(QWidget *parent)
    : QWidget(parent)
{
    // ✅ 타이틀바 제거
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    // setFixedSize(300, 200);
    setFixedSize(254,332);
    QLabel* titleLabel = new QLabel("<span style='color:#FF6A00; font-size:16px; font-weight:bold;'>고객센터</span>", this);
    titleLabel->setAlignment(Qt::AlignCenter);

    chatbotButton = new QPushButton("챗봇 상담", this);
    callButton = new QPushButton("전화 상담 연결", this);

    chatbotButton->setStyleSheet("QPushButton { background-color: #FF6A00; color: white; font-weight: bold; }");
    callButton->setStyleSheet("QPushButton { background-color: #999999; color: white; font-weight: bold; }");

    connect(chatbotButton, &QPushButton::clicked, this, &CustomerCenterWidget::on_chatbotButton_clicked);
    connect(callButton, &QPushButton::clicked, this, &CustomerCenterWidget::on_callButton_clicked);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addSpacing(10);
    layout->addWidget(chatbotButton);
    layout->addWidget(callButton);
    layout->setAlignment(Qt::AlignCenter);

    setLayout(layout);
}

CustomerCenterWidget::~CustomerCenterWidget()
{
    // Qt가 부모-자식 관계로 메모리 자동 해제
}

void CustomerCenterWidget::on_chatbotButton_clicked()
{
    if (!chatbotWindow) {
        chatbotWindow = new ChatBotWindow(this);
    }
    chatbotWindow->show();
    chatbotWindow->raise();
    chatbotWindow->activateWindow();
}

void CustomerCenterWidget::on_callButton_clicked()
{
    QUrl url("https://slack.com/intl/ko-kr/");
    QDesktopServices::openUrl(url);
}

void CustomerCenterWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QWidget::keyPressEvent(event);
    }
}
