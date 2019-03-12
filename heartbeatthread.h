#ifndef HEARTBEATTHREAD_H
#define HEARTBEATTHREAD_H
#include<QObject>
#include"qtcpsocket.h"
#include"qtimer.h"
#include"qdatetime.h"
#include"qdatastream.h"
#include"operatefile.h"
class HeartBeatThread:public QObject
{
 Q_OBJECT
public:
    HeartBeatThread(qintptr socketDescriptor);
private:
    QTcpSocket *tcpsocket;
    qintptr socketDescriptor;
    QDateTime recvtime;
    QTimer *timer;
    Login m_data;
    QString client_name;
protected:
    friend QDataStream & operator >>(QDataStream &out,Login &LoginMessage);
private slots:
    void receiveMsg();
    void checkClientState();
    void init();
    void sendHeartbeatPack();
};

#endif // HEARTBEATTHREAD_H
