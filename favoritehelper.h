#ifndef FAVORITEHELPER_H
#define FAVORITEHELPER_H

#include <QString>
#include <QStringList>
#include <QMap>

class FavoriteHelper {
public:
    static void addFavorite(const QString& userAccount, const QString& favoriteAccount);
    static QSet<QString> loadFavorites(const QString& userAccount);
    static QStringList loadFavoritesList(const QString& userAccount);
    // FavoriteHelper.h
    static bool removeFavorite(const QString& userAccount, const QString& toAccount);

};

#endif // FAVORITEHELPER_H
