#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
#include "commande.h"
#include "statistiques.h"
#include "smtpclient.h"
#include "commandnotifier.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    Client c;
    ui->tableView_client->setModel(c.afficher());

    afficherCommandes();

     connect(ui->comboBox_tri, &QComboBox::currentTextChanged, this, &MainWindow::on_pushButton_rechercher_clicked);
     connect(ui->comboBox_tri, &QComboBox::currentTextChanged,
             this, &MainWindow::on_comboBox_tri_currentTextChanged);
     connect(ui->comboBox_tri, &QComboBox::currentTextChanged,
             this, &MainWindow::on_comboBox_tri_currentTextChange);

     QSqlDatabase db = QSqlDatabase::database();
         notifier = new CommandNotifier(db, this);




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ajouter_clicked()
{
    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QString adresse = ui->lineEdit_adresse->text().trimmed();
    QString tel = ui->lineEdit_tel->text().trimmed();

    if (nom.isEmpty() || prenom.isEmpty() || adresse.isEmpty() || tel.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis !");
        return;
    }


    QRegExp rx("\\d+");
    if (!rx.exactMatch(tel)) {
        QMessageBox::warning(this, "Erreur", "Le numéro de téléphone doit contenir uniquement des chiffres !");
        return;
    }

    Client c(nom, prenom, adresse, tel);

    if (c.ajouter()) {
        QMessageBox::information(this, "Succès", "Client ajouté avec succès !");
        ui->tableView_client->setModel(c.afficher());
        ui->lineEdit_nom->clear();
        ui->lineEdit_prenom->clear();
        ui->lineEdit_adresse->clear();
        ui->lineEdit_tel->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du client !");
    }
}


void MainWindow::on_pushButton_modifier_clicked()
{
    QModelIndex index = ui->tableView_client->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un client à modifier !");
        return;
    }

    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QString adresse = ui->lineEdit_adresse->text().trimmed();
    QString tel = ui->lineEdit_tel->text().trimmed();

    if (nom.isEmpty() || prenom.isEmpty() || adresse.isEmpty() || tel.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis !");
        return;
    }

    QRegExp rx("\\d+");
    if (!rx.exactMatch(tel)) {
        QMessageBox::warning(this, "Erreur", "Le numéro de téléphone doit contenir uniquement des chiffres !");
        return;
    }

    int id = ui->tableView_client->model()->data(ui->tableView_client->model()->index(index.row(), 0)).toInt();

    Client c(id, nom, prenom, adresse, tel);

    if (c.modifier()) {
        QMessageBox::information(this, "Succès", "Client modifié avec succès !");
        ui->tableView_client->setModel(c.afficher());

        ui->lineEdit_nom->clear();
        ui->lineEdit_prenom->clear();
        ui->lineEdit_adresse->clear();
        ui->lineEdit_tel->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification !");
    }
}



void MainWindow::on_tableView_client_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString id = ui->tableView_client->model()->data(ui->tableView_client->model()->index(row, 0)).toString();
    QString nom = ui->tableView_client->model()->data(ui->tableView_client->model()->index(row, 1)).toString();
    QString prenom = ui->tableView_client->model()->data(ui->tableView_client->model()->index(row, 2)).toString();
    QString adresse = ui->tableView_client->model()->data(ui->tableView_client->model()->index(row, 3)).toString();
    QString tel = ui->tableView_client->model()->data(ui->tableView_client->model()->index(row, 4)).toString();


    // ui->lineEdit_id->setText(id);

    ui->lineEdit_nom->setText(nom);
    ui->lineEdit_prenom->setText(prenom);
    ui->lineEdit_adresse->setText(adresse);
    ui->lineEdit_tel->setText(tel);
}


void MainWindow::on_pushButton_supprimer_clicked()
{
    QModelIndex index = ui->tableView_client->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un client à supprimer !");
        return;
    }

    int id = ui->tableView_client->model()->data(ui->tableView_client->model()->index(index.row(), 0)).toInt();

    Client c;
    if (c.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Client supprimé avec succès !");
        ui->tableView_client->setModel(c.afficher());

        ui->lineEdit_nom->clear();
        ui->lineEdit_prenom->clear();
        ui->lineEdit_adresse->clear();
        ui->lineEdit_tel->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression !");
    }
}



void MainWindow::on_pushButton_rechercher_clicked()
{
    QString nom = ui->lineEdit_rechNom->text();
    QString prenom = ui->lineEdit_rechPrenom->text();
    QString adresse = ui->lineEdit_rechAdresse->text();
    QString triPar = ui->comboBox_tri->currentText();

    Client c;
    ui->tableView_client->setModel(c.rechercher(nom, prenom, adresse, triPar));
}

void MainWindow::on_comboBox_tri_currentTextChanged(const QString &triPar)
{
    Client c;
    ui->tableView_client->setModel(c.trier(triPar));
}

void MainWindow::on_comboBox_tri_currentTextChange(const QString &triPar)
{
    ui->tableView_commandes->setModel(Commande::trier(triPar));
}




