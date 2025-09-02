#ifndef STATISTIQUES_H
#define STATISTIQUES_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QVBoxLayout>

class Statistiques : public QWidget
{
    Q_OBJECT
public:
    explicit Statistiques(QWidget *parent = nullptr);

private slots:
    void updateChart();

private:
    QtCharts::QChartView *chartView;
};

#endif // STATISTIQUES_H
