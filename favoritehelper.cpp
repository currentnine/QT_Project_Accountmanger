#include "favoritehelper.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

static QString favoriteFile = "favorites.txt";

void FavoriteHelper::addFavorite(const QString& userAccount, const QString& favoriteAccount) {
    QFile file(favoriteFile);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << userAccount << "," << favoriteAccount << "\n";
    file.close();
}

QSet<QString> FavoriteHelper::loadFavorites(const QString& userAccount) {
    QSet<QString> favorites;
    QFile file("favorites.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return favorites;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.size() != 2) continue;

        QString from = parts[0].trimmed();
        QString to = parts[1].trimmed();
        if (from == userAccount)
            favorites.insert(to);
    }
    file.close();
    return favorites;
}
bool FavoriteHelper::removeFavorite(const QString& userAccount, const QString& toAccount) {
    QFile file("favorites.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.size() != 2) continue;

        QString from = parts[0].trimmed();
        QString to = parts[1].trimmed();

        if (!(from == userAccount && to == toAccount)) {
            lines << line;  // 유지할 항목만 다시 저장
        }
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;

    QTextStream out(&file);
    for (const QString& line : lines)
        out << line << "\n";

    file.close();
    return true;
}
