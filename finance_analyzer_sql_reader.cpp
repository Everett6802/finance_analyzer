#include <assert.h>
#include <stdexcept>
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
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_BETWEEN_FORMAT = " WHERE date BETWEEN '%s' AND '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_GREATER_THAN_FORMAT = " WHERE date > '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_LESS_THAN_FORMAT = " WHERE date < '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_EQUAL_FORMAT = " WHERE date = '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT = " WHERE month(date) BETWEEN '%d' AND '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_GREATER_THAN_FORMAT = " WHERE month(date) > '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_LESS_THAN_FORMAT = " WHERE month(date) < '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_EQUAL_FORMAT = " WHERE month(date) = '%s'";

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

unsigned short FinanceAnalyzerSqlReader::get_sql_field_command(int source_index, const DEQUE_INT& query_field, string& field_cmd)
{
	if (query_field.empty())
		throw invalid_argument("The query should NOT be empty");
//	string field_cmd;
// Select all the fields in the table
	if ((int)query_field.size() == FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index] - 1) // Caution: Don't include the "date" field
		field_cmd = string("*");
	else
	{
// Assemble the MySQL command of the designated field
// The "date" field is a must
		char field_buf[16];
		snprintf(field_buf, 16, "%s", MYSQL_DATE_FILED_NAME);
		field_cmd = string(field_buf);
		int query_field_size = query_field.size();
		for(int field_index = 0 ; field_index < query_field_size ; field_index++)
		{
			snprintf(field_buf, 16, ",%s%d", MYSQL_FILED_NAME_BASE, query_field[field_index]);
			field_cmd += string(field_buf);
		}
	}

	return RET_SUCCESS;
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

unsigned short FinanceAnalyzerSqlReader::select_data(
		int source_index,
		const std::string& table_name,
		const std::string& cmd_table_field,
		const PDEQUE_INT query_field,
		const PTIME_RANGE_CFG time_range_cfg,
		PRESULT_SET result_cfg
	)
{
	assert(query_field != NULL && !query_field->empty() && "result_cfg should NOT be NULL/Empty");
	assert(result_cfg != NULL && "result_cfg should NOT be NULL");
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
			{
				if (time_range_cfg->is_single_time())
					snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_EQUAL_FORMAT, time_range_cfg->get_start_time()->to_string());
				else
					snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT, time_range_cfg->get_start_time()->get_month(), time_range_cfg->get_end_time()->get_month());
			}
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
			{
				if (time_range_cfg->is_single_time())
					snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_EQUAL_FORMAT, time_range_cfg->get_start_time()->to_string());
				else
					snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_BETWEEN_FORMAT, time_range_cfg->get_start_time()->to_string(), time_range_cfg->get_end_time()->to_string());
			}
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
	WRITE_FORMAT_DEBUG("select data by command: %s",cmd_select_data.c_str());
	if(mysql_query(connection, cmd_select_data.c_str()) != 0)
	{
		WRITE_FORMAT_ERROR("mysql_query() fails, due to: %s", mysql_error(connection));
		return RET_FAILURE_MYSQL;
	}

// Count the amount of the field including the date field
	int expected_data_dimension = (int)query_field->size() + 1;
//	if ((*query_field)[0] == -1)
//		expected_data_dimension = FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index];
//	else
//		expected_data_dimension = (int)query_field->size() + 1;
// Store the query result into a self-defined data structure
	MYSQL_RES *result = mysql_store_result(connection);
	int actual_data_dimension = mysql_num_fields(result);
	MYSQL_ROW row;
	if (actual_data_dimension != expected_data_dimension) // Since the 'date' field is NOT in the query_field
	{
		WRITE_FORMAT_ERROR("actual_data_dimension[%d] is NOT identical to expected_data_dimension[%d]", actual_data_dimension, expected_data_dimension);
		return RET_FAILURE_UNKNOWN;
	}

	unsigned short ret = RET_SUCCESS;
// Fetch the data in each row
	while ((row = mysql_fetch_row(result)))
	{
//		unsigned long *lengths = mysql_fetch_lengths(result);
// Parse each row
// Set the date
		ret = result_cfg->set_date(row[0]);
		if (CHECK_FAILURE(ret))
			return ret;
// Set the data in each field
		for(int field_index = 1 ; field_index < actual_data_dimension ; field_index++)
		{
			ret = result_cfg->set_data(source_index, (*query_field)[field_index - 1], row[field_index]);
			if (CHECK_FAILURE(ret))
				return ret;
//			printf("%s ", row[i]);
		}
//		printf("\n");
	}

	return RET_SUCCESS;
}
