#pragma once
#include <QString>
#include <qtsql/QSqlDatabase>
#include <QVector>
class SqlServer
{
public:
	enum connect_db_type
	{
		sqlserver = 0,
		mysql = 1,
	};


	static bool ReadSetupFile( QString filePath,  connect_db_type db_type = mysql);//读取数据库配置文件energy_setup.ini,在程序启动时只需调用一次即可
	static SqlServer* Instance( connect_db_type db_type = mysql);//类的静态实例函数,单例模式适用于mysql,若连接sqlserver,不要用单例模式而是使用声明对象方式	
	bool ConnectServer();//连接数据库
	bool IsDataBaseConnected();//判断数据库是否连接
	void CloseServer();//断开数据库
	bool GetSQLData(QString strSQL, QVector<QVector<QString>>& sqlData);//获取sql语句执行后的数据
	bool GetSQLData(QString strSQL, QVector<QString>& sqlData);//获取sql语句执行后的数据
	bool GetSQLData(QString strSQL, QString& firstValue);//获取sql语句执行后的首值(第1行第1列)
	void RunSQL(QString strSQL);
public:
//	//获取指定表的指定列值            
	//参数1  表名称  
	//参数2 查询的列名称      
	//参数3查询的列值       
	//参数4输出的列       
	//参数5输出值(如果是多个值)
	void RequerySQLtableNameColValue(QString tableName, QString csIncolName, 
		QString csIncolvalue, QString csOutcolvalue,
		QVector<QString>& csResults);
	QString GetHWID();//获取华为工号
	bool GetUserShstate(QString& pushState,QString& checkState);
//
	static SqlServer* instance_;//类的静态实例
private:	
	SqlServer(connect_db_type db_type = mysql);
	~SqlServer();
	
	static QString m_server_mysql;//mysql服务器名(IP)
	static QString m_dataBase_mysql;//mysql数据库名
	static QString m_sqlUserName_mysql;//mysql用户名
	static QString m_sqlPassword_mysql;//mysql密码
	static QString m_port_mysql;//mysql端口号
	QString m_hwid;//华为工号
	static QSqlDatabase m_Database;
	connect_db_type m_dbType;//连接的数据库类型

};
