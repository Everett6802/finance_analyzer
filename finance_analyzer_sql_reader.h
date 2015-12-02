#ifndef FINANCE_ANALYZER_SQL_READER_H
#define FINANCE_ANALYZER_SQL_READER_H

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <string>
#include <list>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


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
	static const char* FORMAT_CMD_SELECT_DATE_RULE_GREATER_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_DATE_RULE_LESS_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_DATE_RULE_EQUAL_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_GREATER_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_LESS_THAN_FORMAT;
	static const char* FORMAT_CMD_SELECT_MONTH_RULE_EQUAL_FORMAT;
	MYSQL* connection;
	char cmd_buf[CMD_BUF_SIZE];
	std::string database_name;

public:
	FinanceAnalyzerSqlReader();
	~FinanceAnalyzerSqlReader();

	static unsigned short get_sql_field_command(int source_index, const DEQUE_INT& query_field, std::string& field_cmd);

	unsigned short try_connect_mysql(const std::string database);
	unsigned short disconnect_mysql();

	unsigned short select_data(
		int source_index,
		const std::string& table_name,
		const std::string& cmd_table_field,
		const PDEQUE_INT query_field,
		const PTIME_RANGE_CFG time_range_cfg,
		PRESULT_SET result_cfg
	);
};

#endif
