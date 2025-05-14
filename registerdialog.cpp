#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "filestorage.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>

RegisterDialog::RegisterDialog(AccountManager* manager, QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog), m_manager(manager)
{
    // ✅ 기존 UI 제거 → 수동 UI 작성
    // delete ui;  ❌ 필요 없음
    // ui = nullptr;

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setFixedSize(320, 300);

    // 제목 라벨
    QLabel* titleLabel = new QLabel("<span style='color:#FF6A00; font-size:16px; font-weight:bold;'>회원가입</span>");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 입력 필드
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("이름");
    accountEdit = new QLineEdit(this);
    accountEdit->setPlaceholderText("계좌번호");
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("비밀번호");
    passwordEdit->setEchoMode(QLineEdit::Password);
    confirmEdit = new QLineEdit(this);
    confirmEdit->setPlaceholderText("비밀번호 확인");
    confirmEdit->setEchoMode(QLineEdit::Password);

    // 버튼
    registerButton = new QPushButton("회원가입", this);
    registerButton->setStyleSheet("QPushButton { background-color: #FF6A00; color: white; font-weight: bold; }");
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);

    // 레이아웃 구성
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(nameEdit);
    layout->addWidget(accountEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(confirmEdit);
    layout->addSpacing(10);
    layout->addWidget(registerButton);

    setLayout(layout);
}

void RegisterDialog::onRegisterClicked()
{
    QString name = nameEdit->text().trimmed();
    QString accNum = accountEdit->text().trimmed();
    QString pw = passwordEdit->text();
    QString pwCheck = confirmEdit->text();

    if (name.isEmpty() || accNum.isEmpty() || pw.isEmpty() || pwCheck.isEmpty()) {
        showWarning("모든 항목을 입력해주세요.");
        return;
    }
    if (pw != pwCheck) {
        showWarning("비밀번호가 일치하지 않습니다.");
        return;
    }
    if (m_manager->findByAccountNumber(accNum)) {
        showWarning("이미 존재하는 계좌번호입니다.");
        return;
    }

    Account newAcc;
    newAcc.name = name;
    newAcc.accountNumber = accNum;
    newAcc.password = pw;
    newAcc.balance = 0;

    m_manager->addAccount(newAcc);
    FileStorage::saveAccounts2("accounts.db", m_manager->getAccounts());

    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText("<span style='color:#FF6A00;'>회원가입이 완료되었습니다.</span>");
    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->exec();

    accept();
}

void RegisterDialog::showWarning(const QString& html)
{
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText(QString("<span style='color:#FF6A00;'>%1</span>").arg(html));
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->exec();
}

void RegisterDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();  // ESC 눌렀을 때 닫기
    } else {
        QDialog::keyPressEvent(event);
    }
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}
