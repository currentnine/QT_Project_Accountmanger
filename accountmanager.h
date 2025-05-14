#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QString>
#include <QVector>
#include <QMap>

/// 단일 계좌 정보 구조체
struct Account {
    QString name;            // 사용자 이름
    QString accountNumber;   // 계좌번호 (고유 ID)
    QString password;        // 로그인용 비밀번호
    int balance = 0;         // 잔액
};

/// 계좌들을 관리하고 로그인, 송금, 입출금 기능을 제공하는 클래스
class AccountManager
{
public:
    AccountManager();

    bool addAccount(const Account& account);                      // 계좌 추가
    Account* login(const QString& accountNumber, const QString& password);  // 로그인
    bool deposit(Account* account, int amount);                   // 입금
    bool withdraw(Account* account, int amount);                 // 출금
    bool transfer(Account* from, Account* to, int amount);       // 송금
    bool removeAccount(const QString& accountNumber);            // 계좌 삭제
    Account* findByAccountNumber(const QString& accountNumber);  // 계좌번호로 찾기
    QMap<QString, QString> createAccountNameMap() const;         // 계좌번호 → 이름 매핑 반환
    QVector<Account>& getAccounts();                             // 전체 계좌 목록 반환

    bool changePassword(const QString &accountNumber, const QString &newPassword);
private:
    QVector<Account> m_accounts;  // 계좌 목록 저장 (메모리 내)
};

#endif // ACCOUNTMANAGER_H
