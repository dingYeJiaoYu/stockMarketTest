#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class QNetworkAccessManager;
class QNetworkReply;
class QLineSeries;
class QListWidgetItem;
class QValueAxis;
class QDateTimeAxis;
class QChart;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QNetworkAccessManager* m_manager;
    QString m_baseUrl;
    QJsonObject m_requestObj;
    QJsonObject m_req;
    QUrl  m_url;
    QString m_token;
    QTimer* m_timer;//用于定时查询股票价格的定时器
    QLineSeries *m_series;
    QString m_curStock;
    QValueAxis* y_axis;
    QDateTimeAxis* x_axis;
    double m_minRate  =   0.98;
    double m_maxRate    =   1.02;
    QChart *m_chart;
    void sendGetRequest();
    void makeUrl(QStringList codelist);
    void initChartView();//初始化chart视图
private slots:
    void onReplyFinished(QNetworkReply *reply);
    void on_pushButton_clicked();
    void updateQuery();
    void on_listWidget_itemClicked(QListWidgetItem *item);
};
#endif // WIDGET_H