void MainWindow::on_btnAjouterCommande_clicked()
{
    QString nomClient = ui->lineNomClient->text().trimmed();
    QString dateCommande = ui->dateEditCommande->date().toString("yyyy-MM-dd");
    double montant = ui->lineMontant->text().toDouble();

    if (nomClient.isEmpty() || montant <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir correctement les champs !");
        return;
    }

    Commande cmd(nomClient, dateCommande, montant);

    if (cmd.ajouter()) {
        QMessageBox::information(this, "Succès", "Commande ajoutée !");
        afficherCommandes();
        envoyerEmailCommande(cmd.getId());
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de la commande !");
    }
}

void MainWindow::envoyerEmailCommande(int idCommande)
{
    SmtpClient smtp("smtp.gmail.com", 587, "nadhirdehmeni@gmail.com", "vnle uaov ulfj yand");

    QString to = "nadhirdehmeni@gmail.com";
    QString subject = "Nouvelle commande #" + QString::number(idCommande);
    QString body = "Votre commande a été enregistrée avec l'ID: " + QString::number(idCommande);

    bool success = smtp.sendMail("nadhirdehmeni@gmail.com", to, subject, body);

    if(success)
        qDebug() << "Email envoyé avec succès !";
    else {
        qDebug() << "Erreur lors de l'envoi de l'email ! Vérifie :";
        qDebug() << "- Serveur SMTP et port";
        qDebug() << "- App Password correct";
        qDebug() << "- Connexion Internet autorise Gmail SMTP";
    }
}





void MainWindow::afficherCommandes()
{
    QSqlQueryModel* modelCommandes = Commande::afficherAvecNomClient();
    ui->tableView_commandes->setModel(modelCommandes);
    ui->tableView_commandes->resizeColumnsToContents();
    ui->tableView_commandes->hideColumn(0);
}

void MainWindow::on_tableView_commandes_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    QAbstractItemModel *model = ui->tableView_commandes->model();

    int idCommande = model->data(model->index(row, 0)).toInt();
    QString nomClient = model->data(model->index(row, 1)).toString();
    QString dateCommande = model->data(model->index(row, 5)).toString();
    double montant = model->data(model->index(row, 6)).toDouble();

    currentIdCommande = idCommande;

    ui->lineNomClient->setText(nomClient);
    ui->dateEditCommande->setDate(QDate::fromString(dateCommande, Qt::ISODate));
    ui->lineMontant->setText(QString::number(montant, 'f', 3));

    qDebug() << "Commande sélectionnée:" << idCommande << nomClient << dateCommande << montant;
}






void MainWindow::on_btnModifierCommande_clicked()
{
    int idCommande = currentIdCommande; // ناخذ ID من المتغيّر
    QString nomClient = ui->lineNomClient->text().trimmed();
    QString dateCommande = ui->dateEditCommande->date().toString("yyyy-MM-dd");
    double montant = ui->lineMontant->text().toDouble();

    if (idCommande <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une commande valide !");
        return;
    }

    if (nomClient.isEmpty() || montant <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir correctement les champs !");
        return;
    }

    Commande cmd(nomClient, dateCommande, montant, idCommande);

    if (cmd.modifier()) {
        QMessageBox::information(this, "Succès", "Commande modifiée !");
        afficherCommandes();
        ui->lineNomClient->clear();
        ui->dateEditCommande->setDate(QDate::currentDate());
        ui->lineMontant->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification de la commande !");
    }
}


void MainWindow::on_btnSupprimerCommande_clicked()
{
    if (currentIdCommande <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une commande à supprimer !");
        return;
    }

    auto reply = QMessageBox::question(
        this, "Confirmation",
        "Voulez-vous vraiment supprimer cette commande ?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        Commande cmd("", "", 0, currentIdCommande);
        if (cmd.supprimer()) {
            QMessageBox::information(this, "Succès", "Commande supprimée !");
            afficherCommandes();

            // تفريغ الحقول
            ui->lineNomClient->clear();
            ui->dateEditCommande->setDate(QDate::currentDate());
            ui->lineMontant->clear();
            currentIdCommande = -1;

        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression !");
        }
    }
}


