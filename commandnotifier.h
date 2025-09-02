#ifndef COMMANDNOTIFIER_H
#define COMMANDNOTIFIER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QtSql>

class CommandNotifier : public QObject
{
    Q_OBJECT
public:
    explicit CommandNotifier(QSqlDatabase db, QObject *parent = nullptr);

private slots:
    void checkNewCommand();

private:
    void showNotification(int id);

    QSqlDatabase m_db;
    QSystemTrayIcon* tray;
    QTimer* timer;
    int lastId;
};

#endif // COMMANDNOTIFIER_H
