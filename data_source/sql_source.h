#ifndef SQL_SOURCE_H
#define SQL_SOURCE_H

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include "../common.h"
#include "source_param.h"


#define CMD_BUF_SIZE 512

class SqlSourceParam : public SourceBaseParam
{
public:
	SqlSourceParam();
	SqlSourceParam(bool new_continue_when_non_exist);
	virtual ~SqlSourceParam();

	virtual FinanceDataType get_type()const;
	virtual unsigned short to_object(ISource** source_obj);
};
typedef SqlSourceParam* PSQL_SOURCE_PARAM;

///////////////////////////////////////////////////////////////////////////

class SqlSource : public ISource
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
	// static const char* FORMAT_CMD_SELECT_MONTH_RULE_BETWEEN_FORMAT;
	// static const char* FORMAT_CMD_SELECT_MONTH_RULE_GREATER_EQUAL_THAN_FORMAT;
	// static const char* FORMAT_CMD_SELECT_MONTH_RULE_LESS_EQUAL_THAN_FORMAT;
	// static const char* FORMAT_CMD_SELECT_MONTH_RULE_EQUAL_FORMAT;

	static unsigned short get_sql_field_command(int method_index, const INT_DEQUE& query_field, std::string& field_cmd);
	static unsigned short read_from_tables(
		const PTIME_RANGE_PARAM time_range_param, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		SqlSource* sql_source_obj, 
		FinanceAnalysisMode finance_analysis_mode,
		PRESULT_SET result_set
	);
	static unsigned short read_market(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, SqlSource* sql_source_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_stock(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, const PCOMPANY_GROUP_SET company_group_set, SqlSource* sql_source_obj, PRESULT_SET_MAP result_set_map);

public:
	static unsigned short read_by_object(const PSEARCH_RULE_SET search_rule_set, PISOURCE source_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_param(const PSEARCH_RULE_SET search_rule_set, PISOURCE_PARAM source_param, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_default(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

private:
	MYSQL* connection;
	char cmd_buf[CMD_BUF_SIZE];
	std::string database_name;
	PSQL_SOURCE_PARAM source_param;
// public:
	unsigned short try_connect_mysql(const std::string database);
	unsigned short disconnect_mysql();

	unsigned short select_data(
		int method_index,
		const std::string& table_name,
		const std::string& cmd_table_field,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_PARAM time_range_param,
		PRESULT_SET result_set
	);

public:
	SqlSource();
	~SqlSource();

	virtual FinanceDataType get_type()const;

	unsigned short set_continue_when_non_exist(bool enable);
	// unsigned short set_time_range_param(const TimeRangeParam* new_time_range_param);
};
typedef SqlSource* PSQL_SOURCE;

#endif
