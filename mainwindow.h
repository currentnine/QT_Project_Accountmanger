#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include "chatbotwindow.h"
#include "accountmanager.h"
#include "filestorage.h"
#include "transactionlogger.h"
#include "registerdialog.h"
#include "customercenterwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Account;  // Account 구조체 전방 선언

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();
    void on_depositButton_clicked();
    void on_withdrawButton_clicked();
    void on_transferButton_clicked();
    void on_searchButton_clicked();
    void showProgressDialog(const QString& type, int duration);
    void on_registerButton_clicked();
    void on_favoriteButton_clicked();
    void loadFavoritesList();
private:
    Ui::MainWindow *ui;

    AccountManager manager;     // 계좌 로직 관리 객체
    Account* currentAccount = nullptr; // 현재 로그인한 계좌
    ChatBotWindow* chatbotWindow = nullptr;
    QMap<QString, QString> accountNameMap;  // 계좌번호 → 이름 매핑
    void updateLogView();

    void initCustomerCenterButtonAnimation();
private slots:
    void on_customerCenterButton_clicked();
    void on_searchPageButton_clicked();
    void on_backButton_clicked();
    void on_logoutButton_clicked();
    void on_changePasswordButton_clicked();

private:
    void init();  // 초기화용 내부 함수
    bool isLoggedIn = false;
    void setupCustomTitleBar();
    QWidget *m_dragWidget = nullptr;
    QLabel *m_dragWidget_2 = nullptr;

    QPoint m_dragPos;

public:
    bool getLoginState() const;
    void setLoginState(bool state);
    void setButtonStates();
    bool eventFilter(QObject *watched, QEvent *event) override;

    void showMessage(const QString& htmlText, QMessageBox::Icon icon = QMessageBox::NoIcon);
private:
    QString getAccountNumberInput(bool* ok);
protected:
        void on_pageChanged(int index);
private:
    CustomerCenterWidget* customerCenter = nullptr;
    bool isDarkMode = false;


};

#endif // MAINWINDOW_H
