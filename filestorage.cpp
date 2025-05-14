#include "filestorage.h"
#include <QFile>
#include <QTextStream>
#include <QtSql>
#include <QUuid>  // 고유한 연결 이름 생성을 위해

//────────────────────────────────────────
// 기존 파일 기반 함수 구현 (변경 없음)
//────────────────────────────────────────
// bool FileStorage::saveAccounts(const QString& filename, const QVector<Account>& accounts) {
//     QFile file(filename);
//     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//         return false;

//     QTextStream out(&file);
//     for (const Account& acc : accounts) {
//         out << acc.name << "," << acc.accountNumber << "," << acc.password << "," << acc.balance << "\n";
//     }
//     file.close();
//     return true;
// }

// bool FileStorage::loadAccounts(const QString& filename, QVector<Account>& accounts) {
//     QFile file(filename);
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//         return false;

//     QTextStream in(&file);
//     accounts.clear();
//     while (!in.atEnd()) {
//         QString line = in.readLine();
//         QStringList parts = line.split(",");
//         if (parts.size() == 4) {
//             Account acc;
//             acc.name = parts[0];
//             acc.accountNumber = parts[1];
//             acc.password = parts[2];
//             acc.balance = parts[3].toInt();
//             accounts.append(acc);
//         }
//     }
//     file.close();
//     return true;
// }

static const QString DB_FILENAME = "accounts.db";


//────────────────────────────────────────
// DB 기반 함수 구현
//────────────────────────────────────────

// DB에 계좌 정보를 저장하는 함수 (기존 saveAccounts와 동일한 인터페이스)
bool FileStorage::saveAccounts2(const QString& filename, const QVector<Account>& accounts) {
    QString connectionName = QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(filename);

    if (!db.open()) {
        qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
        return false;
    }

    {
        // 블록 스코프에서 QSqlQuery 사용
        {
            QSqlQuery query(db);
            QString createTableQuery = R"(
                CREATE TABLE IF NOT EXISTS accounts (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT,
                    accountNumber TEXT,
                    password TEXT,
                    balance INTEGER
                )
            )";
            if (!query.exec(createTableQuery)) {
                qCritical() << "테이블 생성 실패:" << query.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return false;
            }

            if (!query.exec("DELETE FROM accounts")) {
                qCritical() << "기존 레코드 삭제 실패:" << query.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return false;
            }

            query.prepare("INSERT INTO accounts (name, accountNumber, password, balance) VALUES (?, ?, ?, ?)");
            for (const Account& acc : accounts) {
                query.addBindValue(acc.name);
                query.addBindValue(acc.accountNumber);
                query.addBindValue(acc.password);
                query.addBindValue(acc.balance);
                if (!query.exec()) {
                    qCritical() << "레코드 삽입 실패:" << query.lastError().text();
                    db.close();
                    QSqlDatabase::removeDatabase(connectionName);
                    return false;
                }
            }
        }
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
    return true;
}


// DB에서 계좌 정보를 불러오는 함수 (기존 loadAccounts와 동일한 인터페이스)
bool FileStorage::loadAccounts2(const QString& filename, QVector<Account>& accounts) {
    QString connectionName = QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(filename);

    if (!db.open()) {
        qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
        return false;
    }

    {
        {
            QSqlQuery query(db);
            QString createTableQuery = R"(
                CREATE TABLE IF NOT EXISTS accounts (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT,
                    accountNumber TEXT,
                    password TEXT,
                    balance INTEGER
                )
            )";
            if (!query.exec(createTableQuery)) {
                qCritical() << "테이블 생성 실패:" << query.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return false;
            }

            if (!query.exec("SELECT name, accountNumber, password, balance FROM accounts")) {
                qCritical() << "데이터 조회 실패:" << query.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return false;
            }

            accounts.clear();
            while (query.next()) {
                Account acc;
                acc.name = query.value(0).toString();
                acc.accountNumber = query.value(1).toString();
                acc.password = query.value(2).toString();
                acc.balance = query.value(3).toInt();
                accounts.append(acc);
            }
        }
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
    return true;
}


