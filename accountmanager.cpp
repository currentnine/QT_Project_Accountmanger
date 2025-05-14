#include "accountmanager.h"

AccountManager::AccountManager() {}

bool AccountManager::addAccount(const Account& account) {
    if (findByAccountNumber(account.accountNumber))
        return false;
    m_accounts.append(account);
    return true;
}

bool AccountManager::removeAccount(const QString& accountNumber) {
    for (int i = 0; i < m_accounts.size(); ++i) {
        if (m_accounts[i].accountNumber == accountNumber) {
            m_accounts.removeAt(i);
            return true;
        }
    }
    return false;
}

Account* AccountManager::login(const QString& accountNumber, const QString& password) {
    for (auto& acc : m_accounts) {
        if (acc.accountNumber == accountNumber && acc.password == password)
            return &acc;
    }
    return nullptr;
}

Account* AccountManager::findByAccountNumber(const QString& accountNumber) {
    for (auto& acc : m_accounts) {
        if (acc.accountNumber.trimmed() == accountNumber.trimmed())
            return &acc;
    }
    return nullptr;
}


bool AccountManager::deposit(Account* account, int amount) {
    if (!account || amount <= 0) return false;
    account->balance += amount;
    return true;
}

bool AccountManager::withdraw(Account* account, int amount) {
    if (!account || amount <= 0 || account->balance < amount)
        return false;
    account->balance -= amount;
    return true;
}

bool AccountManager::transfer(Account* from, Account* to, int amount) {
    if (!from || !to || amount <= 0 || from->balance < amount)
        return false;
    from->balance -= amount;
    to->balance += amount;
    return true;
}

QVector<Account>& AccountManager::getAccounts() {
    return m_accounts;
}

QMap<QString, QString> AccountManager::createAccountNameMap() const {
    QMap<QString, QString> map;
    for (const Account& acc : m_accounts) {
        map[acc.accountNumber] = acc.name;
    }
    return map;
}

bool AccountManager::changePassword(const QString& accountNumber, const QString& newPassword) {
    for (Account& acc : m_accounts) {
        if (acc.accountNumber == accountNumber) {
            acc.password = newPassword;
            return true;
        }
    }
    return false;
}
