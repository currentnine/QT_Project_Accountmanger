#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transactionlogger.h"
#include "chatbotwindow.h"
#include "toggleswitch.h"
#include "threadprogressdialog.h"
#include "favoritehelper.h"
#include "favoritetransferdialog.h"
#include "customercenterwidget.h"
#include "thememanager.h"
#include "changepassworddialog.h"
#include <QStyleFactory>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QComboBox>
#include <QTimer>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 기본 타이틀바 제거
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    // 그림자/투명도 깨짐 방지 (선택)
    setAttribute(Qt::WA_TranslucentBackground, false);

    // 커스텀 타이틀바 삽입
    setupCustomTitleBar();


    init();
}


void MainWindow::init(){

    setLoginState(false);  // ✅ setter 사용

    ui->accountLineEdit->setMinimumHeight(30);  // 또는 .setMinimumSize(width, height);
    ui->passwordLineEdit->setMinimumHeight(30);
    ui->accountLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->passwordLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->logoLabel->setPixmap(QPixmap(":/image/logo.jpg").scaledToHeight(80));

    // 열 너비 비율 설정: [label:5] [value:5] [chatbot:2]
    ui->infoRowLayout->setColumnStretch(0, 5);  // 이름/계좌번호/자산 label 열
    ui->infoRowLayout->setColumnStretch(1, 5);  // 값 표시 (nameLabel, numberLabel, balanceLabel)
    ui->infoRowLayout->setColumnStretch(2, 2);  // 챗봇 버튼 열


    qApp->setStyle(QStyleFactory::create("Fusion"));
    qApp->setPalette(style()->standardPalette());
    this->setStyleSheet("");
    ToggleSwitch *themeSwitch = new ToggleSwitch(this);

    QHBoxLayout *topRightLayout = new QHBoxLayout;
    topRightLayout->addStretch();
    topRightLayout->addWidget(themeSwitch);
    ui->verticalLayout->insertLayout(1, topRightLayout);

    ThemeManager::applyLightTheme(this);
    connect(themeSwitch, &ToggleSwitch::toggled, this, [=](bool darkMode) {
        isDarkMode = darkMode;  // ✅ 현재 테마 상태 저장

        if (darkMode) {
            QPixmap inverted = ToggleSwitch::invertPixmap(QPixmap(":/image/logo.jpg"));
            ui->logoLabel->setPixmap(inverted.scaledToHeight(80));
            ThemeManager::applyDarkTheme(this);
        } else {
            ui->logoLabel->setPixmap(QPixmap(":/image/logo.jpg").scaledToHeight(80));
            ThemeManager::applyLightTheme(this);
        }
    });


    // 계좌 데이터 불러오기
    QVector<Account>& accList = manager.getAccounts();
    // FileStorage::loadAccounts("accounts.txt", accList);
    FileStorage::loadAccounts2("accounts.db", accList);
    FileStorage::testLoadAccountsTable("accounts.db");

    initCustomerCenterButtonAnimation();
    FileStorage::init();
    TransactionLogger::init();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_depositButton_clicked()
{
    if (!currentAccount) return;

    int amount = ui->amountLineEdit->text().toInt();
    if (amount <= 0) {
        QMessageBox::warning(this, "입력 오류", "올바른 금액을 입력하세요.");
        return;
    }

    if (manager.deposit(currentAccount, amount)) {
        ui->balanceLabel->setText(QString::number(currentAccount->balance));

        // FileStorage::saveAccounts("accounts.txt", manager.getAccounts());
        FileStorage::saveAccounts2("accounts.db", manager.getAccounts());
        FileStorage::testLoadAccountsTable("accounts.db");

        TransactionLogger::log("입금", currentAccount->accountNumber, amount, currentAccount->balance);

        // ✅ 로딩창 띄우고, 닫힌 뒤 로그 갱신
        ThreadProgressDialog* dlg = new ThreadProgressDialog(this);
        dlg->startTransaction("입금", 2000);

        // ✅ 로그 뷰 갱신
        updateLogView();

        showMessage(QString("<span style='color:#FF6A00;'>입금이 완료되었습니다.</span>"));

    } else{
        showMessage(QString("<span style='color:#FF6A00;'>입금 실패</span>"), QMessageBox::Warning);
    }
}

void MainWindow::on_withdrawButton_clicked()
{
    if (!currentAccount) return;
    int amount = ui->amountLineEdit->text().toInt();

    if (manager.withdraw(currentAccount, amount)) {
        ui->balanceLabel->setText(QString::number(currentAccount->balance));
        // FileStorage::saveAccounts("accounts.txt", manager.getAccounts());
        FileStorage::saveAccounts2("accounts.db", manager.getAccounts());
        FileStorage::testLoadAccountsTable("accounts.db");

        TransactionLogger::log("출금", currentAccount->accountNumber, amount, currentAccount->balance);

        updateLogView();
        showProgressDialog("출금", 2000);


        showMessage(QString("<span style='color:#FF6A00;'>출금 완료되었습니다.</span>"));

} else{
        showMessage(QString("<span style='color:#FF6A00;'>출금 실패</span>"), QMessageBox::Warning);
}

}

void MainWindow::on_loginButton_clicked()
{

        if (!getLoginState()) {
            // 로그인 시도
            QString accNum = ui->accountLineEdit->text();
            QString pw = ui->passwordLineEdit->text();

            currentAccount = manager.login(accNum, pw);

            if (currentAccount) {
                setLoginState(true);  // 내부 상태 변경

                showMessage(QString("<span style='color:#FF6A00;'>로그인 성공!</span>"));

                // 로그인 성공 후 메인 페이지로 전환 (stackedWidget의 index 1)
                ui->stackedWidget->setCurrentIndex(1);

                // 메인 페이지의 계좌 정보 업데이트
                ui->nameLabel->setText(currentAccount->name);
                ui->numberLabel->setText(currentAccount->accountNumber);
                ui->balanceLabel->setText(QString::number(currentAccount->balance));
            } else {

                showMessage(QString("<span style='color:#FF6A00;'>계좌번호 또는 비밀번호가 잘못되었습니다.</span>"),QMessageBox::Warning);


            }
        }
        else {

            showMessage(QString("<span style='color:#FF6A00;'>로그아웃 되었습니다.</span>"));



            currentAccount = nullptr;
            setLoginState(false);

            // 로그아웃 시 로그인 페이지로 전환 (index 0)
            ui->stackedWidget->setCurrentIndex(0);

            // 메인 페이지/검색 페이지 정보 초기화
            ui->nameLabel->clear();
            ui->numberLabel->clear();
            ui->balanceLabel->clear();
            ui->logListWidget->clear();
        }

}

void MainWindow::on_transferButton_clicked()
{
    if (!currentAccount) return;

    // ✅ 타이틀바 없는 송금 방식 선택 다이얼로그 생성
    QStringList options = { "직접 입력", "즐겨찾기에서 선택" };
    QString selectedMethod;

    QDialog methodDialog(this);
    methodDialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    methodDialog.setFixedSize(300, 150);

    QVBoxLayout* layout = new QVBoxLayout(&methodDialog);

    QLabel* label = new QLabel("<span style='color:#FF6A00; font-weight:bold;'>송금할 방법:</span>");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QComboBox* combo = new QComboBox(&methodDialog);
    combo->addItems(options);
    layout->addWidget(combo);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &methodDialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &methodDialog, &QDialog::reject);

    if (methodDialog.exec() == QDialog::Accepted) {
        selectedMethod = combo->currentText();
    } else {
        return;  // 사용자가 취소함
    }

    // ✅ 상대 계좌 입력 처리
    QString toAccount;
    if (selectedMethod == "직접 입력") {
        // toAccount = QInputDialog::getText(this, "송금", "상대 계좌번호:");
        bool ok = false;

        toAccount = getAccountNumberInput(&ok);

    } else {
        FavoriteTransferDialog dialog(currentAccount->accountNumber, this);
        if (dialog.exec() == QDialog::Accepted) {
            toAccount = dialog.getSelectedAccount();
        } else {
            return;
        }
    }

    int amount = ui->amountLineEdit->text().toInt();
    Account* target = manager.findByAccountNumber(toAccount);

    if (!target) {
        showMessage(QString("<span style='color:#FF6A00;'>상대 계좌를 찾을 수 없습니다.</span>"),
                    QMessageBox::Warning);
        return;
    }

    if (manager.transfer(currentAccount, target, amount)) {
        ui->balanceLabel->setText(QString::number(currentAccount->balance));
        // FileStorage::saveAccounts("accounts.txt", manager.getAccounts());
        FileStorage::saveAccounts2("accounts.db", manager.getAccounts());

        TransactionLogger::logTransfer(currentAccount->accountNumber, target->accountNumber,
                                       amount, currentAccount->balance);
        TransactionLogger::log("입금", target->accountNumber, amount, target->balance);

        showProgressDialog("송금", 2500);
        updateLogView();

        showMessage(QString("<span style='color:#FF6A00;'>송금이 완료되었습니다.</span>"));
    } else {
        showMessage(QString("<span style='color:#FF6A00;'>송금 실패</span>"), QMessageBox::Warning);
    }
}

