#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "accountmanager.h"
#include <QMessageBox>

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDialog(Account* currentAccount, AccountManager* accountManager, QWidget *parent = nullptr);
    ~ChangePasswordDialog() = default;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onAcceptClicked();

private:
    void showMessage(const QString& html, QMessageBox::Icon icon = QMessageBox::NoIcon);

    Account* currentAccount;
    AccountManager* accountManager;

    QLineEdit* currentPasswordEdit;
    QLineEdit* newPasswordEdit;
    QLineEdit* confirmPasswordEdit;
    QPushButton* acceptButton;
};

#endif // CHANGEPASSWORDDIALOG_H
