#include "transactionlogger.h"

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlError>
#include <QDebug>
#include <QUuid>

// 데이터베이스 파일 이름 (transactions.db)
static const QString DB_FILENAME = "transactions.db";

//-----------------------------------------------------------------------------
// Helper 함수: transactions 테이블이 존재하지 않으면 생성
//-----------------------------------------------------------------------------
static bool ensureTransactionTable(QSqlDatabase &db) {
    QSqlQuery query(db);
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT,
            type TEXT,
            accountFrom TEXT,
            accountTo TEXT,
            amount INTEGER,
            balance INTEGER
        )
    )";
    if (!query.exec(createTableQuery)) {
        qCritical() << "테이블 생성 실패:" << query.lastError().text();
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// 초기화 함수: 데이터베이스에 transactions 테이블이 없으면 생성 후 예시 데이터를 3개 삽입
//-----------------------------------------------------------------------------
void TransactionLogger::init() {
    QString connectionName = QUuid::createUuid().toString();
    {
        // QSqlDatabase 객체를 생성한 후 해당 블록 내에서 사용합니다.
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(DB_FILENAME);

        if (!db.open()) {
            qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
            return;
        }

        // transactions 테이블이 존재하지 않으면 생성
        if (!ensureTransactionTable(db)) {
            db.close();
            return;
        }

        {
            // 테이블의 레코드 수 확인 (별도의 블록)
            QSqlQuery countQuery(db);
            if (!countQuery.exec("SELECT COUNT(*) FROM transactions")) {
                qCritical() << "테이블 카운트 조회 실패:" << countQuery.lastError().text();
                db.close();
                return;
            }
            countQuery.next();
            int recordCount = countQuery.value(0).toInt();

            // 테이블이 비어있다면(레코드 수 0) 예시 로그 10행 삽입:
            // 각 계좌(총 5개)에 대해 입금과 출금 로그를 각각 기록 (5 * 2 = 10행)
            if (recordCount == 0) {
                // 계좌 정보 (예시 계좌번호)
                QStringList accountNumbers = {"1", "2", "3", "4", "5"};
                // 각 계좌별로 입금 및 출금 로그 기록
                for (int i = 0; i < accountNumbers.size(); ++i) {
                    QString accNum = accountNumbers.at(i);

                    // 1. 입금 로그: 500원이 입금되어 잔액이 100000 + 500 = 100500
                    {
                        QSqlQuery depositQuery(db);
                        depositQuery.prepare("INSERT INTO transactions (timestamp, type, accountFrom, accountTo, amount, balance) "
                                             "VALUES (?, ?, ?, ?, ?, ?)");
                        QString timeDeposit = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                        depositQuery.addBindValue(timeDeposit);
                        depositQuery.addBindValue("입금");
                        depositQuery.addBindValue(accNum);   // accountFrom에 계좌번호 저장
                        depositQuery.addBindValue("");        // accountTo는 사용하지 않음
                        int depositAmount = 500;
                        int depositBalance = 100000 + depositAmount;  // 100500
                        depositQuery.addBindValue(depositAmount);
                        depositQuery.addBindValue(depositBalance);

                        if (!depositQuery.exec()) {
                            qCritical() << "예시 입금 로그 삽입 실패:" << depositQuery.lastError().text();
                            db.close();
                            return;
                        }
                    }

                    // 2. 출금 로그: 500원 출금하여 잔액이 100500 - 500 = 100000
                    {
                        QSqlQuery withdrawalQuery(db);
                        withdrawalQuery.prepare("INSERT INTO transactions (timestamp, type, accountFrom, accountTo, amount, balance) "
                                                "VALUES (?, ?, ?, ?, ?, ?)");
                        QString timeWithdrawal = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                        withdrawalQuery.addBindValue(timeWithdrawal);
                        withdrawalQuery.addBindValue("출금");
                        withdrawalQuery.addBindValue(accNum);   // accountFrom에 계좌번호 저장
                        withdrawalQuery.addBindValue("");
                        int withdrawalAmount = 500;
                        int withdrawalBalance = (100000 + 500) - withdrawalAmount;  // 100000
                        withdrawalQuery.addBindValue(withdrawalAmount);
                        withdrawalQuery.addBindValue(withdrawalBalance);

                        if (!withdrawalQuery.exec()) {
                            qCritical() << "예시 출금 로그 삽입 실패:" << withdrawalQuery.lastError().text();
                            db.close();
                            return;
                        }
                    }
                } // for each account
            }
        }  // countQuery 및 for 루프 관련 모든 QSqlQuery 객체가 이 시점에 소멸됨

        db.close();  // 데이터베이스 명시적 종료
    }  // QSqlDatabase 객체(db)도 이 블록이 종료되면서 소멸됨

    // 이제 안전하게 연결 제거
    QSqlDatabase::removeDatabase(connectionName);
}


//-----------------------------------------------------------------------------
// 입금/출금 로그 기록 함수 (accountTo는 빈 문자열)
//-----------------------------------------------------------------------------
void TransactionLogger::log(const QString& type, const QString& accountNumber, int amount, int balance) {
    QString connectionName = QUuid::createUuid().toString();
    {  // 내부 블록: db 및 사용된 QSqlQuery 객체가 모두 소멸됨
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(DB_FILENAME);

        if (!db.open()) {
            qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
            return;
        }
        if (!ensureTransactionTable(db)) {
            db.close();
            return;
        }
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        {
            QSqlQuery query(db);
            query.prepare("INSERT INTO transactions (timestamp, type, accountFrom, accountTo, amount, balance) "
                          "VALUES (?, ?, ?, ?, ?, ?)");
            query.addBindValue(time);
            query.addBindValue(type);
            query.addBindValue(accountNumber);
            query.addBindValue("");  // accountTo는 사용하지 않음 (입금/출금)
            query.addBindValue(amount);
            query.addBindValue(balance);

            if (!query.exec()) {
                qCritical() << "로그 삽입 실패:" << query.lastError().text();
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
}

//-----------------------------------------------------------------------------
// 송금 로그 기록 함수
//-----------------------------------------------------------------------------
void TransactionLogger::logTransfer(const QString& from, const QString& to, int amount, int fromBalance) {
    QString connectionName = QUuid::createUuid().toString();
    {  // 내부 블록
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(DB_FILENAME);

        if (!db.open()) {
            qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
            return;
        }
        if (!ensureTransactionTable(db)) {
            db.close();
            return;
        }
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        {
            QSqlQuery query(db);
            query.prepare("INSERT INTO transactions (timestamp, type, accountFrom, accountTo, amount, balance) "
                          "VALUES (?, ?, ?, ?, ?, ?)");
            query.addBindValue(time);
            query.addBindValue("송금");
            query.addBindValue(from);
            query.addBindValue(to);
            query.addBindValue(amount);
            query.addBindValue(fromBalance);

            if (!query.exec()) {
                qCritical() << "송금 로그 삽입 실패:" << query.lastError().text();
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
}

//-----------------------------------------------------------------------------
// 전체 로그를 조회하여 CSV 형식의 문자열 리스트로 반환
//-----------------------------------------------------------------------------
QStringList TransactionLogger::loadLogs(const QString& search) {
    QStringList logs;
    QString connectionName = QUuid::createUuid().toString();
    {  // 내부 블록
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(DB_FILENAME);

        if (!db.open()) {
            qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
            return logs;
        }
        if (!ensureTransactionTable(db)) {
            db.close();
            return logs;
        }
        {
            QSqlQuery query(db);
            if (!query.exec("SELECT timestamp, type, accountFrom, accountTo, amount, balance FROM transactions ORDER BY id ASC")) {
                qCritical() << "로그 데이터 조회 실패:" << query.lastError().text();
                db.close();
                return logs;
            }

            while (query.next()) {
                QString timestamp = query.value("timestamp").toString();
                QString type = query.value("type").toString();
                QString accountFrom = query.value("accountFrom").toString();
                QString accountTo = query.value("accountTo").toString();
                int amount = query.value("amount").toInt();
                int balance = query.value("balance").toInt();

                QString line;
                if (type == "송금" && !accountTo.isEmpty()) {
                    line = QString("%1,%2,%3,%4,%5,%6")
                    .arg(timestamp)
                        .arg(type)
                        .arg(accountFrom)
                        .arg(accountTo)
                        .arg(amount)
                        .arg(balance);
                } else {
                    line = QString("%1,%2,%3,%4,%5")
                    .arg(timestamp)
                        .arg(type)
                        .arg(accountFrom)
                        .arg(amount)
                        .arg(balance);
                }
                if (search.isEmpty() || line.contains(search, Qt::CaseInsensitive))
                    logs << line;
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
    return logs;
}

//-----------------------------------------------------------------------------
// 사용자 이름과 계좌맵을 기준으로 로그를 포맷하여 필터링한 문자열 리스트 반환
// (송금 로그와 입금/출금 로그를 각각 처리)
//-----------------------------------------------------------------------------
QStringList TransactionLogger::loadLogsByName(const QString& userName,
                                              const QMap<QString, QString>& accountNameMap,
                                              const QString& keyword)
{
    QStringList result;
    QString connectionName = QUuid::createUuid().toString();
    {  // 내부 블록
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(DB_FILENAME);

        if (!db.open()) {
            qCritical() << "데이터베이스 열기 실패:" << db.lastError().text();
            return result;
        }
        if (!ensureTransactionTable(db)) {
            db.close();
            return result;
        }
        {
            QSqlQuery query(db);
            if (!query.exec("SELECT timestamp, type, accountFrom, accountTo, amount, balance FROM transactions ORDER BY id ASC")) {
                qCritical() << "로그 데이터 조회 실패:" << query.lastError().text();
                db.close();
                return result;
            }

            while (query.next()) {
                QString timestamp = query.value("timestamp").toString();
                QString type = query.value("type").toString();
                QString accountFrom = query.value("accountFrom").toString();
                QString accountTo = query.value("accountTo").toString();
                QString amount = QString::number(query.value("amount").toInt());
                QString balance = QString::number(query.value("balance").toInt());

                if (type == "송금" && !accountTo.isEmpty()) {
                    QString fromName = accountNameMap.value(accountFrom, accountFrom);
                    QString toName = accountNameMap.value(accountTo, accountTo);

                    if (fromName == userName) {
                        QString formatted = QString("[%1] %2님에게 송금 | 금액: %3 | 잔액: %4 | 계좌: %5")
                                                .arg(timestamp, toName, amount, balance, accountTo);
                        if (keyword.isEmpty() || formatted.contains(keyword, Qt::CaseInsensitive))
                            result.append(formatted);
                    } else if (toName == userName) {
                        QString formatted = QString("[%1] %2님에게 송금 | 금액: %3 | 잔액: %4 | 계좌: %5")
                                                .arg(timestamp, toName, amount, balance, accountTo);
                        if (keyword.isEmpty() || formatted.contains(keyword, Qt::CaseInsensitive))
                            result.append(formatted);
                    }
                } else if (accountNameMap.value(accountFrom) == userName) {
                    QString formatted = QString("[%1] %2 | 금액: %3 | 잔액: %4")
                                            .arg(timestamp, type, amount, balance);
                    if (keyword.isEmpty() || formatted.contains(keyword, Qt::CaseInsensitive))
                        result.append(formatted);
                }
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
    return result;
}
