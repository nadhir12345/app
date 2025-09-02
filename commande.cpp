#include "commande.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QMessageBox>

Commande::Commande(QString nomClient, QString dateCommande, double montant)
    : nomClient(nomClient), dateCommande(dateCommande), montant(montant) {}

Commande::Commande(QString nomClient, QString dateCommande, double montant, int idCommande)
    : nomClient(nomClient), dateCommande(dateCommande), montant(montant), idCommande(idCommande) {}

// -------- Validation --------
bool Commande::validerSaisie(QString &erreur) {
    QRegularExpression regexNom("^[A-Za-z]+$");

    // Vérifier nom client
    if (nomClient.trimmed().isEmpty() || !regexNom.match(nomClient).hasMatch()) {
        erreur = "Le nom du client doit être une chaîne de lettres non vide.";
        return false;
    }

    // Vérifier montant positif
    if (montant < 0) {
        erreur = "Le montant doit être positif.";
        return false;
    }

    // Vérifier date valide (YYYY-MM-DD)
    QRegularExpression regexDate("^\\d{4}-\\d{2}-\\d{2}$");
    if (!regexDate.match(dateCommande).hasMatch()) {
        erreur = "La date doit être au format YYYY-MM-DD.";
        return false;
    }

    return true;
}

// -------- Ajouter --------
bool Commande::ajouter()
{
    QString erreur;
    if (!validerSaisie(erreur)) {
        QMessageBox::critical(nullptr, "Erreur Ajout Commande", erreur);
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT ID_CLIENT FROM CLIENT WHERE NOM = :nom");
    query.bindValue(":nom", nomClient);

    if (!query.exec()) {
        qDebug() << "Erreur recherche ID_CLIENT:" << query.lastError().text();
        return false;
    }

    int idClient = -1;
    if (query.next()) {
        idClient = query.value(0).toInt();
    } else {
        QMessageBox::critical(nullptr, "Erreur Ajout Commande", "Client introuvable.");
        return false;
    }

    query.prepare("INSERT INTO COMMANDE (DATE_COMMANDE, MONTANT, ID_CLIENT) "
                  "VALUES (TO_DATE(:dateCommande, 'YYYY-MM-DD'), :montant, :idClient) "
                  "RETURNING ID_COMMANDE INTO :idCommande");
    query.bindValue(":dateCommande", dateCommande);
    query.bindValue(":montant", montant);
    query.bindValue(":idClient", idClient);
    query.bindValue(":idCommande", idCommande, QSql::Out);

    if (!query.exec()) {
        qDebug() << "Erreur ajout commande:" << query.lastError().text();
        return false;
    }

    return true;
}

// -------- Modifier --------
bool Commande::modifier()
{
    QString erreur;
    if (!validerSaisie(erreur)) {
        QMessageBox::critical(nullptr, "Erreur Modification Commande", erreur);
        return false;
    }

    if (idCommande == -1) {
        QMessageBox::critical(nullptr, "Erreur Modification Commande", "ID commande invalide.");
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT ID_CLIENT FROM CLIENT WHERE NOM = :nom");
    query.bindValue(":nom", nomClient);

    if (!query.exec()) {
        qDebug() << "Erreur recherche ID_CLIENT:" << query.lastError().text();
        return false;
    }

    int idClient = -1;
    if (query.next()) {
        idClient = query.value(0).toInt();
    } else {
        QMessageBox::critical(nullptr, "Erreur Modification Commande", "Client introuvable.");
        return false;
    }

    query.prepare("UPDATE COMMANDE SET DATE_COMMANDE = TO_DATE(:dateCommande, 'YYYY-MM-DD'), "
                  "MONTANT = :montant, ID_CLIENT = :idClient WHERE ID_COMMANDE = :idCommande");
    query.bindValue(":dateCommande", dateCommande);
    query.bindValue(":montant", montant);
    query.bindValue(":idClient", idClient);
    query.bindValue(":idCommande", idCommande);

    if (!query.exec()) {
        qDebug() << "Erreur modification commande:" << query.lastError().text();
        return false;
    }

    return true;
}

// -------- Supprimer --------
bool Commande::supprimer()
{
    if (idCommande == -1) {
        QMessageBox::critical(nullptr, "Erreur Suppression Commande", "ID commande invalide.");
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM COMMANDE WHERE ID_COMMANDE = :idCommande");
    query.bindValue(":idCommande", idCommande);

    if (!query.exec()) {
        qDebug() << "Erreur suppression commande:" << query.lastError().text();
        return false;
    }

    return true;
}

// -------- Afficher avec info client --------
QSqlQueryModel* Commande::afficherAvecNomClient()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QString requete =
        "SELECT C.ID_COMMANDE AS \"ID Commande\", "
        "CLI.NOM AS \"Nom Client\", "
        "CLI.PRENOM AS \"Prenom Client\", "
        "CLI.ADRESSE AS \"Adresse Client\", "
        "TO_CHAR(CLI.TEL) AS \"Telephone Client\", "
        "TO_CHAR(C.DATE_COMMANDE, 'YYYY-MM-DD') AS \"Date Commande\", "
        "C.MONTANT AS \"Montant\" "
        "FROM COMMANDE C "
        "JOIN CLIENT CLI ON C.ID_CLIENT = CLI.ID_CLIENT "
        "ORDER BY C.DATE_COMMANDE DESC";
    model->setQuery(requete);

    if (model->lastError().isValid()) {
        qDebug() << "Erreur récupération commandes:" << model->lastError().text();
    }
    return model;
}

// -------- Rechercher --------
QSqlQueryModel* Commande::rechercher(const QString& nomClient, const QString& prenomClient, double montantMin)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    QString baseQuery =
        "SELECT C.ID_COMMANDE AS \"ID Commande\", "
        "CLI.NOM AS \"Nom Client\", "
        "CLI.PRENOM AS \"Prénom Client\", "
        "CLI.ADRESSE AS \"Adresse Client\", "
        "TO_CHAR(CLI.TEL) AS \"Telephone Client\", "
        "TO_CHAR(C.DATE_COMMANDE, 'YYYY-MM-DD') AS \"Date Commande\", "
        "C.MONTANT AS \"Montant\" "
        "FROM COMMANDE C "
        "JOIN CLIENT CLI ON C.ID_CLIENT = CLI.ID_CLIENT "
        "WHERE 1=1 ";

    if (!nomClient.isEmpty())
        baseQuery += "AND LOWER(CLI.NOM) LIKE '%' || LOWER(:nomClient) || '%' ";
    if (!prenomClient.isEmpty())
        baseQuery += "AND LOWER(CLI.PRENOM) LIKE '%' || LOWER(:prenomClient) || '%' ";
    if (montantMin > 0)
        baseQuery += "AND C.MONTANT >= :montantMin ";

    query.prepare(baseQuery);

    if (!nomClient.isEmpty()) query.bindValue(":nomClient", nomClient);
    if (!prenomClient.isEmpty()) query.bindValue(":prenomClient", prenomClient);
    if (montantMin > 0) query.bindValue(":montantMin", montantMin);

    if (!query.exec()) {
        qDebug() << "Erreur recherche commandes:" << query.lastError().text();
        return model;
    }

    model->setQuery(query);
    return model;
}

// -------- Trier --------
QSqlQueryModel* Commande::trier(QString triPar)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QMap<QString, QString> colonnesValides {
        {"NOM", "CLI.NOM"},
        {"PRENOM", "CLI.PRENOM"},
        {"MONTANT", "C.MONTANT"},
        {"DATE", "C.DATE_COMMANDE"}
    };

    QString queryStr =
        "SELECT C.ID_COMMANDE AS \"ID Commande\", "
        "CLI.NOM AS \"Nom Client\", "
        "CLI.PRENOM AS \"Prenom Client\", "
        "CLI.ADRESSE AS \"Adresse Client\", "
        "TO_CHAR(CLI.TEL) AS \"Telephone Client\", "
        "TO_CHAR(C.DATE_COMMANDE, 'YYYY-MM-DD') AS \"Date Commande\", "
        "C.MONTANT AS \"Montant\" "
        "FROM COMMANDE C "
        "JOIN CLIENT CLI ON C.ID_CLIENT = CLI.ID_CLIENT ";

    QString triParUpper = triPar.toUpper();
    if (!triPar.isEmpty() && colonnesValides.contains(triParUpper))
        queryStr += " ORDER BY " + colonnesValides[triParUpper] + " ASC";

    model->setQuery(queryStr);

    if (model->lastError().isValid())
        qDebug() << "Erreur tri commandes:" << model->lastError().text();

    return model;
}
