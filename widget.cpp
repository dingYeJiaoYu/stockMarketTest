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

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_token =   "1c71b87778c507b69ae8e80a6ae3e730-c-app";
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &Widget::onReplyFinished);
    // QString url = "https://quote.tradeswitcher.com/quote-stock-b-api/depth-tick?token=1c71b87778c507b69ae8e80a6ae3e730-c-app&query= { \"trace\": \"bdb19512-a799-4c1e-b593-916b01b7a0a9\", \"data\": { \"symbol_list\": [ { \"code\": \"TSLA.US\" }, { \"code\": \"AAPL.US\" } ] } }"; // 示例 URL
    makeUrl();
    sendGetRequest();
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

void Widget::makeUrl()
{
    m_baseUrl   =   "https://quote.tradeswitcher.com/quote-stock-b-api/depth-tick";
    QJsonValue tracevalue   =   QUuid::createUuid().toString();

    QJsonObject data;
    QJsonArray symbolArr;
    QJsonObject s1,s2;
    s1["code"]  =   "TSLA.US";
    s2["code"]  =   "300823.SZ";

    symbolArr.append(s1);
    symbolArr.append(s2);

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

        // 处理 JSON 数据
        qDebug() << "Parsed JSON:" << jsonObject;
        // qDebug()<<jsonObject.keys();

    } else {
        // 输出错误信息
        qDebug() << "Error:" << reply->errorString();
    }

    // 释放资源
    reply->deleteLater();
}
