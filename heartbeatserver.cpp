#include "heartbeatserver.h"
#include "operatefile.h"
#include "heartbeatthread.h"
HeartBeatServer::HeartBeatServer(QObject *parent)
    : QTcpServer(parent)
{
    OperateFile::readiniFile(INIFILE,"monitor/PORT",PORT);
}
void HeartBeatServer::incomingConnection(qintptr socketDescriptor)
{
    QThread *thread=new QThread();
    HeartBeatThread *heartbeatsocket=new HeartBeatThread(socketDescriptor);
    heartbeatsocket->moveToThread(thread);
    connect(thread,SIGNAL(started()),heartbeatsocket,SLOT(init()));
    thread->start();
}
void HeartBeatServer::init()
{
   this->listen(QHostAddress::Any,quint16(PORT.toInt()));
}
