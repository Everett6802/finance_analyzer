#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include <string>
#include <list>
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


// #define DO_DEBUG

class FinanceAnalyzerSqlReader;
class FinanceAnalyzerCalculator;

class FinanceAnalyzerMgr
{
	enum ConfigFieldType{CONFIG_FIELD_UNKNOWN, CONFIG_FIELD_EMAIL_ADDRESS, CONFIG_FIELD_SIZE};
	DECLARE_MSG_DUMPER()

private:
	DECLARE_WORKDAY_CANLENDAR();
	DECLARE_DATABASE_TIME_RANGE();
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader;
	FinanceAnalyzerCalculator* finance_analyzer_calculator;
	std::list<std::string> email_address_list;

	unsigned short parse_config();

public:
	enum ShowResultType{SHOW_RESULT_STDOUT=0x1, SHOW_RESULT_FILE=0x2, SHOW_RESULT_EMAIL=0x4};

	FinanceAnalyzerMgr();
	~FinanceAnalyzerMgr();

	unsigned short initialize();
	unsigned short query(const PTIME_RANGE_CFG time_range_cfg, const PQUERY_SET query_set, PRESULT_SET result_set)const;
	unsigned short correlate(const PTIME_RANGE_CFG time_range_cfg, FinanceSourceType finance_source_type1, int finance_field_no1, FinanceSourceType finance_source_type2, int finance_field_no2, float& correlation)const;

	unsigned short run_daily();
	unsigned short show_daily(const SmartPointer<TimeCfg>& sp_time_cfg, const PRESULT_SET result_set, int show_result_type=SHOW_RESULT_FILE|SHOW_RESULT_EMAIL)const;
	unsigned short analyze_daily();

#ifdef DO_DEBUG
	unsigned short test();
#endif
};

#endif
