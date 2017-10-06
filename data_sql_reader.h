#ifndef DATA_SQL_READER_H
#define DATA_SQL_READER_H

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include "common.h"


#define CMD_BUF_SIZE 512

class DataSqlReaderParam
{
public:
	bool continue_when_non_exist;

	DataSqlReaderParam();
	~DataSqlReaderParam();
};
typedef DataSqlReaderParam* PDATA_SQL_READER_PARAM;

///////////////////////////////////////////////////////////////////////////

class DataSqlReader
{
	DECLARE_MSG_DUMPER()
	// friend class FinanceAnalyzerInteractiveSession;

private:
	static const char* MYSQL_SERVER;
	static const char* MYSQL_USERNAME;
	static const char* MYSQL_PASSWORD;

	static const char* FORMAT_CMD_CREATE_DATABASE;
	static const char* FORMAT_CMD_SELECT_DATA_HEAD;
	static const char* FORMAT_CMD_SELECT_DATA_TAIL_FORMAT;
	static const char* FORMAT_CMD_SELECT_DATE_RULE_BETWEEN_FORMAT;
	static const char* FORMAT_CMD_SELECT_DATE_RULE_GREATER_EQUAL_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_DATE_RULE_LESS_EQUAL_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_DATE_RULE_EQUAL_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_GREATER_EQUAL_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_LESS_EQUAL_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_EQUAL_FORMAT;

	static unsigned short get_sql_field_command(int method_index, const INT_DEQUE& query_field, std::string& field_cmd);
	static unsigned short read_from_tables(
		const PTIME_RANGE_CFG restricted_time_range_cfg, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		DataSqlReader* reader_obj, 
		FinanceAnalysisMode finance_analysis_mode,
		PRESULT_SET result_set
	);
	static unsigned short read_market(const PQUERY_SET query_set, const PTIME_RANGE_CFG time_range_cfg, DataSqlReader* reader_obj, PRESULT_SET_MAP result_set_map);
	// static unsigned short read_market(const PQUERY_SET query_set, const PTIME_RANGE_CFG time_range_cfg, PRESULT_SET_MAP result_set_map);
	static unsigned short read_stock(const PQUERY_SET query_set, const PTIME_RANGE_CFG time_range_cfg, const PCOMPANY_GROUP_SET company_group_set, DataSqlReader* reader_obj, PRESULT_SET_MAP result_set_map);
	// static unsigned short read_stock(const PQUERY_SET query_set, const PTIME_RANGE_CFG time_range_cfg, const PCOMPANY_GROUP_SET company_group_set, PRESULT_SET_MAP result_set_map);

public:
	// static unsigned short query(const PSEARCH_RULE_SET search_rule_set, DataSqlReader* sql_reader, PRESULT_SET_MAP result_set_map);
	// static unsigned short query(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_object(const PSEARCH_RULE_SET search_rule_set, void* sql_reader_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_param(const PSEARCH_RULE_SET search_rule_set, void* sql_reader_param, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_default(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

private:
	MYSQL* connection;
	char cmd_buf[CMD_BUF_SIZE];
	std::string database_name;
	DataSqlReaderParam data_sql_reader_param;
// public:
	DataSqlReader();
	~DataSqlReader();

	unsigned short try_connect_mysql(const std::string database);
	unsigned short disconnect_mysql();

	unsigned short select_data(
		int source_index,
		const std::string& table_name,
		const std::string& cmd_table_field,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_CFG time_range_cfg,
		PRESULT_SET result_set
	);

	unsigned short set_continue_when_non_exist(bool enable);
};
typedef DataSqlReader* PDATA_SQL_READER;

#endif
