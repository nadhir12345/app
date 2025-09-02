#ifndef CLIENT_H
#define CLIENT_H
#include <QString>
#include <QSqlQueryModel>

class Client {
private:
    int id;
    QString nom, prenom, adresse, telephone;

public:
    Client();
    Client(QString, QString, QString, QString);
    Client(int, QString, QString, QString, QString);

    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool modifier();
    bool validerSaisie(QString &erreur);

    void setId(int);

    QSqlQueryModel* rechercher(QString nom, QString prenom, QString adresse, QString triPar);

    QSqlQueryModel* trier(QString triPar);



    // + méthode de recherche/tri/stat
};

#endif
