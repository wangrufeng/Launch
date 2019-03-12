#ifndef OPERATEFILE_H
#define OPERATEFILE_H
#include <QString>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QDateTime>
#include <QDataStream>
#include <QTextStream>
#include"QXmlStreamReader"

#define UPDATEFILENAME "update.xml"
#define INIFILE "config.ini"
enum KIND
{
    LOGINSUCCESS,
    LOGINFAILL,

};
class Login{
public:
    QString rybh;
    QString rymc;
    QString gsbh;
    QString fgsbh;
    QString yyzbh;
    QString sfsjsfjg;
};
class UpdateFile{
public:
    QString name;
    QString path;
    QString last_update_time;
    QString version;
};

class OperateFile
{

public:
    OperateFile();

public:
    /**************************
    *readiniFile 读取ini配置文件
    *参数说明：
    *filename   文件的绝对路径
    *indexName  对应配置文件中索引名称 ex: UMP/IP
    *asReslut   返回结果
    ***************************/
    static void readiniFile(const QString& filename,const QString& indexName,QString& asReslut);
    /**************************
    *writeiniFile 修改ini配置文件
    *参数说明：
    *filename   文件的绝对路径
    *indexName  对应配置文件中索引名称 ex: UMP/IP
    *asData     写入内容
    ***************************/
    static void writeiniFile(const QString& filename,const QString& indexName,const QString& asData);
    /**************************
    *deleteFile 删除文件
    *参数说明：
    *fileName  要删除文件的绝对路径
    ***************************/
    static bool deleteFile(const QString& fileName);
    /**************************
    *tracelog 跟踪日志
    *参数说明：
    *asData  日志内容
    ***************************/
    static void tracelog(const QString& asData);
    /*

    */
    static void deletefile(const QString &path);
    /*拷贝文件*/
    static bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    /*拷贝目录*/
    static bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);
    static void getUpdateFileList(const QString &filename, QList<UpdateFile> &filelist);
    static void getXmlVersion(const QString &filename,QString & version);
    static QString mkMutiDir(const QString path);

};

#endif // OPERATEFILE_H
