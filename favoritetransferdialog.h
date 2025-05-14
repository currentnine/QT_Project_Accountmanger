// FavoriteTransferDialog.h
#ifndef FAVORITETRANSFERDIALOG_H
#define FAVORITETRANSFERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>

class FavoriteTransferDialog : public QDialog {
    Q_OBJECT

public:
    explicit FavoriteTransferDialog(const QString& myAccount, QWidget *parent = nullptr);
    // QString selectedAccount() const;
    QString getSelectedAccount() const;

private slots:
    void removeSelected();
    void acceptSelection();

private:
    QString myAccount;
    QListWidget* listWidget;
    QPushButton* removeButton;
    QPushButton* sendButton;
    QString selectedAccount;

protected:
    void keyPressEvent(QKeyEvent* event) override;

};

#endif // FAVORITETRANSFERDIALOG_H
