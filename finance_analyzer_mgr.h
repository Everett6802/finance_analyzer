#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include <string>
#include <list>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


// #define DO_DEBUG

class FinanceAnalyzerSqlReader;

class FinanceAnalyzerMgr
{
	enum ConfigFieldType{CONFIG_FIELD_UNKNOWN, CONFIG_FIELD_EMAIL_ADDRESS, CONFIG_FIELD_SIZE};

	DECLARE_MSG_DUMPER()

private:
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader;
	std::list<std::string> email_address_list;

	unsigned short parse_config();

public:
	enum ShowResultType{SHOW_RESULT_STDOUT=0x1, SHOW_RESULT_FILE=0x2, SHOW_RESULT_EMAIL=0x4};

	FinanceAnalyzerMgr();
	~FinanceAnalyzerMgr();

	unsigned short initialize();
	unsigned short query(const PTIME_RANGE_CFG time_range_cfg, const PQUERY_SET query_set, PRESULT_SET result_set)const;

	unsigned short run_daily();
	unsigned short show_daily_result(const SmartPointer<TimeCfg>& sp_time_cfg, const PRESULT_SET result_set, int show_result_type=SHOW_RESULT_FILE|SHOW_RESULT_EMAIL)const;
#ifdef DO_DEBUG
	unsigned short test();
#endif
};

#endif
