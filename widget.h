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
    void sendGetRequest();
    void makeUrl();
private slots:
    void onReplyFinished(QNetworkReply *reply);
};
#endif // WIDGET_H