void FileStorage::testLoadAccountsTable(const QString& dbFilename) {
    QString connectionName = QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(dbFilename);

    if (!db.open()) {
        qDebug() << "데이터베이스 열기 실패:" << db.lastError().text();
        return;
    }

    {
        // 🔽 QSqlQuery 생성 및 사용을 이 블록 안에 국한
        {
            QSqlQuery query(db);

            QString createTableQuery = R"(
                CREATE TABLE IF NOT EXISTS accounts (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT,
                    accountNumber TEXT,
                    password TEXT,
                    balance INTEGER
                )
            )";
            if (!query.exec(createTableQuery)) {
                qDebug() << "테이블 생성 실패:" << query.lastError().text();
                return;
            }

            if (!query.exec("SELECT id, name, accountNumber, password, balance FROM accounts")) {
                qDebug() << "데이터 조회 실패:" << query.lastError().text();
                return;
            }

            qDebug() << "accounts 테이블 전체 데이터:";
            while (query.next()) {
                int id = query.value("id").toInt();
                QString name = query.value("name").toString();
                QString accountNumber = query.value("accountNumber").toString();
                QString password = query.value("password").toString();
                int balance = query.value("balance").toInt();

                qDebug() << "ID:" << id
                         << "이름:" << name
                         << "계좌번호:" << accountNumber
                         << "비밀번호:" << password
                         << "잔액:" << balance;
            }
        } // 🔴 여기서 query 소멸
    }

    db.close(); // 🔴 반드시 닫고
    QSqlDatabase::removeDatabase(connectionName); // 🔴 안전하게 제거 가능
}
void FileStorage::init() {
    QString connectionName = QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(DB_FILENAME);

    if (!db.open()) {
        qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
        return;
    }

    {
        // 1. accounts 테이블 생성
        {
            QSqlQuery query(db);
            QString createTableQuery = R"(
                CREATE TABLE IF NOT EXISTS accounts (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT,
                    accountNumber TEXT,
                    password TEXT,
                    balance INTEGER
                )
            )";
            if (!query.exec(createTableQuery)) {
                qCritical() << "테이블 생성 실패:" << query.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return;
            }
        }

        // 2. 레코드 수 확인
        int recordCount = 0;
        {
            QSqlQuery countQuery(db);
            if (!countQuery.exec("SELECT COUNT(*) FROM accounts")) {
                qCritical() << "레코드 수 확인 실패:" << countQuery.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return;
            }
            if (countQuery.next()) {
                recordCount = countQuery.value(0).toInt();
            }
        }

        // 3. 초기 더미 데이터 삽입
        if (recordCount == 0) {
            QStringList names { "홍길동", "김현석", "김세진", "전준서", "정현구" };
            {
                QSqlQuery insertQuery(db);
                insertQuery.prepare("INSERT INTO accounts (name, accountNumber, password, balance) VALUES (?, ?, ?, ?)");
                for (int i = 0; i < names.size(); ++i) {
                    QString name = names.at(i);
                    QString accNumber = QString::number(i + 1);
                    QString password = QString::number(i + 1);
                    int balance = 100000;

                    insertQuery.addBindValue(name);
                    insertQuery.addBindValue(accNumber);
                    insertQuery.addBindValue(password);
                    insertQuery.addBindValue(balance);

                    if (!insertQuery.exec()) {
                        qCritical() << "초기 데이터 삽입 실패:" << insertQuery.lastError().text();
                        db.close();
                        QSqlDatabase::removeDatabase(connectionName);
                        return;
                    }
                }
            }
        }
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
}



bool FileStorage::updatePassword(const QString& accountNumber, const QString& currentPassword, const QString& newPassword) {
    QString connectionName = QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(DB_FILENAME);

    if (!db.open()) {
        qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
        return false;
    }

    bool success = false;
    {
        {
            QSqlQuery query(db);
            query.prepare("SELECT password FROM accounts WHERE accountNumber = ?");
            query.addBindValue(accountNumber);

            if (!query.exec() || !query.next()) {
                qCritical() << "계좌번호로 현재 비밀번호를 찾을 수 없음:" << query.lastError().text();
            } else {
                QString storedPassword = query.value(0).toString();
                if (storedPassword == currentPassword) {
                    query.prepare("UPDATE accounts SET password = ? WHERE accountNumber = ?");
                    query.addBindValue(newPassword);
                    query.addBindValue(accountNumber);

                    if (!query.exec()) {
                        qCritical() << "비밀번호 변경 실패:" << query.lastError().text();
                    } else {
                        success = true;
                    }
                }
            }
        }
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
    return success;
}

