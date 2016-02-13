#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include <string>
#include <list>
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


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
	unsigned short show_result(std::string result_str, const PTIME_CFG time_cfg, int show_result_type)const;

public:
	FinanceAnalyzerMgr();
	~FinanceAnalyzerMgr();

	unsigned short initialize();
	unsigned short query(const PTIME_RANGE_CFG time_range_cfg, const PQUERY_SET query_set, PRESULT_SET result_set)const;
	unsigned short correlate(FinanceSourceType finance_source_type1, int finance_field_no1, ArrayElementCalculationType calculation_type1, FinanceSourceType finance_source_type2, int finance_field_no2, ArrayElementCalculationType calculation_type2, float& correlation, const PTIME_RANGE_CFG time_range_cfg=NULL)const;
	unsigned short correlate(FinanceSourceType finance_source_type1, int finance_field_no1, FinanceSourceType finance_source_type2, int finance_field_no2, float& correlation, const PTIME_RANGE_CFG time_range_cfg=NULL)const;

	unsigned short run_daily(int show_result_type);
	unsigned short analyze_daily(int show_result_type);

#ifdef DO_DEBUG
	unsigned short test();
#endif
};

#endif
