#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"QtNetwork"
#include"heartbeatserver.h"
#include"QList"
#include"operatefile.h"
#include <QNetworkAccessManager>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket *clientSocket;
    quint64 nextBlockSize;
    QString version,version_s;
    int bytesWritten;
    QThread *client_thread;
    QThread *server_thread;
    HeartBeatServer *beatserver;
    QList<UpdateFile>filelist;
    QString updateString;
    QString IP;
    QString PORT;
    QString UpdateDir;
    QString UMPDir;
    QFile *fp;
    QHash<QString,QString>path_file_list;
    int updateFileSum;
    bool isGetXmlFile;
    bool isExit;
    QNetworkAccessManager *accessManager;
    QNetworkRequest request;
    QNetworkReply *reply;
private slots:
    void disconnect();
    void communicate();
    void unconnect(QAbstractSocket::SocketError socketError);
    void replyFinished(QNetworkReply *);
    void httpReadyRead();
    void httpFinished();
    void startRequest();
    void getCurrentVersion();
    void updateDataReadProgress(qint64,qint64);

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    static void AutoRunWithSystem(bool IsAutoRun, QString AppName)
    {
        QSettings *reg = new QSettings(
            "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            QSettings::NativeFormat);

        if (IsAutoRun) {
            QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            reg->setValue(AppName, strAppPath);
        } else {
            reg->setValue(AppName, "");
        }
    }

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
