#ifndef FINANCE_ANALYZER_MATH_FORMULA_STATISTICS_H
#define FINANCE_ANALYZER_MATH_FORMULA_STATISTICS_H

#include <string>
#include <list>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"


class FinanceAnalyzerStatistics;
// class FinanceAnalyzerSqlReader;

class FinanceAnalyzerMathFormulaStatistics
{
	friend class FinanceAnalyzerStatistics;
	DECLARE_MSG_DUMPER()
	DECLARE_WORKDAY_CANLENDAR()
	DECLARE_DATABASE_TIME_RANGE()

private:
	static unsigned short generate_general_query_set(QuerySet& query_set);
	static const PQUERY_SET get_general_query_set();

	// bool need_res_info;
	FinanceAnalyzerStatistics* parent_obj;

	FinanceAnalyzerMathFormulaStatistics(FinanceAnalyzerStatistics* parent);
	~FinanceAnalyzerMathFormulaStatistics();

	const char* get_description_head(const PRESULT_SET result_set, const PRESULT_SET_ACCESS_PARAM result_set_access_param, const PTIME_RANGE_CFG time_range_cfg=NULL)const;
	unsigned short get_restricted_time_range(int source_index, int field_index, const PTIME_RANGE_CFG time_range_cfg, SmartPointer<TimeRangeCfg>& sp_restricted_time_range_cfg)const;
	unsigned short query_from_database(int source_index, int field_index, const SmartPointer<TimeRangeCfg>& sp_restricted_time_range_cfg, SmartPointer<ResultSet>& sp_result_set)const;

	unsigned short get_statistics_result(StatisticsMethod statistics_method, int source_index, int field_index, const PTIME_RANGE_CFG time_range_cfg, std::string& result_str)const;

	unsigned short get_range_value_result_str(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const;
	unsigned short get_average_result_str(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const;
	unsigned short get_standard_deviation_result_str(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const;
	
public:
	unsigned short calculate_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const;
};

#endif
