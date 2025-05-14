// thememanager.h
#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QWidget>

class ThemeManager {
public:
    static void applyLightTheme(QWidget* target);
    static void applyDarkTheme(QWidget* target);
};

#endif // THEMEMANAGER_H
