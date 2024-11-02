#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QLineEdit>

class QPushButton;
class QLabel;
class QVBoxLayout;
class QFileDialog;
class QMessageBox;
class QNetworkReply;
class QHttpMultiPart;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void browseTaskFile();
    void browseDataFile();
    void sendFilesToServer();

private:
    QLineEdit *taskPathEdit;
    QLineEdit *dataPathEdit;
    QNetworkAccessManager *manager;
};

#endif // MAINWINDOW_H
