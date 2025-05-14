#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include "accountmanager.h"
#include <QString>

class FileStorage
{
public:
    static void init() ;

    static bool saveAccounts(const QString& filename, const QVector<Account>& accounts);
    static bool loadAccounts(const QString& filename, QVector<Account>& accounts);

    // DB 기반 함수 (saveAccounts, loadAccounts 대신 사용 가능)
    static bool saveAccounts2(const QString& filename, const QVector<Account>& accounts);
    static bool loadAccounts2(const QString& filename, QVector<Account>& accounts);

    static void testLoadAccountsTable(const QString& dbFilename) ;


    static bool updatePassword(const QString &accountNumber, const QString &currentPassword, const QString &newPassword);
};

#endif // FILESTORAGE_H

