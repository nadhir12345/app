#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
#include "commande.h"
#include "smtpclient.h"
#include "commandnotifier.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_ajouter_clicked();

    void on_pushButton_modifier_clicked();

    void on_tableView_client_clicked(const QModelIndex &index);

    void on_pushButton_supprimer_clicked();

    void on_pushButton_rechercher_clicked();

    void on_comboBox_tri_currentTextChanged(const QString &triPar);

    void on_btnAjouterCommande_clicked();

    void afficherCommandes();

    void on_tableView_commandes_clicked(const QModelIndex &index);

    void on_btnModifierCommande_clicked();

    void on_btnSupprimerCommande_clicked();

    void exportTableViewToPDF();

    void on_exportTableViewToPDF_clicked();

    void on_pushButton_rechercherCommande_clicked();

    void on_pushButton_resetRechercheCommande_clicked();

    void on_comboBox_tri_currentTextChange(const QString &critere);

    void on_btnStats_clicked();

    void envoyerEmailCommande(int idCommande);






private:
    Ui::MainWindow *ui;
    int currentIdCommande = -1; // نخزّن فيه الـ ID المختار
    CommandNotifier* notifier;
};
#endif // MAINWINDOW_H
