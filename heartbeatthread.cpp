#include "heartbeatthread.h"
#include "QProcess"
#include"qdebug.h"
#include"qthread.h"
HeartBeatThread::HeartBeatThread(qintptr socketDescriptor):socketDescriptor(socketDescriptor)
{

}
QDataStream & operator >>(QDataStream &in,Login &LoginMessage)
{
    in>>LoginMessage.fgsbh>>LoginMessage.gsbh>>LoginMessage.rybh\
    >>LoginMessage.rymc>>LoginMessage.sfsjsfjg>>LoginMessage.yyzbh;
    return in;
}
void HeartBeatThread::init()
{
   tcpsocket=new QTcpSocket(this);
   timer=new QTimer(this);
   if (!tcpsocket->setSocketDescriptor(socketDescriptor))
   {
       //emit error(tcpsocket->error());
       qDebug()<<tcpsocket->error();
       return;
   }
   connect(tcpsocket, SIGNAL(readyRead()),this, SLOT(receiveMsg()));
   connect(timer,SIGNAL(timeout()),this,SLOT(checkClientState()));
   timer->start(5000);
}
void HeartBeatThread::receiveMsg()
{
    QDataStream in(tcpsocket);
    quint32 nextBlockSize=0;
    in.setVersion(QDataStream::Qt_5_7);
    if(nextBlockSize==0)
    {
        if(tcpsocket->bytesAvailable()<sizeof(quint64))
            return;

    }
    in >> nextBlockSize;
    if ((quint64)(tcpsocket->bytesAvailable())<nextBlockSize)
    {
        return;
    }
    in>>client_name;
    if(client_name=="SPPserver")
    {
        in>>m_data;
        //qDebug()<<"recv from client:"<<nextBlockSize<<m_data.fgsbh<<m_data.gsbh<<m_data.rybh;
    }
    recvtime=QDateTime::currentDateTime();
    this->sendHeartbeatPack();
}
void HeartBeatThread::checkClientState()
{

    if(QDateTime::currentDateTime().toTime_t()-recvtime.toTime_t()>10)
    {
        timer->stop();
        QString filename;
        if(client_name=="SPPserver")
        {
            qDebug()<<"client is dead";
            QStringList arguments;
            OperateFile::readiniFile(INIFILE,"file_path/SPPServer",filename);
            arguments.append("withoutlogin");
            QProcess::startDetached(filename,arguments);
        }
        else if(client_name=="EC")
        {
           //OperateFile::readiniFile(INIFILE,"file_path/EC",filename);
           qDebug()<<"run bat";
           system("runbat.vbs");
        }
        this->deleteLater();
    }
}
void HeartBeatThread::sendHeartbeatPack()
{
    QByteArray block;
    QString clien_name("Launch");
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out<<(quint32)0;
    out<<clien_name;
    out.device()->seek(0);
    out<<(quint32)(block.size()-sizeof(quint32));
    tcpsocket->write(block,block.size());
    tcpsocket->flush();
}
