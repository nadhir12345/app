#include "client.h"
#include <QSqlQuery>
#include <QVariant>
#include <QRegularExpression>
#include <QDebug>
#include <QMessageBox>



Client::Client() {}
Client::Client(QString nom, QString prenom, QString adresse, QString telephone)
    : nom(nom), prenom(prenom), adresse(adresse), telephone(telephone) {}

Client::Client(int id, QString nom, QString prenom, QString adresse, QString telephone)
    : id(id), nom(nom), prenom(prenom), adresse(adresse), telephone(telephone) {}

void Client::setId(int newId) {
    id = newId;
}

// -------- Méthode de validation --------
bool Client::validerSaisie(QString &erreur) {
    // Vérifier nom : uniquement lettres
    QRegularExpression regexNom("^[A-Za-z]+$");
    if (nom.trimmed().isEmpty() || !regexNom.match(nom).hasMatch()) {
        erreur = "Le nom doit être une chaîne de lettres non vide.";
        return false;
    }

    // Vérifier prénom : uniquement lettres
    if (prenom.trimmed().isEmpty() || !regexNom.match(prenom).hasMatch()) {
        erreur = "Le prénom doit être une chaîne de lettres non vide.";
        return false;
    }

    // Vérifier téléphone : exactement 8 chiffres
    QRegularExpression regexTel("^[0-9]{8}$");
    if (!regexTel.match(telephone).hasMatch()) {
        erreur = "Le téléphone doit contenir exactement 8 chiffres.";
        return false;
    }

    // adresse peut être vide ou n'importe quelle chaîne
    return true;
}

// -------- Ajouter --------
bool Client::ajouter() {
    QString erreur;
    if (!validerSaisie(erreur)) {
        QMessageBox::critical(nullptr, "Erreur Ajout", erreur);
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO CLIENT (NOM, PRENOM, ADRESSE, TEL) VALUES (:nom, :prenom, :adresse, :telephone)");

    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":adresse", adresse);
    query.bindValue(":telephone", telephone);
    return query.exec();
}

// -------- Modifier --------
bool Client::modifier() {
    QString erreur;
    if (!validerSaisie(erreur)) {
        QMessageBox::critical(nullptr, "Erreur Modification", erreur);
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE CLIENT SET NOM = :nom, PRENOM = :prenom, ADRESSE = :adresse, TEL = :telephone WHERE ID_CLIENT = :id");

    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":adresse", adresse);
    query.bindValue(":telephone", telephone);
    query.bindValue(":id", id);

    return query.exec();
}

// -------- Afficher --------
QSqlQueryModel* Client::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel();

    model->setQuery(R"(
        SELECT ID_CLIENT,
               NOM,
               PRENOM,
               ADRESSE,
               TO_CHAR(TEL) AS TEL
        FROM CLIENT
    )");

    return model;
}

// -------- Supprimer --------
bool Client::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM CLIENT WHERE ID_CLIENT = :id");
    query.bindValue(":id", id);
    return query.exec();
}

// -------- Rechercher --------
QSqlQueryModel* Client::rechercher(QString nom, QString prenom, QString adresse, QString triPar)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    nom = nom.trimmed();
    prenom = prenom.trimmed();
    adresse = adresse.trimmed();

    QString queryStr = R"(
            SELECT ID_CLIENT,
                   NOM,
                   PRENOM,
                   ADRESSE,
                   TO_CHAR(TEL) AS TEL
            FROM CLIENT
            WHERE 1=1
        )";

    if (!nom.isEmpty())
        queryStr += " AND LOWER(NOM) LIKE LOWER('%" + nom + "%')";

    if (!prenom.isEmpty())
        queryStr += " AND LOWER(PRENOM) LIKE LOWER('%" + prenom + "%')";

    if (!adresse.isEmpty())
        queryStr += " AND LOWER(ADRESSE) LIKE LOWER('%" + adresse + "%')";

    if (!triPar.isEmpty())
        queryStr += " ORDER BY " + triPar;

    model->setQuery(queryStr);
    return model;
}

// -------- Trier --------
QSqlQueryModel* Client::trier(QString triPar)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QStringList colonnesValides = {"NOM", "PRENOM", "ADRESSE", "TEL"};
    QString queryStr = R"(
            SELECT ID_CLIENT,
                   NOM,
                   PRENOM,
                   ADRESSE,
                   TO_CHAR(TEL) AS TEL
            FROM CLIENT
        )";

    if (!triPar.isEmpty() && colonnesValides.contains(triPar.toUpper()))
        queryStr += " ORDER BY " + triPar;

    model->setQuery(queryStr);
    return model;
}
