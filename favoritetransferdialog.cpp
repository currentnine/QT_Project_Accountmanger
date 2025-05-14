// FavoriteTransferDialog.cpp
#include "favoritetransferdialog.h"
#include "favoritehelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QKeyEvent>  // 추가 필요

FavoriteTransferDialog::FavoriteTransferDialog(const QString& myAcc, QWidget *parent)
    : QDialog(parent), myAccount(myAcc)
{
    // setWindowTitle("즐겨찾기 송금");

    // ✅ 타이틀바 제거
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setFixedSize(300, 350);

    // ✅ 상단 타이틀 UI로 처리
    QLabel* titleLabel = new QLabel("<span style='color:#FF6A00; font-size:16px; font-weight:bold;'>즐겨찾기 송금</span>", this);
    titleLabel->setAlignment(Qt::AlignCenter);

    // 리스트
    listWidget = new QListWidget(this);
    listWidget->setStyleSheet("QListWidget { padding: 5px; font-size: 13px; }");

    QSet<QString> favorites = FavoriteHelper::loadFavorites(myAccount);
    for (const QString& acc : favorites)
        listWidget->addItem(acc);

    // 버튼
    sendButton = new QPushButton("송금", this);
    removeButton = new QPushButton("삭제", this);

    sendButton->setStyleSheet("QPushButton { background-color: #FF6A00; color: white; font-weight: bold; }");
    removeButton->setStyleSheet("QPushButton { background-color: #ccc; font-weight: bold; }");

    connect(removeButton, &QPushButton::clicked, this, &FavoriteTransferDialog::removeSelected);
    connect(sendButton, &QPushButton::clicked, this, &FavoriteTransferDialog::acceptSelection);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(removeButton);
    btnLayout->addWidget(sendButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(listWidget);
    layout->addLayout(btnLayout);

    setLayout(layout);

}


void FavoriteTransferDialog::removeSelected() {


    QListWidgetItem* item = listWidget->currentItem();
    if (!item) return;

    QString toAcc = item->text().trimmed();
    FavoriteHelper::removeFavorite(myAccount, toAcc);
    delete item;

    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText("<span style='color:#FF6A00;'>즐겨찾기에서 삭제되었습니다.</span>");
    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->exec();
}

void FavoriteTransferDialog::acceptSelection() {
    QListWidgetItem* item = listWidget->currentItem();
    if (!item) {
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setText("<span style='color:#FF6A00;'>송금할 계좌를 선택하세요.</span>");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        msgBox->exec();

        return;
    }

    selectedAccount = item->text().trimmed();  // ✅ 여기서 저장
    accept();
}

QString FavoriteTransferDialog::getSelectedAccount() const {
    return selectedAccount;  // 선택된 계좌를 반환
}

void FavoriteTransferDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();  // 또는 close();
    } else {
        QDialog::keyPressEvent(event);  // 기본 동작 유지
    }
}
