#include "statistiques.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

using namespace QtCharts;

Statistiques::Statistiques(QWidget *parent) : QWidget(parent)
{
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);

    resize(900, 600);


    updateChart();


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Statistiques::updateChart);
    timer->start(5000);
}

void Statistiques::updateChart()
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT * FROM (
            SELECT c.NOM || ' ' || c.PRENOM AS client_name,
                   SUM(cmd.MONTANT) AS total
            FROM CLIENT c
            JOIN COMMANDE cmd ON c.ID_CLIENT = cmd.ID_CLIENT
            GROUP BY c.NOM, c.PRENOM
            ORDER BY total DESC
        ) WHERE ROWNUM <= 5
    )");

    if (!query.exec()) {
        qDebug() << "Erreur SQL:" << query.lastError().text();
        return;
    }

    auto *series = new QPieSeries();

    while (query.next()) {
        QString clientName = query.value(0).toString();
        double montant = query.value(1).toDouble();
        qDebug() << "Client:" << clientName << ", Montant:" << montant;
        series->append(clientName, montant);
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Top 5 Clients par Montant (Pie Chart)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chartView->setChart(chart); // pas de delete manuel
}
