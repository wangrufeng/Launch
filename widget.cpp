#include "widget.h"
#include "ui_widget.h"
#include"QFile"
#include"QMessageBox"
#include"operatefile.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    nextBlockSize=0;
    bytesWritten=0;
    isGetXmlFile=false;
    isExit=false;
    ui->progressBar->setMaximum(0);
    clientSocket=new QTcpSocket(0);
    beatserver=new HeartBeatServer();
    server_thread=new QThread();
    accessManager = new QNetworkAccessManager(this);
    connect(server_thread,SIGNAL(started()),beatserver,SLOT(init()));
    connect(clientSocket,SIGNAL(readyRead()),this,SLOT(communicate()));
    connect(clientSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(unconnect(QAbstractSocket::SocketError)));
    connect(accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    //connect(accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    beatserver->moveToThread(server_thread);
    server_thread->start();
    OperateFile::readiniFile(INIFILE,"update_server/IP",IP);
    OperateFile::readiniFile(INIFILE,"update_server/PORT",PORT);
    //clientSocket->connectToHost(QHostAddress(IP),quint16(PORT.toInt()));
    OperateFile::readiniFile(INIFILE,"path/UPDATEDIR",UpdateDir);
    OperateFile::readiniFile(INIFILE,"path/UMPDIR",UMPDir);
    getCurrentVersion();
}
void Widget::communicate()
{
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_5);
    //while(1)
    //{
    int bytes=(int)(clientSocket->bytesAvailable());
    //qDebug()<<"bytes:"<<bytes;
    bytesWritten+=bytes;
    if (nextBlockSize == 0)
    {
        if (quint64(bytes) < sizeof(quint64))
        {
            qDebug()<<"no value";
            //break;
            return;
        }
       in >> nextBlockSize;
        //设置进度条最大值
       qDebug()<<"nextblocksize:"<<nextBlockSize;
       if(isGetXmlFile==true)
       {
           ui->progressBar->setMaximum((int)nextBlockSize);
           this->show();
       }
    }
    if(ui->progressBar->maximum())
    {
        //设置进度条当前进度
        ui->progressBar->setValue(bytes);
    }
    if (quint64(bytes)<nextBlockSize)
    {
        qDebug()<<"recv done";
        return;
    }

        //通信协议
        quint16 protocol;
        in>>protocol;
        switch(protocol)
        {
            case 0x0001:
            {
                QByteArray block;
                in>>block;
                QFile update_file("update.temp");
                if(update_file.open(QIODevice::ReadWrite)&&block.length()!=0)
                {
                    update_file.write(block);
                    update_file.close();
                }
                QString version_server,version_client;
                OperateFile::getXmlVersion("update.temp",version_server);
                OperateFile::getXmlVersion(UPDATEFILENAME,version_client);
                if(version_client==version_server)
                {
                     this->hide();
                     OperateFile::deleteFile("update.temp");
                     QProcess::startDetached("SPPServer.exe");
                }
                else
                {
                    QList<UpdateFile>list_server,list_client,list_temp;
                    OperateFile::getUpdateFileList("update.temp",list_server);
                    OperateFile::getUpdateFileList(UPDATEFILENAME,list_client);
                      for(int i=0;i<list_server.size();i++)
                     {
                          for(int j=0;j<list_client.size();j++)
                          {
                             if(list_server.at(i).name==list_client.at(j).name)
                             {
                                 if( list_server.at(i).version!=list_client.at(j).version)
                                    list_temp.append(list_server.at(i));
                                 break;
                             }
                             else if(j+1==list_client.size()&&list_client.at(j).name!=list_server.at(i).name)
                             {
                                 list_temp.append(list_server.at(i));
                             }

                         }
                         qDebug()<<"size:"<<list_temp.size();
                     }
                     updateFileSum=list_temp.size();
                     for(int i=0; i<list_temp.size();i++)
                     {

                         updateString.append(list_temp.at(i).name);//路径+文件名
                         qDebug()<<"11111111"<<list_temp.at(i).path<<list_temp.at(i).name;
                         path_file_list.insert(list_temp.at(i).name,list_temp.at(i).path);
                         if(i+1!=list_temp.size())
                             updateString.append("@");
                     }
                    qDebug()<<"updatestring:"<<updateString;
                    QByteArray block;
                    QDataStream out(&block, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Qt_4_5);
                    out<<quint64(0)<<quint16(0x0001);
                    out<<updateString;
                    out.device()->seek(0);
                    out<<quint64(block.size()-sizeof(quint64));
                    clientSocket->write(block);
                    isGetXmlFile=true;
                    qDebug()<<"sendsuccess";
                    ui->label->setText(QString("正在下载，共%1个文件").arg(QString::number(updateFileSum)));

                }
                break;
            }
            case 0x0002:
            {
                QStringList update_list=updateString.split("@");
                QString filename;
                qDebug()<<"update list:"<<update_list;
                foreach (QString update_file, update_list) {

                   QByteArray block;
                   in>>block;
                   filename=path_file_list.value(update_file)+update_file;
                   qDebug()<<"block size:"<<block.size();
                   QFile file(filename);
                   if (file.open(QIODevice::WriteOnly)&&block.length()!=0)
                   {
                       qDebug()<<"filname:"<<filename;
                       file.write(block);
                       file.close();
                   }
                   else
                       qDebug()<<"222222222222:"<<file.errorString();
                }
                OperateFile::deleteFile(UPDATEFILENAME);
                QFile createfile("update.temp");
                createfile.rename(UPDATEFILENAME);
                this->hide();
                //QProcess::startDetached(path_file_list.value("SPPServer.exe")+"SPPServer.exe");
                clientSocket->deleteLater();
                break;
            }
        }
        nextBlockSize=0;
        qDebug()<<"write finished";
    //}
}
void Widget::unconnect(QAbstractSocket::SocketError socketError)
{
        QProcess::startDetached("SPPServer.exe");
        //qApp->quit();
        this->hide();
}
void Widget::disconnect()
{
    clientSocket->deleteLater();
    QMessageBox::information(this,tr("错误"),tr("网络连接中断！"));
    qApp->quit();
}
void Widget::replyFinished(QNetworkReply *reply)
{
    QString currentversion;
    //QString umpdir;
    QProcess p;
    OperateFile::getXmlVersion(UpdateDir+"update.xml",currentversion);
    if(reply->error() == QNetworkReply::NoError)
    {
        char m_receivedata[128]={0};
        reply->read(m_receivedata,128);
        if(currentversion!=QString::fromLocal8Bit(m_receivedata))
        {
            accessManager->disconnect(SIGNAL(finished(QNetworkReply*)));
            startRequest();
        }
        else
        {
            this->hide();
            system("run_ump.vbs");
            QProcess::startDetached("SPPServer.exe");
        }
    }
    else
    {
        this->hide();
        system("run_ump.vbs");
        QProcess::startDetached("SPPServer.exe");
        //p.execute("start_ump.bat");
        //p.waitForFinished();
    }
    reply->deleteLater();
}
void Widget::startRequest()//链接请求
{
    ui->label->setText("正在下载文件……");
    QString downloadstring;
    OperateFile::readiniFile("INTERFACE.DATA","UPDATE/DOWNLOADPACKAGE",downloadstring);
    fp=new QFile("zdsj_tmp.rar");
    if(!fp->open(QIODevice::WriteOnly))
    {
        delete fp;
    }
    request.setUrl(QUrl(downloadstring));
    reply = accessManager->get(request);
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));
    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
}
void Widget::getCurrentVersion()
{
    QString VersionUrl;
    OperateFile::readiniFile("INTERFACE.DATA","UPDATE/GETCURRENTVERSION",VersionUrl);
    request.setUrl(QUrl(VersionUrl));
    reply = accessManager->get(request);
}
void Widget::httpReadyRead()//有可用数据
{
    if (fp) fp->write(reply->readAll()); //如果文件存在，则写入文件
}
void Widget::updateDataReadProgress(qint64 read, qint64 total)
{
    //qDebug()<<"55555555555555"<<"111:"<<read<<"222:"<<total;
    //ui->progressBar->setMaximum(total); //最大值
    //ui->progressBar->setValue(read);  //当前值
}
void Widget::httpFinished()  //完成下载
{
    ui->label->setText("正在解压……");
    QProcess p;
    QString rarstring;
    QString sourcefile;
    QString destfile;
    fp->flush();
    fp->close();
    reply->deleteLater();
    reply = 0;
    delete fp;
    sourcefile=QDir::currentPath()+"/"+"zdsj_tmp.rar";
    destfile=QDir::currentPath()+"/"+UpdateDir;
    rarstring=QString("Rar.exe x -ibck -y -o+ %1 %2").arg(sourcefile).arg(destfile);
    p.execute(rarstring);
    p.waitForFinished();
    QList<UpdateFile>list_server;
    QFileInfo fileinfo;
    OperateFile::getUpdateFileList(UpdateDir+"update.xml",list_server);
    for(int i=0;i<list_server.size();i++)
    {
        if(list_server.at(i).name.contains("Launch.exe"))
        {
            isExit=true;
            continue;
        }
        fileinfo.setFile(UpdateDir+list_server.at(i).name);
        if(fileinfo.isDir())
            OperateFile::copyDirectoryFiles(UpdateDir+list_server.at(i).name,list_server.at(i).path+list_server.at(i).name,true);
        else
            OperateFile::copyFileToPath(UpdateDir+list_server.at(i).name,list_server.at(i).path+list_server.at(i).name,true);
    }
    ui->label->setText("程序启动中……");
    this->hide();

    system("run_ump.vbs");
    //p.waitForFinished();
    QProcess::startDetached("SPPServer.exe");
    /*if(isExit==true) qApp->quit(); else this->hide();*/
}
Widget::~Widget()
{
    delete ui;
}
