#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "accountmanager.h"
namespace Ui {
class RegisterDialog;
}
class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(AccountManager* manager, QWidget *parent = nullptr);
    ~RegisterDialog();
protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onRegisterClicked();
private:
    Ui::RegisterDialog *ui;
    AccountManager* m_manager;  // 외부에서 받아온 계좌 관리자

    void showWarning(const QString& html);

    QLineEdit* nameEdit;
    QLineEdit* accountEdit;
    QLineEdit* passwordEdit;
    QLineEdit* confirmEdit;
    QPushButton* registerButton;
};
#endif // REGISTERDIALOG_H
