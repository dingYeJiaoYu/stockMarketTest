#include "widget.h"
#include "ui_widget.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &Widget::onReplyFinished);
    QString url = "https://quote.tradeswitcher.com/quote-stock-b-api/depth-tick?token=325b74d95d2721dedd5261537391eb83-c-app&query= { \"trace\": \"bdb19512-a799-4c1e-b593-916b01b7a0a9\", \"data\": { \"symbol_list\": [ { \"code\": \"TSLA.US\" }, { \"code\": \"300750.SZ\" } ] } }"; // 示例 URL
    sendGetRequest(url);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::sendGetRequest(const QString &url)
{
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    // 发起 GET 请求
    m_manager->get(request);
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
        // qDebug() << "Parsed JSON:" << jsonObject;
        qDebug()<<jsonObject.keys();

    } else {
        // 输出错误信息
        qDebug() << "Error:" << reply->errorString();
    }

    // 释放资源
    reply->deleteLater();
}
