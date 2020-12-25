#include "QtconnetSQL.h"
#include <QFile>
#include<QTextStream>
#include<QByteArray>
#include<QIODevice>
#include<QMessageBox>
#include<qtsql/QSqlError>
#include <qtsql/QSqlQuery>
#include <qtsql/QSqlRecord>
#include<QDir>
QString username = QDir::home().dirName();
#define MAX_PATH          260
SqlServer* SqlServer::instance_ = nullptr;//在外部初始化类的静态成员
QString SqlServer::m_server_mysql = NULL;//mysql服务器名(IP)
QString SqlServer::m_dataBase_mysql = NULL;//mysql数据库名
QString SqlServer::m_sqlUserName_mysql = NULL;//mysql用户名
QString SqlServer::m_sqlPassword_mysql = NULL;//mysql密码
QString SqlServer::m_port_mysql;//mysql端口号
QSqlDatabase SqlServer::m_Database;//连接数据库

SqlServer::SqlServer(connect_db_type db_type /*= mysql*/)
{

}

SqlServer::~SqlServer()
{
	CloseServer();
}

void GetPrivateProfileQString(QString strSectionName, QString strKeyName,
	QString strDefault, QString &pReturnedValue,
	int size, QString strFileName)
{
	//设置默认返回字符串
	if (strDefault == NULL)
	{
		strDefault = "";
	}
	//文件不存在，返回默认字符串及其长度
	if (!QFile::exists(strFileName) || strSectionName == NULL || strKeyName == NULL)
	{
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("文件不存在,请联系管理员！"));
		return;
	}

	QString strSection("[" + strSectionName + "]");
	QString strKey(strKeyName + "=");

	QFile readFile(strFileName);

	//打开失败，返回默认字符串及其长度
	if (!readFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("未找到或配置不正确,请联系管理员！"));
		return;
	}

	QTextStream readStream(&readFile);
	while (!readStream.atEnd())
	{
		QString line(readStream.readLine());
		if (line.indexOf(strSection) == 0)
		{
			line = readStream.readLine();
			while (line.indexOf(strKey)!=0 && line.indexOf("[")!=0 && !readStream.atEnd())
			{
				line = readStream.readLine();
				line.remove(QRegExp("\\s"));
				if (line.indexOf(strKey) == 0)
				{
					QString source(line.mid(strKey.length()));
					//lst = str.split(','); str.section(',',1,1);
					pReturnedValue = source/*.section(',', 0, 0)*/;
					//pReturnedValue = source.section(',', 0, 0);
					readFile.close();
					return;
					//return StrCopy(source, pReturnedValue, size);
				}
				
			}
		}
	}
	readFile.close();
	//未找到，返回默认值及其长度
	//return StrCopy(strDefault, pReturnedValue, size);
	return;
}


bool SqlServer::ReadSetupFile(QString filePath, connect_db_type db_type /*= mysql*/)
{
	if (db_type == mysql)
	{

		GetPrivateProfileQString(QString::fromLocal8Bit("power_mysql"), QString::fromLocal8Bit("POWHOST"), QString::fromLocal8Bit(""), m_server_mysql, MAX_PATH, filePath);
		GetPrivateProfileQString(QString::fromLocal8Bit("power_mysql"), QString::fromLocal8Bit("DATABASE"), QString::fromLocal8Bit(""), m_dataBase_mysql, MAX_PATH, filePath);
		GetPrivateProfileQString(QString::fromLocal8Bit("power_mysql"), QString::fromLocal8Bit("UID"), QString::fromLocal8Bit(""), m_sqlUserName_mysql, MAX_PATH, filePath);
		GetPrivateProfileQString(QString::fromLocal8Bit("power_mysql"), QString::fromLocal8Bit("PWD"), QString::fromLocal8Bit(""), m_sqlPassword_mysql, MAX_PATH, filePath);

		int pos = m_server_mysql.indexOf(',');
		if (pos >= 0)
		{
			m_port_mysql = m_server_mysql.section(',', 1, 1);
			m_server_mysql = m_server_mysql.section(',', 0, 0);
		}
		else
		{
			m_port_mysql = QString::fromLocal8Bit("3306");
		}
		if (m_sqlUserName_mysql .isEmpty()) {
			m_sqlUserName_mysql = QString::fromLocal8Bit("JAGUARSERVER");
		}
		if (m_sqlPassword_mysql.isEmpty()) {
			m_sqlPassword_mysql = QString::fromLocal8Bit("carl2019@HW");
		}
		if (m_server_mysql.isEmpty() || m_dataBase_mysql.isEmpty()) {
			QMessageBox::information(NULL,"",filePath + QString::fromLocal8Bit("未找到或配置不正确,请联系管理员！"));
			return false;
		}
	}
	return true;
}

SqlServer* SqlServer::Instance(connect_db_type db_type /*= mysql*/)
{
	if (instance_ == nullptr)
	{
		if (db_type == mysql)
		{
			instance_ = new SqlServer(db_type);
		}

	}
	return instance_;
}

//断开数据库
void SqlServer::CloseServer()
{
	if (IsDataBaseConnected())
	{
		m_Database.close();
	}
}