void MainWindow::exportTableViewToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", QString(), "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setFullPage(false);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    QAbstractItemModel *model = ui->tableView_commandes->model();

    int rows = model->rowCount();
    int cols = model->columnCount();

    QRect rect = printer.pageRect();
    int x = rect.left() + 20;
    int y = rect.top() + 40;   // شوية space من فوق
    int colWidth = rect.width() / cols - 10;
    int rowHeight = 30;

    // --- عنوان كبير ---
    QFont titleFont("Arial", 14, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(rect, Qt::AlignHCenter | Qt::AlignTop, "Liste des Commandes");
    y += 40;

    // --- ستايل الجدول ---
    QFont headerFont("Arial", 11, QFont::Bold);
    painter.setFont(headerFont);
    painter.setBrush(QColor("#DCE6F1"));   // خلفية headers
    painter.setPen(Qt::black);

    // --- رؤوس الأعمدة ---
    /*QFont headerFont("Arial", 11, QFont::Bold);
    painter.setFont(headerFont);
    painter.setPen(Qt::black);*/

    for (int col = 0; col < cols; ++col) {
        QRect cellRect(x + col * colWidth, y, colWidth, rowHeight);

        // خلفية العنوان بلون فاتح
        painter.fillRect(cellRect, QColor("#DCE6F1"));
        painter.drawRect(cellRect);

        QString header = model->headerData(col, Qt::Horizontal).toString();
        painter.drawText(cellRect.adjusted(2, 0, -2, 0), Qt::AlignCenter, header);
    }
    y += rowHeight;

    // --- البيانات ---
    QFont dataFont("Arial", 10);
    painter.setFont(dataFont);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QRect cellRect(x + col * colWidth, y, colWidth, rowHeight);

            // خلفية متناوبة للصفوف
            painter.fillRect(cellRect, (row % 2 == 0) ? QColor("#FFFFFF") : QColor("#F9F9F9"));
            painter.drawRect(cellRect);

            QString data = model->data(model->index(row, col)).toString();

            // نصوص: ID و Montant تتوسّط، البقية على اليسار
            if (col == 0 || col == cols-1)
                painter.drawText(cellRect.adjusted(3, 0, -3, 0), Qt::AlignCenter, data);
            else
                painter.drawText(cellRect.adjusted(3, 0, -3, 0), Qt::AlignLeft | Qt::AlignVCenter, data);
        }
        y += rowHeight;

        // معالجة تجاوز الصفحة
        if (y + rowHeight > rect.bottom() - 50) {
            printer.newPage();
            y = rect.top() + 40;
        }
    }

    painter.end();

    QMessageBox::information(this, "Succès", "Le fichier PDF a été généré avec succès !");
}


void MainWindow::on_exportTableViewToPDF_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", QString(), "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setFullPage(false);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    QAbstractItemModel *model = ui->tableView_commandes->model();

    int rows = model->rowCount();
    int cols = model->columnCount();

    QRect rect = printer.pageRect();
    int x = rect.left() + 20;
    int y = rect.top() + 40;
    int colWidth = rect.width() / cols - 10;
    int rowHeight = 30;

    // --- عنوان كبير ---
    QFont titleFont("Arial", 14, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(rect, Qt::AlignHCenter | Qt::AlignTop, "Liste des Commandes");
    y += 40;

    // --- رؤوس الأعمدة ---
    QFont headerFont("Arial", 11, QFont::Bold);
    painter.setFont(headerFont);

    for (int col = 0; col < cols; ++col) {
        QRect cellRect(x + col * colWidth, y, colWidth, rowHeight);
        painter.fillRect(cellRect, QColor("#DCE6F1"));
        painter.drawRect(cellRect);

        QString header = model->headerData(col, Qt::Horizontal).toString();
        painter.drawText(cellRect.adjusted(2, 0, -2, 0), Qt::AlignCenter, header);
    }
    y += rowHeight;

    // --- البيانات ---
    QFont dataFont("Arial", 10);
    painter.setFont(dataFont);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QRect cellRect(x + col * colWidth, y, colWidth, rowHeight);
            painter.fillRect(cellRect, (row % 2 == 0) ? QColor("#FFFFFF") : QColor("#F9F9F9"));
            painter.drawRect(cellRect);

            QString data = model->data(model->index(row, col)).toString();

            if (col == 0 || col == cols-1)
                painter.drawText(cellRect.adjusted(3, 0, -3, 0), Qt::AlignCenter, data);
            else
                painter.drawText(cellRect.adjusted(3, 0, -3, 0), Qt::AlignLeft | Qt::AlignVCenter, data);
        }
        y += rowHeight;

        if (y + rowHeight > rect.bottom() - 50) {
            printer.newPage();
            y = rect.top() + 40;
        }
    }

    painter.end();
    QMessageBox::information(this, "Succès", "Le fichier PDF a été généré avec succès !");
}


void MainWindow::on_pushButton_rechercherCommande_clicked()
{
    QString nom = ui->lineEdit_rechNomClient->text().trimmed();
    QString prenom = ui->lineEdit_rechPrenomClient->text().trimmed();

    double montant = 0.0;
    QString montantText = ui->lineEdit_rechMontant->text().trimmed();
    if (!montantText.isEmpty()) {
        bool ok;
        montant = montantText.toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Erreur", "Le montant doit être un nombre valide !");
            return;
        }
    }

    ui->tableView_commandes->setModel(Commande::rechercher(nom, prenom, montant));
}

void MainWindow::on_pushButton_resetRechercheCommande_clicked()
{
    ui->lineEdit_rechNomClient->clear();
    ui->lineEdit_rechPrenomClient->clear();
    ui->lineEdit_rechMontant->clear();

    ui->tableView_commandes->setModel(Commande::afficherAvecNomClient());
}




void MainWindow::on_btnStats_clicked()
{
    Statistiques *statWin = new Statistiques();
    statWin->show();
}