QString MainWindow::getAccountNumberInput(bool* ok)
{
    *ok = false;
    QString inputText;

    QDialog dialog(this);
    dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dialog.setFixedSize(300, 150);

    QLabel* label = new QLabel("<span style='color:#FF6A00; font-weight:bold;'>상대 계좌번호를 입력하세요:</span>", &dialog);
    label->setAlignment(Qt::AlignCenter);

    QLineEdit* lineEdit = new QLineEdit(&dialog);
    lineEdit->setPlaceholderText("예: 12345678");

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(buttonBox);
    dialog.setLayout(layout);

    if (dialog.exec() == QDialog::Accepted) {
        inputText = lineEdit->text().trimmed();
        *ok = true;
    }

    return inputText;
}

void MainWindow::on_searchButton_clicked()
{
    if (!currentAccount) return;
    QString keyword = ui->searchLineEdit->text().trimmed();
    QString userName = currentAccount->name;
    QMap<QString, QString> map = manager.createAccountNameMap();
    QStringList filteredLogs = TransactionLogger::loadLogsByName(userName, map, keyword);
    qDebug() << filteredLogs;
    ui->logListWidget->clear();
    ui->logListWidget->addItems(filteredLogs);
}


void MainWindow::showProgressDialog(const QString& type, int delay)
{

    ThreadProgressDialog* dlg = new ThreadProgressDialog(this);
    dlg->startTransaction(type, 2000);
}

