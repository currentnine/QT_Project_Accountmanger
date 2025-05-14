#ifndef CHATBOTWINDOW_H
#define CHATBOTWINDOW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class QTextEdit;
class QLineEdit;
class QPushButton;

class ChatBotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatBotWindow(QWidget *parent = nullptr);
    void sendInitialMessage();

private slots:
    void sendMessage();

private:
    QTextEdit* outputEdit;
    QLineEdit* inputLine;
    QPushButton* sendButton;
    QPushButton* closeButton;  // 추가


    QNetworkAccessManager* networkManager;
    void sendToGPT(const QString& userMsg);

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // CHATBOTWINDOW_H
