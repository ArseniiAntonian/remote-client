#include "logindialog.h"
#include "ui_logindialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "registerdialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Unable to connect to the database.");
        close();
        return;
    }

    // Создаем элементы интерфейса
    QLabel *usernameLabel = new QLabel("Username:", this);
    usernameLineEdit = new QLineEdit(this);

    QLabel *passwordLabel = new QLabel("Password:", this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login", this);
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::on_loginButton_clicked);

    registerButton = new QPushButton("Register", this);  // Кнопка для регистрации
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::on_registerButton_clicked);

    messageLabel = new QLabel("", this);

    // Основной макет
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameLineEdit);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(messageLabel);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    setLayout(layout);
    setWindowTitle("Login");
    resize(300, 200);
}

LoginDialog::~LoginDialog() {
    if (db.isOpen()) db.close();
}

bool LoginDialog::connectToDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");

    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT NOT NULL)")) {
        qDebug() << "Error creating table:" << query.lastError().text();
        return false;
    }

    return true;
}

void LoginDialog::on_loginButton_clicked() {
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        messageLabel->setText("Please enter both username and password.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    if (query.next()) {
        accept();
    } else {
        messageLabel->setText("Incorrect username or password.");
    }
}

void LoginDialog::on_registerButton_clicked() {
    RegisterDialog registerDialog(this);
    if (registerDialog.exec() == QDialog::Accepted) {
        accept();
    }
}