void MainWindow::updateLogView()
{
    if (!currentAccount) return;
    QString userName = currentAccount->name;
    QMap<QString, QString> nameMap = manager.createAccountNameMap();
    QStringList logs = TransactionLogger::loadLogsByName(userName, nameMap);

    ui->logListWidget->clear();
    ui->logListWidget->addItems(logs);
}


bool MainWindow::getLoginState() const {
    return isLoggedIn;
}

void MainWindow::setLoginState(bool state) {
    isLoggedIn = state;
    setButtonStates();  // 로그인 상태가 바뀔 때마다 UI 상태 자동 반영
}


void MainWindow::setButtonStates()
{
    bool enabled = getLoginState();  // 내부 상태를 기준으로 판단



    // 거래 기능 버튼들
    ui->depositButton->setEnabled(enabled);
    ui->withdrawButton->setEnabled(enabled);
    ui->transferButton->setEnabled(enabled);
    ui->searchButton->setEnabled(enabled);
    ui->favoriteButton->setEnabled(enabled);

    // 로그인 입력창
    ui->accountLineEdit->setEnabled(!enabled);
    ui->passwordLineEdit->setEnabled(!enabled);

    // 로그인 상태에 따른 UI 전환
    if (enabled) {
        // 로그인 성공 시: 버튼 → 로그아웃, 입력창 초기화
        ui->loginButton->setText("로그아웃");
        ui->accountLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->accountLineEdit->setPlaceholderText("");
        ui->passwordLineEdit->setPlaceholderText("");
        ui->registerButton->hide();
        // 개인 정보 상태 업데이트
        ui->nameLabel->setText(currentAccount->name);
        ui->numberLabel->setText(currentAccount->accountNumber);
        ui->balanceLabel->setText(QString::number(currentAccount->balance));
        ui->stackedWidget->setCurrentIndex(1);
        ui->changePasswordButton->show();


    } else {
        // 개인 정보 상태 초기화
        ui->nameLabel->clear();
        ui->numberLabel->clear();
        ui->balanceLabel->clear();
        ui->logListWidget->clear();

        // 로그아웃 시: 버튼 → 로그인, 입력창 복원
        ui->loginButton->setText("로그인");
        ui->accountLineEdit->setPlaceholderText("계좌번호 입력");
        ui->passwordLineEdit->setPlaceholderText("비밀번호 입력");
        ui->registerButton->show();
        ui->favoriteButton->show();
        ui->changePasswordButton->hide();
        ui->stackedWidget->setCurrentIndex(0);

    }
}

