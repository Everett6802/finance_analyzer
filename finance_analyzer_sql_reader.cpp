#include <assert.h>
#include <stdexcept>
#include <set>
#include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_database_time_range.h"
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
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_GREATER_EQUAL_THAN_FORMAT = " WHERE date >= '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_LESS_EQUAL_THAN_FORMAT = " WHERE date <= '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_DATE_RULE_EQUAL_FORMAT = " WHERE date = '%s'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT = " WHERE month(date) BETWEEN '%d' AND '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_GREATER_EQUAL_THAN_FORMAT = " WHERE month(date) >= '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_LESS_EQUAL_THAN_FORMAT = " WHERE month(date) <= '%d'";
const char* FinanceAnalyzerSqlReader::FORMAT_CMD_SELECT_MONTH_RULE_EQUAL_FORMAT = " WHERE month(date) = '%s'";

//const char* FinanceAnalyzerSqlReader::format_cmd_create_table = "CREATE TABLE sql%s (date VARCHAR(16), time VARCHAR(16), severity INT, data VARCHAR(512))";
//const char* FinanceAnalyzerSqlReader::format_cmd_insert_into_table = "INSERT INTO sql%s VALUES(\"%s\", \"%s\", %d, \"%s\")";
DECLARE_MSG_DUMPER_PARAM()

unsigned short FinanceAnalyzerSqlReader::get_sql_field_command(int source_type_index, const INT_DEQUE& query_field, string& field_cmd)
{
	if (query_field.empty())
		throw invalid_argument("The query should NOT be empty");
//	string field_cmd;
// Select all the fields in the table
	if ((int)query_field.size() == FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_type_index] - 1) // Caution: Don't include the "date" field
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

