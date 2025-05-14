//
// Created by denhumen on 5/14/25.
//

#include <QApplication>
#include "src/ui/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}