void MainWindow::on_registerButton_clicked()
{
    RegisterDialog dialog(&manager, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 창은 자동 종료됨
    }
}


void MainWindow::on_favoriteButton_clicked()
{
    if (!currentAccount) return;

    QList<QListWidgetItem*> selected = ui->logListWidget->selectedItems();
    if (selected.isEmpty()) {

        showMessage(QString("<span style='color:#FF6A00;'>즐겨찾기에 추가할 항목을 선택하세요.</span>"),QMessageBox::Warning);

        return;
    }

    for (QListWidgetItem* item : selected) {
        QString logText = item->text();
        qDebug() << "🧾 로그 텍스트:" << logText;

        // ✅ 송금 로그가 아닌 경우 무시
        if (!logText.contains("송금")) {
            qDebug() << "⛔ 송금 로그가 아님, 건너뜀!";
            continue;
        }

        // ✅ 계좌번호 추출: "계좌: 12345678" 형태
        QRegularExpression regex(R"(계좌:\s*(\d+))");
        QRegularExpressionMatch match = regex.match(logText);

        if (match.hasMatch()) {
            QString toAcc = match.captured(1).trimmed();

            // ✅ 자기 자신은 추가하지 않음
            if (toAcc == currentAccount->accountNumber) {
                qDebug() << "⛔ 자기 자신의 계좌, 추가 생략!";

                showMessage(QString("<span style='color:#FF6A00;'>자신의 계좌는 즐겨찾기에 추가할 수 없습니다.</span>"),QMessageBox::Warning);

                continue;
            }

            qDebug() << "✅ 추출된 계좌번호:" << toAcc;
            FavoriteHelper::addFavorite(currentAccount->accountNumber, toAcc);


            showMessage(QString("<span style='color:#FF6A00;'>즐겨찾기 추가</span>"));



        } else {
            qDebug() << "❌ '계좌:' 키워드가 로그에 없음!";
        }
    }
}


void MainWindow::loadFavoritesList()
{
    // TODO: 나중에 즐겨찾기 리스트를 불러오는 기능 구현
    qDebug() << "즐겨찾기 목록 불러오기 (임시)";
}


void MainWindow::initCustomerCenterButtonAnimation()
{
    // 버튼을 레이아웃 관리 대상에서 제거하여 직접 위치 조절이 가능하도록 함.
    // 버튼의 부모를 중앙 위젯(ui->centralwidget)으로 변경하고, (레이아웃 상의 다른 위젯보다)
    // 화면의 맨 앞에 오도록 raise() 호출.
    ui->customerCenterButton->setParent(ui->centralwidget);
    ui->customerCenterButton->raise();

    // 레이아웃 및 위젯 배치가 완료된 후에 위치 계산을 위해 QTimer::singleShot 사용
    QTimer::singleShot(0, this, [this]() {
        // 상위 위젯의 영역을 가져옵니다.
        QRect parentRect = ui->centralwidget->contentsRect();
        QSize btnSize = ui->customerCenterButton->size();

        // 오른쪽 하단 배치: 여백 값 조정 가능
        const int marginRight = 20;
        const int marginBottom = 20;
        QPoint newPos(parentRect.width() - btnSize.width() - marginRight,
                      parentRect.height() - btnSize.height() - marginBottom);

        // 고객센터 버튼을 계산된 위치로 이동
        ui->customerCenterButton->move(newPos);
        qDebug() << "CustomerCenterButton new position:" << ui->customerCenterButton->pos();

        // 애니메이션 시작: 지정한 위치(startPos)를 기준으로 수직 애니메이션 효과 적용
        const QPoint startPos = ui->customerCenterButton->pos();
        const int animOffset = 10;    // 위아래 이동할 픽셀 수
        const int durationMs = 2000;  // 애니메이션 시간 (밀리초)

        QPropertyAnimation *animation = new QPropertyAnimation(ui->customerCenterButton, "pos", this);
        animation->setDuration(durationMs);
        animation->setStartValue(startPos);
        animation->setKeyValueAt(0.5, QPoint(startPos.x(), startPos.y() + animOffset));
        animation->setEndValue(startPos);
        animation->setEasingCurve(QEasingCurve::InOutSine);
        animation->setLoopCount(-1);  // 무한 반복
        animation->start();
    });
}


