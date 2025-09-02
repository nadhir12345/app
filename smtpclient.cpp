#include "smtpclient.h"
#include <QTextStream>
#include <QDebug>

SmtpClient::SmtpClient(const QString &h, quint16 p,
                       const QString &u, const QString &pw, QObject *parent)
    : QObject(parent), host(h), port(p), user(u), pass(pw) {}

bool SmtpClient::sendMail(const QString &from, const QString &to,
                          const QString &subject, const QString &body) {
    QTcpSocket socket;
    socket.connectToHost(host, port);
    if (!socket.waitForConnected(5000)) {
        qDebug() << "SMTP: Cannot connect";
        return false;
    }
    auto sendLine = [&](const QString &line) {
        socket.write(line.toUtf8() + "\r\n");
        socket.flush();
        socket.waitForBytesWritten();
        socket.waitForReadyRead();
        qDebug() << socket.readAll();
    };

    sendLine("EHLO localhost");
    sendLine("AUTH LOGIN");
    sendLine(QByteArray(user.toUtf8()).toBase64());
    sendLine(QByteArray(pass.toUtf8()).toBase64());
    sendLine("MAIL FROM:<" + from + ">");
    sendLine("RCPT TO:<" + to + ">");
    sendLine("DATA");
    sendLine("Subject: " + subject);
    sendLine("From: " + from);
    sendLine("To: " + to);
    sendLine("Content-Type: text/html; charset=utf-8");
    sendLine("");
    sendLine(body);
    sendLine(".");
    sendLine("QUIT");

    return true;
}
