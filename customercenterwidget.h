#ifndef CUSTOMERCENTERWIDGET_H
#define CUSTOMERCENTERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "chatbotwindow.h"

class CustomerCenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerCenterWidget(QWidget *parent = nullptr);
    ~CustomerCenterWidget();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_chatbotButton_clicked();
    void on_callButton_clicked();

private:
    ChatBotWindow* chatbotWindow = nullptr;
    QPushButton* chatbotButton;
    QPushButton* callButton;
};

#endif // CUSTOMERCENTERWIDGET_H
