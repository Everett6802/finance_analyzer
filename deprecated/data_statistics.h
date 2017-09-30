#ifndef DATA_STATISTICS_H
#define DATA_STATISTICS_H

#include "common.h"
#include "workday_canlendar.h"
// #include "database_time_range.h"


class FinanceAnalyzerSqlReader;
class FinanceAnalyzerMathFormulaStatistics;
class FinanceAnalyzerGraphTableStatistics;

class FinanceAnalyzerStatistics
{
	friend class FinanceAnalyzerMathFormulaStatistics;
	friend class FinanceAnalyzerGraphTableStatistics;
	DECLARE_MSG_DUMPER()
	DECLARE_WORKDAY_CANLENDAR()
	// DECLARE_DATABASE_TIME_RANGE()

private:
	STRING_LIST email_address_list;

	// void enable_res_info(bool enable);
	// const char* get_last_res_info()const;
	unsigned short copy_email_adress_list(STRING_LIST& src_email_address_list);
	unsigned short show_result(std::string& result_str, int show_result_type, const char* data_description)const;

public:
	FinanceAnalyzerStatistics();
	~FinanceAnalyzerStatistics();

	unsigned short initialize(STRING_LIST& src_email_address_list);
	unsigned short get_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg);
};

#endif
