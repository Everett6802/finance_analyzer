#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


class FinanceAnalyzerSqlReader;

class FinanceAnalyzerMgr
{
	DECLARE_MSG_DUMPER()
private:
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader;

public:
	FinanceAnalyzerMgr();
	~FinanceAnalyzerMgr();

	unsigned short query(const PTIME_RANGE_CFG time_range_cfg, const PQUERY_SET query_set, PRESULT_SET result_set)const;

	unsigned short run_daily();
	unsigned short write_daily(const char* filepath, const PRESULT_SET result_set)const;

	unsigned short test();
};

#endif
