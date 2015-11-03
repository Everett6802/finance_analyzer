//#include <fcntl.h>
#include "finance_analyzer_sql_reader.h"

/*
 * Go to the following links to see more detailed info:
 * http://www.cs.wichita.edu/~chang/lecture/cs742/program/how-mysql-c-api.html
 * http://docstore.mik.ua/orelly/linux/sql/ch19_01.htm
 */


using namespace std;

const char* FinanceAnalyzerSqlReader::MYSQL_SERVER = "localhost";
const char* FinanceAnalyzerSqlReader::MYSQL_USERNAME = "root";
const char* FinanceAnalyzerSqlReader::MYSQL_PASSWORD = "lab4man1";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_CREATE_DATABASE = "CREATE DATABASE %s";
//const char* FinanceAnalyzerSqlReader::format_cmd_create_table = "CREATE TABLE sql%s (date VARCHAR(16), time VARCHAR(16), severity INT, data VARCHAR(512))";
//const char* FinanceAnalyzerSqlReader::format_cmd_insert_into_table = "INSERT INTO sql%s VALUES(\"%s\", \"%s\", %d, \"%s\")";
DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerSqlReader::FinanceAnalyzerSqlReader() :
	connection(NULL)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerSqlReader::~FinanceAnalyzerSqlReader()
{
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerSqlReader::try_connect_mysql(const string database)
{
	WRITE_DEBUG("Initialize the parameters connected to the MySQL database server");
	connection = mysql_init(NULL); // 初始化数据库连接变量
	if(connection == NULL)
	{
		WRITE_FORMAT_ERROR("mysql_init() fails, due to: %s", mysql_error(connection));
		return RET_FAILURE_MYSQL;
	}

	database_name = database;
	WRITE_DEBUG("Try to connect to the MySQL database server...");
// 函数mysql_real_connect建立一个数据库连接，成功返回MYSQL*连接句柄，失败返回NULL
	if(mysql_real_connect(connection, MYSQL_SERVER, MYSQL_USERNAME, MYSQL_PASSWORD, database_name.c_str(), 0, NULL, 0) == NULL)
	{
// The database does NOT exist !!! Try to create one
		WRITE_FORMAT_DEBUG("The %s database does NOT exist, create a NEW one", database_name.c_str());
// mysql_create_db() has been deprecated in the newer releases of MySQL. MySQL now supports the CREATE DATABASE SQL statement.
// This should be used, via the mysql_query function, instead
		if(mysql_real_connect(connection, MYSQL_SERVER, MYSQL_USERNAME, MYSQL_PASSWORD, NULL, 0, NULL, 0) == NULL)
		{
			WRITE_FORMAT_ERROR("mysql_real_connect() fails, due to: %s", mysql_error(connection));
			return RET_FAILURE_MYSQL;
		}

		snprintf(cmd_buf, CMD_BUF_SIZE, FORMAT_CMD_CREATE_DATABASE, database_name.c_str());
		WRITE_FORMAT_DEBUG("Try to create database[%s] by command: %s", database_name.c_str(), cmd_buf);
		if(mysql_query(connection, cmd_buf) != 0)
		{
			WRITE_FORMAT_ERROR("mysql_query() fails, due to: %s", mysql_error(connection));
			return RET_FAILURE_MYSQL;
		}
		WRITE_FORMAT_DEBUG("The %s database is created", database_name.c_str());
	}
	WRITE_DEBUG("Try to connect to the MySQL database server...... Successfully");

// Select the database
	if (mysql_select_db(connection, database_name.c_str()))
	{
		WRITE_FORMAT_ERROR("mysql_select_db() fails, due to: %s", mysql_error(connection));
		return RET_FAILURE_MYSQL;
	}

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerSqlReader::disconnect_mysql()
{
	WRITE_DEBUG("Disconnect from the MySQL database server...");
	if (connection != NULL)
	{
		mysql_close(connection);
		connection = NULL;
	}
	database_name.clear();

	return RET_SUCCESS;
}

//unsigned short FinanceAnalyzerSqlReader::parse_config_param(const char* param_title, const char* param_content)
//{
//	if (param_title == NULL || param_content == NULL)
//	{
//		WRITE_ERR("Invalid argument: param_title/param_content");
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	static const char* title[] = {"server", "username", "password", "database"};
//	static int title_len = sizeof title / sizeof title[0];
//
//	unsigned short ret = RET_SUCCESS;
//	bool found = false;
//	for (int index = 0 ; index < title_len ; index++)
//	{
//		if (strcmp(title[index], param_title) == 0)
//		{
//			int param_content_len = strlen(param_content);
//			char* param_member_variable = NULL;
//			switch(index)
//			{
//			case 0:
//				param_member_variable = server;
//				break;
//			case 1:
//				param_member_variable = username;
//				break;
//			case 2:
//				param_member_variable = password;
//				break;
//			case 3:
//				param_member_variable = database;
//				break;
//			}
//
//			if (param_member_variable != NULL)
//			{
//				memset(param_member_variable, 0x0, sizeof(char) * RET_STRING_SIZE);
//				memcpy(param_member_variable, param_content, param_content_len);
//				found = true;
//				WRITE_DEBUG_FORMAT(RET_STRING_SIZE, "Update parameter: %s=%s", param_title, param_content);
//			}
//			else
//			{
//				WRITE_ERR_FORMAT(RET_STRING_SIZE, "Incorrect parameter: %s=%s", param_title, param_content);
//				ret = RET_FAILURE_INVALID_ARGUMENT;
//			}
//			break;
//		}
//	}
//// If the title is NOT found...
//	if (!found)
//	{
//		WRITE_ERR_FORMAT(RET_STRING_SIZE, "Incorrect parameter, fail to find the title: %s", param_title);
//		ret = RET_FAILURE_INVALID_ARGUMENT;
//	}
//
//	return ret;
//}
//
//unsigned short FinanceAnalyzerSqlReader::open_device()
//{
//// Check if the connection is established
//	if (connection == NULL)
//	{
//		WRITE_ERR_FORMAT(RET_STRING_SIZE, "Thread[%s]=> The connection is NOT established", facility_name);
//		return RET_FAILURE_MYSQL;
//	}
//
//// Checks to see if the connection to the MySQL server is still alive. If it is not, the client will attempt to reconnect automatically.
//// This function returns zero if the connection is alive and nonzero in the case of an error.
//	if (mysql_ping(connection))
//	{
//		WRITE_INFO_FORMAT(RET_STRING_SIZE, "Thread[%s]=> The connection is NOT alive.Attempt to reconnect it......", facility_name);
//// Select the database
//		if (mysql_select_db(connection, database))
//		{
//			WRITE_ERR_FORMAT(RET_STRING_SIZE, "Thread[%s]=> mysql_select_db() fails, due to: %s", facility_name, mysql_error(connection));
//			return RET_FAILURE_MYSQL;
//		}
//	}
//
//	if (!table_created)
//	{
//// Get the current time
//		generate_current_time_string(current_time_string);
//
//// Create the table in the database...
//		snprintf(cmd_buf, RET_LONG_STRING_SIZE, format_cmd_create_table, current_time_string);
//		WRITE_DEBUG_FORMAT(RET_LONG_STRING_SIZE, "Thread[%s]=> Try to create table[sql%s] by command: %s", facility_name, current_time_string, cmd_buf);
//		if(mysql_query(connection, cmd_buf) != 0)
//		{
//			int error = mysql_errno(connection);
//			if (error != ER_TABLE_EXISTS_ERROR)
//			{
//				WRITE_ERR_FORMAT(RET_STRING_SIZE, "Thread[%s]=> mysql_query() fails, due to: %d, %s", facility_name, error, mysql_error(connection));
//				return RET_FAILURE_MYSQL;
//			}
//			else
//				WRITE_DEBUG_FORMAT(RET_STRING_SIZE, "Thread[%s]=> The sql%s has already existed", facility_name, current_time_string);
//		}
//		table_created = true;
//	}
//
//	return RET_SUCCESS;
//}
//
//unsigned short FinanceAnalyzerSqlReader::close_device()
//{
//// Close the MySQL
////	mysql_close(connection);
//
//	return RET_SUCCESS;
//}
//
//unsigned short FinanceAnalyzerSqlReader::initialize(const char* config_path, void* config)
//{
//	WRITE_DEBUG("Initialize the FinanceAnalyzerSqlReader object......");
//
//// Parse the config file first
//	unsigned short ret = parse_config(config_path, "sql");
//	if (CHECK_FAILURE(ret))
//		return ret;
//
//// Create the connection to the MySQL server
//	ret = try_connect_mysql();
//	if (CHECK_FAILURE(ret))
//		return ret;
//	//	mysql_close(connection);
//	//	connection = NULL;
//
//	if (CHECK_FAILURE(ret))
//		return ret;
//
//	return RET_SUCCESS;
//}
//
//unsigned short FinanceAnalyzerSqlReader::deinitialize()
//{
//	WRITE_DEBUG("DeInitialize the FinanceAnalyzerSqlReader object......");
//	WRITE_DEBUG("Release the parameters connected to the MySQL database server");
//	if(connection != NULL)  // 关闭数据库连接
//	{
//		mysql_close(connection);
//		connection = NULL;
//	}
//
//	return RET_SUCCESS;
//}
//
//unsigned short FinanceAnalyzerSqlReader::write_msg(PMSG_CFG msg_cfg)
//{
//// Write the message into SQL database
//	snprintf(cmd_buf, RET_LONG_STRING_SIZE, format_cmd_insert_into_table, current_time_string, msg_cfg->date_str, msg_cfg->time_str, msg_cfg->severity, msg_cfg->data);
//	WRITE_DEBUG_FORMAT(RET_LONG_STRING_SIZE, "Thread[%s]=> Try to Write the message[%s] to MySQL by command: %s", facility_name, msg_cfg->to_string(), cmd_buf);
//	if(mysql_query(connection, cmd_buf) != 0)
//	{
//		WRITE_ERR_FORMAT(RET_STRING_SIZE, "Thread[%s]=> mysql_query() fails, due to: %s", facility_name, mysql_error(connection));
//		return RET_FAILURE_MYSQL;
//	}
//
//	return RET_SUCCESS;
//}
