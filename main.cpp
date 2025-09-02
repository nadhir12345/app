#include "mainwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "Drivers disponibles :" << QSqlDatabase::drivers();

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("OracleXE");
    db.setUserName("system");
    db.setPassword("Champion");





    if (!db.open()) {
        qDebug() << "❌ Échec de la connexion à Oracle:" << db.lastError().text();
    } else {
        qDebug() << "✅ Connexion Oracle réussie!";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
