#ifndef HEARTBEATSERVER_H
#define HEARTBEATSERVER_H
#include <QStringList>
#include <QTcpServer>
#include"qthread.h"
class HeartBeatServer:public QTcpServer
{
     Q_OBJECT
public:
    HeartBeatServer(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
public slots:
    void init();
private:
    QString PORT;

};

#endif // HEARTBEATSERVER_H
