#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class SmtpClient : public QObject {
    Q_OBJECT
public:
    SmtpClient(const QString &host, quint16 port,
               const QString &user, const QString &pass, QObject *parent = nullptr);
    bool sendMail(const QString &from, const QString &to,
                  const QString &subject, const QString &body);

private:
    QString host;
    quint16 port;
    QString user;
    QString pass;
};

#endif // SMTPCLIENT_H
