#ifndef FINANCE_ANALYZER_SQL_READER_H
#define FINANCE_ANALYZER_SQL_READER_H

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include "finance_analyzer_common.h"


#define CMD_BUF_SIZE 512

class FinanceAnalyzerSqlReader
{
	DECLARE_MSG_DUMPER()

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

	static unsigned short get_sql_field_command(int source_type_index, const INT_DEQUE& query_field, std::string& field_cmd);
	static unsigned short query_from_tables(
		const PTIME_RANGE_CFG restricted_time_range_cfg, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, 
		PRESULT_SET result_set
	);

	MYSQL* connection;
	char cmd_buf[CMD_BUF_SIZE];
	std::string database_name;

	unsigned short select_data(
		int source_index,
		const std::string& table_name,
		const std::string& cmd_table_field,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_CFG time_range_cfg,
		PRESULT_SET result_set
	);

public:
	FinanceAnalyzerSqlReader();
	~FinanceAnalyzerSqlReader();

	static unsigned short query_market(const PTIME_RANGE_CFG time_range_cfg, const PMARKET_QUERY_SET market_query_set, FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, PRESULT_SET result_set);
	static unsigned short query_market(const PTIME_RANGE_CFG time_range_cfg, const PMARKET_QUERY_SET market_query_set, PRESULT_SET result_set);

	static unsigned short query_stock(const PTIME_RANGE_CFG time_range_cfg, const PSTOCK_QUERY_SET stock_query_set, FinanceAnalyzerSqlReader* finance_analyzer_sql_reader, PRESULT_SET_GROUP result_set_group);
	static unsigned short query_stock(const PTIME_RANGE_CFG time_range_cfg, const PSTOCK_QUERY_SET stock_query_set, PRESULT_SET_GROUP result_set_group);

	unsigned short try_connect_mysql(const std::string database);
	unsigned short disconnect_mysql();
};

#endif
