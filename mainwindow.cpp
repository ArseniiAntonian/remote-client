#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), manager(new QNetworkAccessManager(this)) {
    auto *widget = new QWidget;
    auto *layout = new QVBoxLayout;

    QLabel *taskLabel = new QLabel("Путь к исполняемому файлу:");
    taskPathEdit = new QLineEdit;
    QPushButton *taskBrowseButton = new QPushButton("Выбрать файл");

    QLabel *dataLabel = new QLabel("Путь к файлу с данными:");
    dataPathEdit = new QLineEdit;
    QPushButton *dataBrowseButton = new QPushButton("Выбрать файл");

    QPushButton *sendButton = new QPushButton("Отправить на сервер");

    layout->addWidget(taskLabel);
    layout->addWidget(taskPathEdit);
    layout->addWidget(taskBrowseButton);
    layout->addWidget(dataLabel);
    layout->addWidget(dataPathEdit);
    layout->addWidget(dataBrowseButton);
    layout->addWidget(sendButton);

    widget->setLayout(layout);
    setCentralWidget(widget);

    connect(taskBrowseButton, &QPushButton::clicked, this, &MainWindow::browseTaskFile);
    connect(dataBrowseButton, &QPushButton::clicked, this, &MainWindow::browseDataFile);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::sendFilesToServer);
}

void MainWindow::browseTaskFile() {
    QString taskPath = QFileDialog::getOpenFileName(this, "Выберите исполняемый файл");
    if (!taskPath.isEmpty()) {
        taskPathEdit->setText(taskPath);
    }
}

void MainWindow::browseDataFile() {
    QString dataPath = QFileDialog::getOpenFileName(this, "Выберите файл с данными");
    if (!dataPath.isEmpty()) {
        dataPathEdit->setText(dataPath);
    }
}

void MainWindow::sendFilesToServer() {
    QString taskPath = taskPathEdit->text();
    QString dataPath = dataPathEdit->text();

    QUrl url("http://192.168.1.102:5000/main_upload_files");

    qDebug()<<"[upload file]"<<url<<QFileInfo(taskPath).fileName();

    QFile *taskFile=new QFile(taskPath);
    if(!taskFile->open(QIODevice::ReadOnly)){
        taskFile->deleteLater();
        qDebug()<<"open file error";
        return;
    }

    QFile *dataFile=new QFile(dataPath);
    if(!dataFile->open(QIODevice::ReadOnly)){
        dataFile->deleteLater();
        qDebug()<<"open file error";
        return;
    }

    QNetworkRequest request;
    request.setUrl(url);

    QHttpMultiPart *multi_part = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart task_part;
    task_part.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"task\"; filename=\"%1\";")
                                     .arg(QFileInfo(taskPath).fileName())));

    task_part.setRawHeader("Content-Type", "multipart/form-data");
    task_part.setBodyDevice(taskFile);
    taskFile->setParent(multi_part);

    QHttpPart data_part;
    data_part.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"data\"; filename=\"%1\";")
                                     .arg(QFileInfo(dataPath).fileName())));

    data_part.setRawHeader("Content-Type", "multipart/form-data");
    data_part.setBodyDevice(dataFile);
    dataFile->setParent(multi_part);

    multi_part->append(task_part);
    multi_part->append(data_part);

    QNetworkReply *reply = manager->post(request,multi_part);
    multi_part-> setParent (reply);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::uploadProgress,
            this, [this](qint64 bytesSent, qint64 bytesTotal){
                qDebug()<<"[upload file] bytesSend"<<bytesSent<<"bytesTotal"<<bytesTotal;
            });

    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<"reply"<<status_code<<QString(reply->readAll());

    qDebug()<<"[upload file] finished";
}