void MainWindow::on_customerCenterButton_clicked()
{
    // 이미 열려 있는 경우 → 앞으로 bring
    if (customerCenter && customerCenter->isVisible()) {
        customerCenter->raise();
        customerCenter->activateWindow();
        return;
    }

    // 새로 생성 (닫힌 상태였거나 nullptr인 경우)
    customerCenter = new CustomerCenterWidget(this);
    customerCenter->setAttribute(Qt::WA_DeleteOnClose);
    customerCenter->show();

    // 닫힐 때 customerCenter 포인터를 nullptr로 초기화
    connect(customerCenter, &QObject::destroyed, this, [=]() {
        customerCenter = nullptr;
    });
}


void MainWindow::on_searchPageButton_clicked()
{
    // 메인 페이지의 "거래내역 검색" 버튼 클릭 시 검색 페이지로 전환
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_backButton_clicked()
{
    // 검색 페이지의 "뒤로가기" 버튼 클릭 시 메인 페이지로 전환
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_logoutButton_clicked()
{
    // 로그아웃 처리: 사용자 계정 초기화 및 로그인 상태 변경
    showMessage(QString("<span style='color:#FF6A00;'>로그아웃 되었습니다.</span>"));

    currentAccount = nullptr;
    setLoginState(false);

    // stackedWidget의 페이지를 로그인 페이지로 전환 (index 0)
    ui->stackedWidget->setCurrentIndex(0);

    // 메인 페이지 및 검색 페이지의 정보 초기화 (필요시)
    ui->nameLabel->clear();
    ui->numberLabel->clear();
    ui->balanceLabel->clear();
    ui->logListWidget->clear();
}



bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_dragWidget || watched == m_dragWidget_2 ) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_dragPos = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
                return true;
            }
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                move(mouseEvent->globalPosition().toPoint() - m_dragPos);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}


void MainWindow::setupCustomTitleBar() {
    QWidget *titleBar = new QWidget(this);
    titleBar->setObjectName("customTitleBar");
    titleBar->setFixedHeight(40);

    QLabel *titleLabel = new QLabel(this);
    titleLabel->setText("<img src=\":/image/hanhwa.png\" width=\"20\" height=\"20\">"
                        "<span style='color: #FF6A00;'> 한화투자증권</span>");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QPushButton *minimizeBtn = new QPushButton("-");
    QPushButton *closeBtn = new QPushButton("×");

    minimizeBtn->setFixedSize(30, 30);
    closeBtn->setFixedSize(30, 30);

    minimizeBtn->setObjectName("minimizeButton");
    closeBtn->setObjectName("closeButton");

    connect(minimizeBtn, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::close);

    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(7, 0, 7, 0);

    layout->addWidget(titleLabel);
    layout->addWidget(minimizeBtn);
    layout->addWidget(closeBtn);

    // 타이틀바를 메인 UI 최상단에 삽입
    ui->verticalLayout->insertWidget(0, titleBar);

    // 드래그 이동을 위한 이벤트 처리
    titleBar->installEventFilter(this);
    titleLabel->installEventFilter(this);  // ✅ 추가

    m_dragWidget = titleBar;
    m_dragWidget_2 = titleLabel;
}







void MainWindow::on_pageChanged(int index) {
    if (index == 0) {
        // 첫 번째 페이지에 라이트 테마 적용
        ThemeManager::applyLightTheme(this);
    } else if (index == 1) {
        // 두 번째 페이지에 다크 테마 적용
        ThemeManager::applyDarkTheme(this);
    }
}




void MainWindow::showMessage(const QString& htmlText, QMessageBox::Icon icon)
{
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText(htmlText);
    msgBox->setIcon(icon);
    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    if (isDarkMode) {
        // ✅ 다크 모드용 스타일
        msgBox->setStyleSheet(R"(
            QLabel {
                color: #FF6A00;
                font-size: 14px;
                font-weight: bold;
            }
            QMessageBox {
                background-color: #2c2c2c;
                border: 2px solid #FF6A00;
                border-radius: 8px;
            }
        )");
    } else {
        // ✅ 라이트 모드용 스타일
        msgBox->setStyleSheet(R"(
            QLabel {
                color: #FF6A00;
                font-size: 14px;
                font-weight: bold;
            }
            QMessageBox {
                background-color: white;
                border: 2px solid #FF6A00;
                border-radius: 8px;
            }
        )");
    }

    msgBox->exec();
}


void MainWindow::on_changePasswordButton_clicked()
{
    if (!currentAccount) return;

    ChangePasswordDialog dialog(currentAccount, &manager, this);
    dialog.exec();  // 메시지는 내부에서 처리됨
}


