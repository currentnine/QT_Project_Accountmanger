// thememanager.cpp
#include "thememanager.h"
#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QPalette>
#include <QStyle>
#include <QMouseEvent>

void ThemeManager::applyLightTheme(QWidget* target) {
    qApp->setPalette(target->style()->standardPalette());

    QFile file(":/qss/hanwha_light.qss");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QString::fromUtf8(file.readAll());
        qApp->setStyleSheet(qss);  // 전체 앱에 적용
        file.close();
    }
}


void ThemeManager::applyDarkTheme(QWidget* target) {
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::AlternateBase, QColor(50, 50, 50));
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Highlight, QColor(255, 100, 0));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);

    QFile file(":/qss/hanwha_dark.qss");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QString::fromUtf8(file.readAll());
        qApp->setStyleSheet(qss);  // ✅ 전역 적용
        file.close();
    }
}


