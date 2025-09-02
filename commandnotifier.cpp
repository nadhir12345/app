#include "commandnotifier.h"
#include <QDebug>

CommandNotifier::CommandNotifier(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_db(db), lastId(0)
{
    tray = new QSystemTrayIcon(QIcon(":/icon.png"), this);
    tray->show();

    // نعمل initialization قبل ما يبدأ التايمر
    if (m_db.isOpen()) {
        QSqlQuery query(m_db);
        if (query.exec("SELECT MAX(ID_COMMANDE) FROM COMMANDE") && query.next()) {
            lastId = query.value(0).toInt();  // نحط آخر commande موجودة
        }
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CommandNotifier::checkNewCommand);
    timer->start(5000); // كل 5 ثواني
}


void CommandNotifier::checkNewCommand()
{
    if (!m_db.isOpen()) return;

    QSqlQuery query(m_db);
    query.prepare("SELECT MAX(ID_COMMANDE) FROM COMMANDE");
    if (query.exec() && query.next()) {
        int maxId = query.value(0).toInt();
        if (maxId > lastId) {
            lastId = maxId;
            showNotification(maxId);
        }
    }
}

void CommandNotifier::showNotification(int id)
{
    QSqlQuery query(m_db);

    // تعديل الاستعلام ليعمل JOIN مع جدول CLIENT
    query.prepare(
        "SELECT CLI.NOM AS NOM_CLIENT, "
        "CLI.PRENOM AS PRENOM_CLIENT, "
        "CLI.ADRESSE AS ADRESSE_CLIENT, "
        "CLI.TEL AS TELEPHONE_CLIENT, "
        "TO_CHAR(C.DATE_COMMANDE, 'YYYY-MM-DD') AS DATE_COMMANDE, "
        "C.MONTANT AS MONTANT "
        "FROM COMMANDE C "
        "JOIN CLIENT CLI ON C.ID_CLIENT = CLI.ID_CLIENT "
        "WHERE C.ID_COMMANDE = :id"
    );


    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        QString nom = query.value("NOM_CLIENT").toString();
        QString prenom = query.value("PRENOM_CLIENT").toString();
        QString date = query.value("DATE_COMMANDE").toString();
        double montant = query.value("MONTANT").toDouble();

        QString message = QString("Client: %1 %2\nDate: %3\nMontant: %4 DT")
                              .arg(nom)
                              .arg(prenom)
                              .arg(date)
                              .arg(montant);

        tray->showMessage("Nouvelle Commande", message, QSystemTrayIcon::Information, 5000);
        //QSound::play(":/notif.wav");
    }
}