bool SqlServer::ConnectServer()
{
	m_Database = QSqlDatabase::addDatabase("QMYSQL");
	m_Database.setHostName(m_server_mysql);      //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
	m_Database.setPort(m_port_mysql.toInt());                 //连接数据库端口号，与设置一致
	m_Database.setDatabaseName(m_dataBase_mysql);      //连接数据库名，与设置一致
	m_Database.setUserName(m_sqlUserName_mysql);          //数据库用户名，与设置一致
	m_Database.setPassword(m_sqlPassword_mysql);    //数据库密码，与设置一致

	if (!m_Database.open())
	{
		//QMessageBox::information(NULL, QString::fromLocal8Bit("err"), QString::fromLocal8Bit("connect to mysql error!"));
		QMessageBox::information(NULL, QString::fromLocal8Bit("err"), m_Database.lastError().text());
		return false;
	}
	else
	{
		//QMessageBox::information(NULL, QString::fromLocal8Bit("恭喜"), QString::fromLocal8Bit("数据库连接成功！"));
		return true;
	}
}

bool SqlServer::IsDataBaseConnected()
{
	if (m_Database.isOpen())
	{
		return true;
	}
	return false;
}


bool SqlServer::GetSQLData(QString strSQL, QVector<QVector<QString>>& sqlData)
{
	QSqlQuery query(m_Database);
	//QMessageBox::information(NULL, "Title", strSql);
	//query.exec("select * FROM ack_drawing_question");
	query.exec(strSQL);
	
	QSqlRecord rec = query.record();
	int index = rec.count(); // 表一共有多少行
     // 表一共有多少列      
	if (index == 0 || query.size()<0)
	{
		QString str;
		str += QString::fromLocal8Bit("执行： ");
		str += strSQL;
		str += QString::fromLocal8Bit(" 并未查询到数据");
		//QMessageBox::information(NULL, QString::fromLocal8Bit("Title"), str);
		return false;
	}
	while (query.next())
	{
		QVector<QString> date;
		date.clear();
		for (int i = 0; i < index; i++)
		{
			
			QString QsData;
			QsData = query.value(i).toString();
			date.push_back(QsData);

		}
		sqlData.push_back(date);
	}
	return true;

}

bool SqlServer::GetSQLData(QString strSQL, QVector<QString>& sqlData)
{
	QSqlQuery query(m_Database);
	//QMessageBox::information(NULL, "Title", strSql);
	//query.exec("select * FROM ack_drawing_question");
	query.exec(strSQL);

	QSqlRecord rec = query.record();
	int index = rec.count(); // 表一共有多少行
							 // 表一共有多少列      
	if (index == 0 || query.size() < 0)
	{
		//QMessageBox::information(NULL, QString::fromLocal8Bit("Title"), QString::fromLocal8Bit("并未查询到数据"));
		return false;
	}
	while (query.next())
	{
		QVector<QString> date;
		date.clear();
		for (int i = 0; i < index; i++)
		{

			QString QsData;
			QsData = query.value(i).toString();
			sqlData.push_back(QsData);

		}
	}
	return true;
}

bool SqlServer::GetSQLData(QString strSQL, QString& firstValue)
{
	QSqlQuery query(m_Database);
	//QMessageBox::information(NULL, "Title", strSql);
	//query.exec("select * FROM ack_drawing_question");
	query.exec(strSQL);

	QSqlRecord rec = query.record();// 表一共有多少行
	int index = rec.count(); // 表一共有多少列 
							      
	if (index == 0 || query.size() <= 0)
	{
		//QMessageBox::information(NULL, QString::fromLocal8Bit("Title"), QString::fromLocal8Bit("并未查询到数据"));
		return false;
	}

	query.next();
	firstValue = query.value(0).toString();

	return true;
}

void SqlServer::RunSQL(QString strSQL)
{
	QSqlQuery query(m_Database);
	query.exec(strSQL);
}

//获取华为工号
QString SqlServer::GetHWID()
{
	if (m_hwid .isEmpty())
	{
		m_hwid = QDir::home().dirName();

	}
	return m_hwid;
}


void SqlServer::RequerySQLtableNameColValue(QString tableName, QString csIncolName,
	QString csIncolvalue, QString csOutcolvalue, 
	QVector<QString>& csResults)
{
	QSqlQuery query(m_Database);
	QString strSql;
	strSql.sprintf("SELECT %s FROM %s WHERE   (%s = '%s')", csOutcolvalue, tableName, csIncolName, csIncolvalue);

	query.exec(strSql);
	QString csText;
	while (query.next())
	{

		for (int i = 0; i < query.size(); i++)
		{

			QString QsData;
			QsData = query.value(i).toString();
			csResults.push_back(QsData);
		}
	}

}

bool SqlServer::GetUserShstate(QString& pushState, QString& checkState)
{
	pushState = "";
	checkState = "";
	QString strSql;
	strSql.sprintf("select push_state,check_state from drwNo_UserInfo where HW_ID = '%s';", GetHWID());
	QVector<QVector<QString>> data;
	GetSQLData(strSql, data);
	if (data.size() < 1)//如果SH_Shstate中没有该用户的ID,则插入该用户默认状态信息(默认都为yes)
	{
		return false;
	}
	pushState = data[0][0];
	checkState = data[0][1];
	return true;
}