unsigned short FinanceAnalyzerSqlReader::query_from_tables(
	const PTIME_RANGE_CFG restricted_time_range_cfg, 
	const PQUERY_SET query_set,
	const std::string& company_code_number,  // For stock mode only, ignored in market mode
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, 
	PRESULT_SET result_set
	)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE()
	if (is_stock_mode())
	{
		if (!company_profile->is_company_exist(company_code_number))
		{
			WRITE_FORMAT_ERROR("The company[%s] does NOT exist", company_code_number.c_str());
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}

	unsigned short ret = RET_SUCCESS;
	string table_name;
	for (QuerySet::const_iterator iter = query_set->begin() ; iter != query_set->end() ; ++iter)
	{
		// const INT_DEQUE& query_field = (*query_set)[source_index];
		const INT_DEQUE& query_field = (*iter);
		assert(!query_field.empty() && "query_field should NOT be empty");
		int source_type_index = iter.get_first();
// Add to the result set
		ret = result_set->add_set(source_type_index, query_field);
		if (CHECK_FAILURE(ret))
			return ret;
// Generate the field command
		string field_cmd = string("");
		FinanceAnalyzerSqlReader::get_sql_field_command(source_type_index, query_field, field_cmd);
// Search for each table
		table_name = string(FINANCE_TABLE_NAME_LIST[source_type_index]);
		if (is_stock_mode())
			table_name = company_code_number + table_name;
		ret = finance_analyzer_sql_reader->select_data(
			source_type_index, 
			table_name, 
			field_cmd, 
			(const PINT_DEQUE)&query_field, 
			restricted_time_range_cfg, 
			result_set
		);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerSqlReader::query_market(
	const PQUERY_SET query_set, 
	const PTIME_RANGE_CFG time_range_cfg, 
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, 
	PRESULT_SET_MAP result_set_map
	)
{
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	DECLARE_AND_IMPLEMENT_STATIC_DATABASE_TIME_RANGE()
	static string company_code_number_dummy("xxxx");

	assert(finance_analyzer_sql_reader != NULL && query_set != NULL && time_range_cfg != NULL && result_set_map != NULL);
	if (!query_set->is_add_query_done())
	{
		WRITE_ERROR("The setting of query data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	if (time_range_cfg->get_start_time() == NULL || time_range_cfg->get_end_time() == NULL)
	{
		WRITE_ERROR("The start/end time in time_range_cfg should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short ret = RET_SUCCESS;
// Connect to the database
	ret = finance_analyzer_sql_reader->try_connect_mysql(FINANCE_DATABASE_MARKET_NAME);
	if (CHECK_FAILURE(ret))
		return ret;
// Check the boundary of each database
	SmartPointer<TimeRangeCfg> sp_restricted_time_range_cfg(new TimeRangeCfg(*time_range_cfg));
	WRITE_FORMAT_DEBUG("The original search time range: %s", sp_restricted_time_range_cfg->to_string());
	ret = database_time_range->restrict_time_range(query_set, sp_restricted_time_range_cfg.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;
	WRITE_FORMAT_DEBUG("The new search time range: %s", sp_restricted_time_range_cfg->to_string());

	ResultSetDataUnit result_set_data_unit = result_set_map->get_data_unit();
	switch (result_set_data_unit)
	{
		case ResultSetDataUnit_NoSourceType:
		{
// Query data from MySQL
			PRESULT_SET result_set = new ResultSet();
			if (result_set == NULL)
			{
				WRITE_ERROR("Fail to allocate memory: result_set");
				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
				goto OUT;
			}
// Query the data from each table
			ret = query_from_tables(
				sp_restricted_time_range_cfg.get_instance(), 
				query_set,
				company_code_number_dummy,  // For stock mode only, ignored in market mode
				finance_analyzer_sql_reader, 
				result_set
			);
			if (CHECK_FAILURE(ret))
				goto OUT;
			ret = result_set_map->register_result_set(NO_SOURCE_TYPE_MARKET_SOURCE_KEY_VALUE, result_set);
			if (CHECK_FAILURE(ret))
				goto OUT;
// Check the result
			result_set->switch_to_check_date_mode();
			ret = result_set->check_data();
			if (CHECK_FAILURE(ret))
				goto OUT;
		}
		break;
		case ResultSetDataUnit_SourceType:
		{
			QuerySet::const_iterator iter = query_set->begin();			
			while (iter != query_set->end())
			{
				int source_type_index = iter.get_first();
				PQUERY_SET query_sub_set = NULL;
				// const PINT_DEQUE field_index_deque = iter.get_second();
// Initialize the sub query set
				ret = query_set->get_query_sub_set(source_type_index, &query_sub_set);
				if (CHECK_FAILURE(ret))
					goto OUT;
				SmartPointer<QuerySet> sp_query_sub_set;
				sp_query_sub_set.set_new(query_sub_set);
				sp_query_sub_set->add_query_done();
// Query data from MySQL
				PRESULT_SET result_set = new ResultSet();
				if (result_set == NULL)
				{
					WRITE_ERROR("Fail to allocate memory: result_set");
					ret = RET_FAILURE_INSUFFICIENT_MEMORY;
					goto OUT;
				}
// Query the data from each table
				ret = query_from_tables(
					sp_restricted_time_range_cfg.get_instance(), 
					sp_query_sub_set.get_instance(),
					company_code_number_dummy,  // For stock mode only, ignored in market mode
					finance_analyzer_sql_reader, 
					result_set
				);
				if (CHECK_FAILURE(ret))
					return ret;
				int source_key = get_source_key(source_type_index);
				ret = result_set_map->register_result_set(source_key, result_set);
				if (CHECK_FAILURE(ret))
					goto OUT;
				++iter;
			}
		}
		break;
		default:
		{
			WRITE_FORMAT_ERROR("Unknown result set data unit: %d", result_set_data_unit);
			ret = RET_FAILURE_INVALID_ARGUMENT;
			goto OUT;
		}
		break;
	}

// 	for (MarketQuerySet::iterator iter = query_set.begin() ; iter < query_set.end() ; iter++)
// 	// for (int source_index = 0 ; source_index < FinanceSourceSize ; source_index++)
// 	{
// 		// const INT_DEQUE& query_field = (*query_set)[source_index];
// 		const INT_DEQUE& query_field = (*iter);
// 		assert(!query_field.empty() && "query_field should NOT be empty");
// 		int source_index = iter.get_first();
// 		// if (query_field.empty())
// 		// 	continue;
// // Add to the result set
// 		ret = result_set->add_set(source_index, query_field);
// 		if (CHECK_FAILURE(ret))
// 			return ret;
// // Generate the field command
// 		string field_cmd = string("");
// 		FinanceAnalyzerSqlReader::get_sql_field_command(source_index, query_field, field_cmd);
// // Query the data in each table
// 		int start_year = sp_restricted_time_range_cfg->get_start_time()->get_year();
// 		int end_year = sp_restricted_time_range_cfg->get_end_time()->get_year();
// // Search for each table
// 		ret = finance_analyzer_sql_reader->select_data(source_index, string(table_name), field_cmd, (const PINT_DEQUE)&query_field, time_range_cfg_in_year, result_set);
// 		if (CHECK_FAILURE(ret))
// 			return ret;
// 	}

// No need, keep them in the memory until the process is dead
	// RELEASE_DATABASE_TIME_RANGE()
	// RELEASE_MSG_DUMPER()
// Disconnect from the database
OUT:
	finance_analyzer_sql_reader->disconnect_mysql();

	return ret;
}

// unsigned short FinanceAnalyzerSqlReader::query_market(
// 	const PQUERY_SET query_set, 
// 	const PTIME_RANGE_CFG time_range_cfg, 
// 	PRESULT_SET_MAP result_set_map
// 	)
// {
// 	FinanceAnalyzerSqlReader finance_analyzer_sql_reader;
// 	return query_market(query_set, time_range_cfg, &finance_analyzer_sql_reader, result_set_map);
// }

unsigned short FinanceAnalyzerSqlReader::query_stock(
	const PQUERY_SET query_set, 
	const PTIME_RANGE_CFG time_range_cfg, 
	const PCOMPANY_GROUP_SET company_group_set,
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, 
	PRESULT_SET_MAP result_set_map
	)
{
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	DECLARE_AND_IMPLEMENT_STATIC_DATABASE_TIME_RANGE()

	assert(finance_analyzer_sql_reader != NULL && query_set != NULL && time_range_cfg != NULL && company_group_set != NULL && result_set_map != NULL);
	if (!query_set->is_add_query_done())
	{
		WRITE_ERROR("The setting of query data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	if (time_range_cfg->get_start_time() == NULL || time_range_cfg->get_end_time() == NULL)
	{
		WRITE_ERROR("The start/end time in time_range_cfg should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if (!company_group_set->is_add_company_done())
	{
		WRITE_ERROR("The setting of company group data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	// if (time_range_cfg->is_month_type())
	// {
	// 	WRITE_ERROR("The time format of time_range_cfg should be Day type");
	// 	return RET_FAILURE_INVALID_ARGUMENT;
	// }
	unsigned short ret = RET_SUCCESS;
	bool init_query_sub_set_array = false; // For ResultSetDataUnit_SourceType only
	SmartPointer<QuerySet> sp_query_sub_set_array[STOCK_SOURCE_TYPE_INDEX_LENGTH]; // For ResultSetDataUnit_SourceType only

	static char database_stock_name[32];
	ResultSetDataUnit result_set_data_unit = result_set_map->get_data_unit();
	// CompanyGroupSet& company_group_set = query_set->get_company_group_set();
	for(CompanyGroupSet::const_iterator iter = company_group_set->begin() ; iter != company_group_set->end() ; ++iter)
	{
		int company_group_number = iter.get_first();
// Connect to the database
		snprintf(database_stock_name, 32, FINANCE_DATABASE_STOCK_NAME_FORMAT, company_group_number);
		ret = finance_analyzer_sql_reader->try_connect_mysql(string(database_stock_name));
		if (CHECK_FAILURE(ret))
			return ret;
		const STRING_DEQUE& company_code_number_deque = *iter;
		for(STRING_DEQUE_CONST_ITER iter = company_code_number_deque.begin() ; iter != company_code_number_deque.end() ; ++iter)
		{
			string company_code_number = (string)*iter;
// Check the boundary of each database
			SmartPointer<TimeRangeCfg> sp_restricted_time_range_cfg(new TimeRangeCfg(*time_range_cfg));
			WRITE_FORMAT_DEBUG("The original search time range: %s", sp_restricted_time_range_cfg->to_string());
			ret = database_time_range->restrict_time_range(query_set, sp_restricted_time_range_cfg.get_instance());
			if (CHECK_FAILURE(ret))
				return ret;
			WRITE_FORMAT_DEBUG("The new search time range: %s", sp_restricted_time_range_cfg->to_string());
// Query the data from each table
			switch (result_set_data_unit)
			{
				case ResultSetDataUnit_NoSourceType:
				{
					PRESULT_SET result_set = new ResultSet();
					if (result_set == NULL)
					{
						WRITE_FORMAT_ERROR("Fail to allocate memory: result_set while reading %s table", company_code_number.c_str());
						ret = RET_FAILURE_INSUFFICIENT_MEMORY;
						goto OUT;
					}
					ret = query_from_tables(
						sp_restricted_time_range_cfg.get_instance(), 
						query_set,
						company_code_number,  // For stock mode only, ignored in market mode
						finance_analyzer_sql_reader, 
						result_set
					);
					if (CHECK_FAILURE(ret))
						goto OUT;
// Check the result
					result_set->switch_to_check_date_mode();
					ret = result_set->check_data();
					if (CHECK_FAILURE(ret))
						goto OUT;
					int source_key = get_source_key(company_group_number, company_code_number);
// Add the result into group
				 	ret = result_set_map->register_result_set(source_key, result_set);
					if (CHECK_FAILURE(ret))
						goto OUT;
				}
				break;
				case ResultSetDataUnit_SourceType:
				{
// Initialize the sub query set for the first time
					if (!init_query_sub_set_array)
					{
						QuerySet::const_iterator iter = query_set->begin();
						while (iter != query_set->end())
						{
							int source_type_index = iter.get_first();
							int source_type_revised_index = source_type_index - FinanceSource_StockStart;
							// const PINT_DEQUE field_index_deque = iter.get_second();
							PQUERY_SET query_sub_set = NULL;
							ret = query_set->get_query_sub_set(source_type_index, &query_sub_set);
							if (CHECK_FAILURE(ret))
								goto OUT;
							sp_query_sub_set_array[source_type_revised_index].set_new(query_sub_set);
							sp_query_sub_set_array[source_type_revised_index]->add_query_done();
						}
						init_query_sub_set_array = true;
					}
					QuerySet::const_iterator iter = query_set->begin();
// Query data from MySQL
					while (iter != query_set->end())
					{
						int source_type_index = iter.get_first();
						int source_type_revised_index = source_type_index - FinanceSource_StockStart;
						PRESULT_SET result_set = new ResultSet();
						if (result_set == NULL)
						{
							WRITE_ERROR("Fail to allocate memory: result_set");
							ret = RET_FAILURE_INSUFFICIENT_MEMORY;
							goto OUT;
						}
// Query the data from each table
						ret = query_from_tables(
							sp_restricted_time_range_cfg.get_instance(), 
							sp_query_sub_set_array[source_type_revised_index].get_instance(),
							company_code_number,  // For stock mode only, ignored in market mode
							finance_analyzer_sql_reader, 
							result_set
						);
						if (CHECK_FAILURE(ret))
							goto OUT;
						int source_key = get_source_key(source_type_index, company_code_number, company_group_number);
						ret = result_set_map->register_result_set(source_key, result_set);
						if (CHECK_FAILURE(ret))
							goto OUT;
						++iter;
					}
				}
				break;
				default:
				{
					WRITE_FORMAT_ERROR("Unknown result set data unit: %d", result_set_data_unit);
					ret = RET_FAILURE_INVALID_ARGUMENT;
					goto OUT;
				}
				break;
			}
// Disconnect from the database
OUT:
			finance_analyzer_sql_reader->disconnect_mysql();
			if (CHECK_FAILURE(ret))
				return ret;
		}
	}

	return ret;
}

// unsigned short FinanceAnalyzerSqlReader::query_stock(
// 	const PQUERY_SET query_set, 
// 	const PTIME_RANGE_CFG time_range_cfg,
// 	const PCOMPANY_GROUP_SET company_group_set, 
// 	PRESULT_SET_MAP result_set_map
// 	)
// {
// 	FinanceAnalyzerSqlReader finance_analyzer_sql_reader;
// 	return query_stock(query_set, time_range_cfg, company_group_set, &finance_analyzer_sql_reader, result_set_map);	
// }

unsigned short FinanceAnalyzerSqlReader::query(
	const PSEARCH_RULE_SET search_rule_set,
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, 
	PRESULT_SET_MAP result_set_map
	)
{
	assert(finance_analyzer_sql_reader != NULL && search_rule_set != NULL && result_set_map != NULL);
	unsigned short ret = RET_SUCCESS;
	if (search_rule_set->get_finance_mode() == FinanceAnalysis_Market)
		ret = query_market(search_rule_set->get_query_rule(), search_rule_set->get_time_rule(), finance_analyzer_sql_reader, result_set_map);
	else if (search_rule_set->get_finance_mode() == FinanceAnalysis_Stock)
		ret = query_stock(search_rule_set->get_query_rule(), search_rule_set->get_time_rule(), search_rule_set->get_company_rule(), finance_analyzer_sql_reader, result_set_map);
	else
	{
		ret = RET_FAILURE_INVALID_ARGUMENT;
		STATIC_WRITE_FORMAT_ERROR("query() fails, due to: %s", get_ret_description(ret));
	}
	return ret;
}

unsigned short FinanceAnalyzerSqlReader::query(
	const PSEARCH_RULE_SET search_rule_set, 
	PRESULT_SET_MAP result_set_map
	)
{
	FinanceAnalyzerSqlReader finance_analyzer_sql_reader;
	return query(search_rule_set, &finance_analyzer_sql_reader, result_set_map);	
}

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

unsigned short FinanceAnalyzerSqlReader::select_data(
		int source_index,
		const std::string& table_name,
		const std::string& cmd_table_field,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_CFG time_range_cfg,
		PRESULT_SET result_set
	)
{
	assert(query_field != NULL && !query_field->empty() && "result_set should NOT be NULL/Empty");
	assert(result_set != NULL && "result_set should NOT be NULL");
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
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_GREATER_EQUAL_THAN_FORMAT, time_range_cfg->get_start_time()->get_month());
			else if (time_range_cfg->get_end_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_MONTH_RULE_LESS_EQUAL_THAN_FORMAT, time_range_cfg->get_end_time()->get_month());
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
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_GREATER_EQUAL_THAN_FORMAT, time_range_cfg->get_start_time()->to_string());
			else if (time_range_cfg->get_end_time() != NULL)
				snprintf(_cmd_search_rule, 256, FORMAT_CMD_SELECT_DATE_RULE_LESS_EQUAL_THAN_FORMAT, time_range_cfg->get_end_time()->to_string());
			else
				need_add_rule = false;
		}
		if (need_add_rule)
			cmd_select_data += string(_cmd_search_rule);
	}
	WRITE_FORMAT_DEBUG("select data by command: %s", cmd_select_data.c_str());
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
		ret = result_set->set_date(row[0]);
		if (CHECK_FAILURE(ret))
			return ret;
// Set the data in each field
		for(int field_index = 1 ; field_index < actual_data_dimension ; field_index++)
		{
			ret = result_set->set_data(source_index, (*query_field)[field_index - 1], row[field_index]);
			if (CHECK_FAILURE(ret))
				return ret;
//			printf("%s ", row[i]);
		}
//		printf("\n");
	}

	return RET_SUCCESS;
}
