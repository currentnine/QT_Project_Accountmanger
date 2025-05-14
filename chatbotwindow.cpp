#include "chatbotwindow.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QSslSocket>
#include <QCloseEvent>
#include <QTimer>


ChatBotWindow::ChatBotWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("ChatGPT 챗봇 상담");

    outputEdit = new QTextEdit(this);
    outputEdit->setReadOnly(true);

    inputLine = new QLineEdit(this);
    sendButton = new QPushButton("보내기", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(outputEdit);
    layout->addWidget(inputLine);
    layout->addWidget(sendButton);
    closeButton = new QPushButton("닫기", this);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &ChatBotWindow::close);
    setLayout(layout);
    networkManager = new QNetworkAccessManager(this);
    connect(sendButton, &QPushButton::clicked, this, &ChatBotWindow::sendMessage);
    setFixedSize(254, 332);  // 예: 400x500 크기
    QTimer::singleShot(1000, this, &ChatBotWindow::sendInitialMessage);
}

void ChatBotWindow::sendInitialMessage()
{
    // 첫 번째 메시지 "무엇을 도와드릴까요?"
    outputEdit->append("🤖: 무엇을 도와드릴까요?");
}



void ChatBotWindow::sendMessage()
{
    QString userMsg = inputLine->text();
    if (userMsg.isEmpty()) return;

    outputEdit->append("🧑: " + userMsg);
    inputLine->clear();

    sendToGPT(userMsg);
}

void ChatBotWindow::sendToGPT(const QString& userMsg)
{

    qDebug() << "SSL 지원 여부:" << QSslSocket::supportsSsl();
    qDebug() << "SSL 라이브러리 경로:" << QSslSocket::sslLibraryBuildVersionString();
    QNetworkRequest request(QUrl("https://api.openai.com/v1/chat/completions"));
    request.setRawHeader("Authorization", "your-api-key");  // ⭐ 여기에 본인의 API 키 입력
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject message;
    message["role"] = "user";
    message["content"] = userMsg;

    QJsonArray messages;
    messages.append(message);

    QJsonObject body;
    body["model"] = "gpt-3.5-turbo"; // 또는 "gpt-4"
    body["messages"] = messages;

    QNetworkReply* reply = networkManager->post(request, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "HTTP 상태 코드:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "오류 설명:" << reply->errorString();
            qDebug() << "응답 본문:" << reply->readAll();
            reply->deleteLater();
            return;
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString replyText = jsonDoc["choices"][0]["message"]["content"].toString();
        outputEdit->append("🤖: " + replyText);
        reply->deleteLater();
    });

}

void ChatBotWindow::closeEvent(QCloseEvent *event) {
    event->accept();  // 창을 진짜로 닫음
    // 또는 event->ignore();  // 닫히지 않도록 방지
}
