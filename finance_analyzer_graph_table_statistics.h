#ifndef FINANCE_ANALYZER_GRAPH_TABLE_STATISTICS_H
#define FINANCE_ANALYZER_GRAPH_TABLE_STATISTICS_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"


class FinanceAnalyzerMgr;
class FinanceAnalyzerSqlReader;

class FinanceAnalyzerGraphTableStatistics
{
	DECLARE_MSG_DUMPER()
	DECLARE_WORKDAY_CANLENDAR()
	DECLARE_DATABASE_TIME_RANGE()

private:
	bool need_res_info;
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader;

	void enable_res_info(bool enable);
	const char* get_last_res_info()const;
public:
	FinanceAnalyzerGraphTableStatistics();
	~FinanceAnalyzerGraphTableStatistics();
};

#endif
