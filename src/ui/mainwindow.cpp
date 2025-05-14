#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wallet/WalletManager.hpp"

#include <QApplication>
#include <QClipboard>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    ui->statusbar->setStyleSheet(
        "QStatusBar { "
          "background-color: #222222;"
          "color: #cccccc;"
          "font-family: Monospace;"
          "font-size: 12px;"
          "padding: 4px;"
        "}"
    );

    // — Home page —
    connect(ui->pushButton,   &QPushButton::clicked, this, &MainWindow::openCreatePage);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::openExistingPage);

    // — Create page —
    connect(ui->copyButton,       &QPushButton::clicked, this, &MainWindow::copyMnemonic);
    connect(ui->openWalletButton, &QPushButton::clicked, this, &MainWindow::openWithGenerated);
    connect(ui->backButtonCreate, &QPushButton::clicked, this, &MainWindow::openHomePage);

    // — Existing page —
    connect(ui->openExistingButton, &QPushButton::clicked, this, &MainWindow::openExisting);
    connect(ui->backButtonExisting, &QPushButton::clicked, this, &MainWindow::openHomePage);

    // — Balance page —
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
    connect(ui->receiveButton, &QPushButton::clicked, this, &MainWindow::openReceivePage);

    connect(ui->coinList, &QListWidget::itemClicked, this, &MainWindow::onCoinSelected);

    connect(ui->copyAddressButton, &QPushButton::clicked, this, &MainWindow::copyAddress);
    connect(ui->backButtonReceive, &QPushButton::clicked, this, &MainWindow::backToBalance);

    connect(ui->sendButton, &QPushButton::clicked,
            this, &MainWindow::openSendPage);

    // Send page buttons
    connect(ui->sendTxButton,   &QPushButton::clicked,
            this, &MainWindow::sendTransactionFromPage);
    connect(ui->backButtonSend, &QPushButton::clicked,
            this, &MainWindow::backFromSendPage);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openCreatePage() {
    QString m = QString::fromStdString(WalletManager::instance().createNewWallet());
    ui->mnemonicLabel->setText(m);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::copyMnemonic() {
    QApplication::clipboard()->setText(ui->mnemonicLabel->text());
    ui->statusbar->showMessage("Mnemonic copied to clipboard", 3000);
}

void MainWindow::openWithGenerated() {
    QString m = ui->mnemonicLabel->text().trimmed();
    if (m.isEmpty()) {
        ui->statusbar->showMessage("No mnemonic to load", 3000);
        return;
    }
    if (!WalletManager::instance().loadWallet(m.toStdString())) {
        ui->statusbar->showMessage("Failed to load wallet", 3000);
        return;
    }

    ui->coinList->clear();
    ui->coinList->addItem("Ethereum");
    ui->coinList->addItem("Solana");
    ui->coinList->setCurrentRow(0);
    WalletManager::instance().setCurrentCoin(TWCoinTypeEthereum);

    ui->stackedWidget->setCurrentIndex(3);
    updateBalanceInfo();
}

void MainWindow::openExistingPage() {
    ui->mnemonicInput->clear();
    ui->statusbar->clearMessage();
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::openExisting() {
    QString m = ui->mnemonicInput->text().trimmed();
    if (m.isEmpty()) {
        ui->statusbar->showMessage("Please paste your mnemonic", 3000);
        return;
    }
    if (!WalletManager::instance().loadWallet(m.toStdString())) {
        ui->statusbar->showMessage("Failed to load wallet", 3000);
        return;
    }

    ui->coinList->clear();
    ui->coinList->addItem("Ethereum");
    ui->coinList->addItem("Solana");
    ui->coinList->setCurrentRow(0);
    WalletManager::instance().setCurrentCoin(TWCoinTypeEthereum);

    ui->stackedWidget->setCurrentIndex(3);
    updateBalanceInfo();
}

void MainWindow::onCoinSelected(QListWidgetItem *item) {
    if (item->text() == "Ethereum") {
        WalletManager::instance().setCurrentCoin(TWCoinTypeEthereum);
    } else {
        WalletManager::instance().setCurrentCoin(TWCoinTypeSolana);
    }

    updateBalanceInfo();
}

void MainWindow::updateBalanceInfo() {
    std::string info = WalletManager::instance().checkBalance();
    ui->balanceInfoLabel->setText(QString::fromStdString(info));

    double bal = WalletManager::instance().getCurrentBalanceDouble();
    // qDebug() << "DEBUG: on-chain Sepolia balance =" << bal << "ETH";
}

void MainWindow::logout() {
    WalletManager::instance().logout();
    ui->statusbar->clearMessage();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::openHomePage() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::openReceivePage() {
    auto current = ui->coinList->currentItem();
    QString coin = current ? current->text().toUpper() : "COIN";
    ui->receiveTitleLabel->setText(coin + " Address");

    std::string addr = WalletManager::instance().getCurrentAddress();
    ui->addressLabel->setText(QString::fromStdString(addr));

    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::copyAddress() {
    QApplication::clipboard()->setText(ui->addressLabel->text());
    ui->statusbar->showMessage("Address copied", 3000);
}

void MainWindow::backToBalance() {
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::performSendTransaction() {
    static const std::string toAddress = "0x42B1616B653B77a6eAc6A99745cEB6DD2EEbA6eD";
    static const double amount = 0.002;

    auto& mgr = WalletManager::instance();
    std::string result = mgr.sendTransaction(toAddress, amount);

    // qDebug() << result;
    ui->statusbar->showMessage(
        QString::fromStdString("TX → " + result),
        5000
    );
}

void MainWindow::openSendPage() {
    auto current = ui->coinList->currentItem();
    QString coin = current ? current->text().toUpper() : "COIN";
    ui->sendTitleLabel->setText(QString("Sending %1").arg(coin));

    ui->amountInput->setValue(0.0);
    ui->addressInput->clear();

    ui->stackedWidget->setCurrentWidget(ui->pageSend);
}

void MainWindow::sendTransactionFromPage() {
    double amount = ui->amountInput->value();
    QString toAddr = ui->addressInput->text().trimmed();
    if (toAddr.isEmpty()) {
        ui->statusbar->showMessage("Please enter a recipient address", 3000);
        return;
    }

    // qDebug() << amount;
    auto result = WalletManager::instance().sendTransaction(
        toAddr.toStdString(),
        amount
    );

    ui->statusbar->showMessage(
        QString::fromStdString("Sending transaction result → " + result),
        5000
    );
    ui->stackedWidget->setCurrentWidget(ui->pageBalance);
}

void MainWindow::backFromSendPage() {
    ui->stackedWidget->setCurrentWidget(ui->pageBalance);
}