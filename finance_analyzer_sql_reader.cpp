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
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATA_HEAD = "SELECT ";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATA_TAIL_FORMAT = " FROM %s";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_BETWEEN_FORMAT = " WHERE date BETWEEN %s AND %s";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_GREATER_THAN_FORMAT = " WHERE date > %s";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_LESS_THAN_FORMAT = " WHERE date < %s";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT = " WHERE month(date) BETWEEN '%d' AND '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_GREATER_THAN_FORMAT = " WHERE month(date) > '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_LESS_THAN_FORMAT = " WHERE month(date) < '%d'";

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
	WRITE_FORMAT_DEBUG("Select the MySQL database: %s", database_name.c_str());

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

unsigned short FinanceAnalyzerSqlReader::select_data(const string table_name, const string cmd_table_field, const PTIME_RANGE_CFG time_range_cfg)
{
// Check if the connection is established
	if (connection == NULL)
	{
		WRITE_ERROR("The connection is NOT established");
		return RET_FAILURE_INCORRECT_OPERATION;
	}

// Checks to see if the connection to the MySQL server is still alive. If it is not, the client will attempt to reconnect automatically.
// This function returns zero if the connection is alive and non-zero in the case of an error.
	if (mysql_ping(connection))
	{
		WRITE_INFO("The connection is NOT alive. Attempt to reconnect it......");
// Select the database
		if (mysql_select_db(connection, database_name.c_str()))
		{
			WRITE_FORMAT_ERROR("mysql_select_db() fails, due to: %s", mysql_error(connection));
			return RET_FAILURE_MYSQL;
		}
	}
// Read data from database
// Generate the SQL command for querying
	char _format_cmd_select_data_tail[32];
	snprintf(_format_cmd_select_data_tail, 32, FORMAT_CMD_SELECT_DATA_TAIL_FORMAT, table_name.c_str());
	string cmd_select_data = FORMAT_CMD_SELECT_DATA_HEAD + cmd_table_field + string(_format_cmd_select_data_tail);
	if (time_range_cfg != NULL)
	{
		char _cmd_search_rule[256];
		bool need_add_rule = true;
		if (time_range_cfg->is_month_type())
		{
			if (time_range_cfg->get_start_time() != NULL && time_range_cfg->get_end_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT, time_range_cfg->get_start_time()->get_month(), time_range_cfg->get_end_time()->get_month());
			else if (time_range_cfg->get_start_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_GREATER_THAN_FORMAT, time_range_cfg->get_start_time()->get_month());
			else if (time_range_cfg->get_end_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_LESS_THAN_FORMAT, time_range_cfg->get_end_time()->get_month());
			else
				need_add_rule = false;
		}
		else
		{
			if (time_range_cfg->get_start_time() != NULL && time_range_cfg->get_end_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_BETWEEN_FORMAT, time_range_cfg->get_start_time()->to_string(), time_range_cfg->get_end_time()->to_string());
			else if (time_range_cfg->get_start_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_GREATER_THAN_FORMAT, time_range_cfg->get_start_time()->to_string());
			else if (time_range_cfg->get_end_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_LESS_THAN_FORMAT, time_range_cfg->get_end_time()->to_string());
			else
				need_add_rule = false;
		}
		if (need_add_rule)
			cmd_select_data += string(_cmd_search_rule);
	}
//	string format_cmd = FORMAT_CMD_SELECT_DATA_HEAD + cmd_table_field + FORMAT_CMD_SELECT_DATA_TAIL_FORMAT + string(" WHERE date BETWEEN 2004-04-09 AND 2004-09-04");
//	snprintf(cmd_buf, CMD_BUF_SIZE, format_cmd.c_str(), table_name.c_str());
	WRITE_FORMAT_DEBUG("select data by command: %s",cmd_select_data.c_str());
	if(mysql_query(connection, cmd_select_data.c_str()) != 0)
	{
		WRITE_FORMAT_ERROR("mysql_query() fails, due to: %s", mysql_error(connection));
		return RET_FAILURE_MYSQL;
	}

	MYSQL_RES *result = mysql_store_result(connection);
	unsigned int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	unsigned int i;
	while ((row = mysql_fetch_row(result)))
	{
//		unsigned long *lengths;
//		lengths = mysql_fetch_lengths(result);
		for(i = 0; i < num_fields; i++)
		{
			printf("%s ", row[i]);
		}
		printf("\n");
	}

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerSqlReader::select_data(const std::string table_name, const std::string cmd_table_field){return select_data(table_name, cmd_table_field, NULL);}
unsigned short FinanceAnalyzerSqlReader::select_data(const std::string table_name, const PTIME_RANGE_CFG time_range_cfg){return select_data(table_name, string("*"), time_range_cfg);}
unsigned short FinanceAnalyzerSqlReader::select_data(const std::string table_name){return select_data(table_name, string("*"), NULL);}


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
