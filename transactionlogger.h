#ifndef TRANSACTIONLOGGER_H
#define TRANSACTIONLOGGER_H

#include <QString>
#include <QStringList>
#include <QMap>

class TransactionLogger
{
public:
    // 데이터베이스 초기화
    // transactions.db 파일에 transactions 테이블이 없으면 생성하고 3개의 예시 로그 데이터를 삽입한다.
    static void init();

    // 입금/출금 로그를 기록 (accountTo는 빈 문자열로 저장)
    static void log(const QString& type, const QString& accountNumber, int amount, int balance);

    // 송금 로그 기록 (accountFrom, accountTo 모두 저장)
    static void logTransfer(const QString& from, const QString& to, int amount, int fromBalance);

    // 전체 로그를 조회 (검색어가 있을 경우 해당 문자열이 포함된 로그만 반환)
    static QStringList loadLogs(const QString& search = QString());

    // 사용자 이름 및 계좌맵을 기준으로 로그를 필터링하여 포맷된 문자열 리스트를 반환
    static QStringList loadLogsByName(const QString& userName,
                                      const QMap<QString, QString>& accountNameMap,
                                      const QString& keyword = QString());



};

#endif // TRANSACTIONLOGGER_H
