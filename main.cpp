#include <QApplication>
#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        MainWindow mainWindow;
        mainWindow.show();
        return app.exec();
    } else {
        return 0;
    }
}
