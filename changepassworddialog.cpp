#include "changepassworddialog.h"
#include "filestorage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QKeyEvent>

ChangePasswordDialog::ChangePasswordDialog(Account* currentAccount, AccountManager* accountManager, QWidget *parent)
    : QDialog(parent), currentAccount(currentAccount), accountManager(accountManager)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setFixedSize(320, 300);

    QLabel* titleLabel = new QLabel("<span style='color:#FF6A00; font-size:16px; font-weight:bold;'>비밀번호 변경</span>");
    titleLabel->setAlignment(Qt::AlignCenter);

    currentPasswordEdit = new QLineEdit(this);
    currentPasswordEdit->setPlaceholderText("현재 비밀번호");
    currentPasswordEdit->setEchoMode(QLineEdit::Password);

    newPasswordEdit = new QLineEdit(this);
    newPasswordEdit->setPlaceholderText("새 비밀번호");
    newPasswordEdit->setEchoMode(QLineEdit::Password);

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("새 비밀번호 확인");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    acceptButton = new QPushButton("변경", this);
    acceptButton->setStyleSheet("QPushButton { background-color: #FF6A00; color: white; font-weight: bold; }");
    connect(acceptButton, &QPushButton::clicked, this, &ChangePasswordDialog::onAcceptClicked);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(currentPasswordEdit);
    layout->addWidget(newPasswordEdit);
    layout->addWidget(confirmPasswordEdit);
    layout->addSpacing(10);
    layout->addWidget(acceptButton);

    setLayout(layout);
}

void ChangePasswordDialog::onAcceptClicked()
{
    QString currentPassword = currentPasswordEdit->text();
    QString newPassword = newPasswordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();

    if (currentPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        showMessage("모든 필드를 입력해 주세요.", QMessageBox::Warning);
        return;
    }

    if (newPassword != confirmPassword) {
        showMessage("새 비밀번호와 확인이 일치하지 않습니다.", QMessageBox::Warning);
        return;
    }

    if (FileStorage::updatePassword(currentAccount->accountNumber, currentPassword, newPassword)) {
        accountManager->changePassword(currentAccount->accountNumber, newPassword);
        showMessage("비밀번호가 성공적으로 변경되었습니다.");
        accept();
    } else {
        showMessage("현재 비밀번호가 올바르지 않거나 변경에 실패했습니다.", QMessageBox::Warning);
    }
}

void ChangePasswordDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void ChangePasswordDialog::showMessage(const QString& html, QMessageBox::Icon icon)
{
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText(QString("<span style='color:#FF6A00;'>%1</span>").arg(html));
    msgBox->setIcon(icon);
    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->exec();
}

