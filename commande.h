#ifndef COMMANDE_H
#define COMMANDE_H

#include <QString>
#include <QSqlQueryModel>

class Commande {
public:
    Commande(QString nomClient, QString dateCommande, double montant);
    Commande(QString nomClient, QString dateCommande, double montant, int idCommande);

    bool ajouter();
    static QSqlQueryModel* afficherAvecNomClient();
    bool modifier();
    bool supprimer();
    static QSqlQueryModel* rechercher(const QString& nomClient, const QString& prenomClient, double montantMin);
    static QSqlQueryModel* trier(QString triPar);
    int getId() const { return idCommande; }
    bool validerSaisie(QString &erreur);



private:
    QString nomClient;
    QString dateCommande;
    double montant;
    int idCommande = -1;
};

#endif // COMMANDE_H
