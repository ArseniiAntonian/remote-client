#include "registerdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent) {
    QLabel *usernameLabel = new QLabel("Username:", this);
    usernameLineEdit = new QLineEdit(this);

    QLabel *passwordLabel = new QLabel("Password:", this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    registerButton = new QPushButton("Register", this);
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::on_registerButton_clicked);

    messageLabel = new QLabel("", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameLineEdit);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(messageLabel);
    layout->addWidget(registerButton);

    setLayout(layout);
    setWindowTitle("Register");
    resize(300, 200);
}

void RegisterDialog::on_registerButton_clicked() {
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        messageLabel->setText("Please fill in all fields.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        QString errorText = query.lastError().text();
        if (errorText.contains("UNIQUE constraint failed")) {
            messageLabel->setText("Username already exists.");
        } else {
            QMessageBox::critical(this, "Database Error", errorText);
        }
    } else {
        accept();
    }
}
