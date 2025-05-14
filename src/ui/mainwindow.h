//
// Created by denhumen on 5/14/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "wallet/WalletManager.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Home page
    void openCreatePage();
    void openExistingPage();

    // Create page
    void copyMnemonic();
    void openWithGenerated();    // “Open Wallet” on Create page
    void openHomePage();         // Back button

    // Existing page
    void openExisting();

    // Balance page
    void onCoinSelected(QListWidgetItem* item);
    void logout();
    void openReceivePage();
    void copyAddress();
    void backToBalance();
    void performSendTransaction();
    void openSendPage();
    void sendTransactionFromPage();
    void backFromSendPage();
private:
    Ui::MainWindow *ui;
    void updateBalanceInfo();
};


#endif //MAINWINDOW_H
