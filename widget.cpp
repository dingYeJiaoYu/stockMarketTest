#include "widget.h"
#include "ui_widget.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUuid>
#include <QDebug>
#include <QUrlQuery>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>
#include <QValueAxis>
#include <QDateTimeAxis>

// using namespace QtCharts;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_token =   "1c71b87778c507b69ae8e80a6ae3e730-c-app";
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &Widget::onReplyFinished);

    m_timer =   new QTimer;
    connect(m_timer,&QTimer::timeout,this,&Widget::updateQuery);

    initChartView();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::sendGetRequest()
{
    QNetworkRequest request(m_url);

    // 发起 GET 请求
    m_manager->get(request);
}

void Widget::makeUrl(QStringList codelist)
{
    // m_baseUrl   =   "https://quote.tradeswitcher.com/quote-stock-b-api/depth-tick"; //股票
    m_baseUrl   =   "https://quote.tradeswitcher.com/quote-b-api/depth-tick";//虚拟货币
    QJsonValue tracevalue   =   QUuid::createUuid().toString();

    QJsonObject data;
    QJsonArray symbolArr;

    for(int i=0;i<codelist.size();i++)
    {
        QJsonObject obj;
        obj["code"] = codelist[i];
        symbolArr.append(obj);
    }

    data["symbol_list"]   = symbolArr;
    m_req.insert("trace",tracevalue);
    m_req.insert("data",data);

    m_url.setUrl(m_baseUrl);
    QJsonDocument jdoc(m_req);
    QUrlQuery params;
    params.addQueryItem("token",m_token);
    params.addQueryItem("query",jdoc.toJson());
    m_url.setQuery(params);
    qDebug()<<m_url;
}

void Widget::initChartView()
{
    // 创建一个简单的线性图表
    m_series = new QLineSeries();
    m_chart = new QChart();
    m_chart->addSeries(m_series);
    y_axis    =   new QValueAxis;
    y_axis->setRange(0, 100);  // 设置股票默认价格范围
    y_axis->setTitleText("价格");
    m_chart->addAxis(y_axis, Qt::AlignLeft);
    m_series->attachAxis(y_axis);

    x_axis    =   new QDateTimeAxis;
    x_axis->setFormat("hh:mm:ss");  // 显示时间的格式
    x_axis->setTitleText("时间");
    x_axis->setRange(QDateTime::currentDateTime(),QDateTime::currentDateTime().addSecs(60*30));
    m_chart->addAxis(x_axis, Qt::AlignBottom);
    m_series->attachAxis(x_axis);

    QChartView *chartView = new QChartView;
    chartView->setChart(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->horizontalLayout_3->addWidget(chartView);
}

void Widget::updateQuery()
{
    QStringList codelist;
    codelist.append(m_curStock);
    // int c   =   ui->listWidget->count();
    // for(int i=0;i<c;i++)
    // {
    //     QListWidgetItem* item   =    ui->listWidget->item(i);
    //     codelist.append(item->data(10000).toString());
    // }
    if(codelist.isEmpty())
    {
        return;
    }
    makeUrl(codelist);
    sendGetRequest();
}

void Widget::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // 获取响应数据
        QByteArray responseData = reply->readAll();

        // 输出原始数据
        // qDebug() << "Response Data:" << responseData;

        // 如果响应是 JSON 格式，解析 JSON 数据
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();        
        QJsonObject  btcObj =   jsonObject["data"].toObject()["tick_list"].toArray()[0].toObject();
        double ts  =   btcObj["tick_time"].toString().toDouble() ;
        double v    =   btcObj ["bids"].toArray()[0].toObject()["price"].toString().toDouble();
        if(!m_series->count())
        {
            y_axis->setRange(v*m_minRate,v*m_maxRate);
        }
        if(ts>x_axis->max().toMSecsSinceEpoch())
        {
            x_axis->setMax(x_axis->max().addSecs(60*10));//如果超过当前的X轴最大时间范围，则扩展十分钟
        }
        if(v<y_axis->min())
        {
            y_axis->setMin(y_axis->min()*m_minRate);
        }
        if(v>y_axis->max())
        {
            y_axis->setMax(y_axis->max()*m_maxRate);
        }
        m_series->append(ts,v);

        qDebug()<<ts<<"____"<<v;
        // 处理 JSON 数据
        // qDebug() << "Parsed JSON:" << jsonObject;
        // qDebug()<<jsonObject["data"].toObject().keys();
        // qDebug()<<jsonObject.keys();

    } else {
        // 输出错误信息
        qDebug() << "Error:" << reply->errorString();
    }

    // 释放资源
    reply->deleteLater();
}

void Widget::on_pushButton_clicked()
{
    if(ui->code->text().isEmpty()||ui->name->text().isEmpty())
    {
        //输入不能为空
        return;
    }
    QListWidgetItem* pitem  =   new QListWidgetItem(ui->name->text());
    pitem->setData(10000,ui->code->text());
    ui->listWidget->addItem(pitem);
    // updateQuery();
}


void Widget::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(item==nullptr)
    {
        return;
    }

    QString selStock    =   item->data(10000).toString();
    if(m_curStock==selStock)
    {
        return;
    }
    m_curStock  =   selStock;
    m_chart->setTitle(item->text()+"价格走势");
    m_series->clear();
    m_series->setName(item->text());
    //如果没有启动定时器，则开始启动
    if(!m_timer->isActive())
    {
        m_timer->start(10000);
    }

    updateQuery();//手动刷新一次
}

