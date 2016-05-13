#ifndef FINANCE_ANALYZER_GRAPH_TABLE_STATISTICS_H
#define FINANCE_ANALYZER_GRAPH_TABLE_STATISTICS_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"


class FinanceAnalyzerStatistics;
// class FinanceAnalyzerSqlReader;

class FinanceAnalyzerGraphTableStatistics
{
	friend class FinanceAnalyzerStatistics;
	DECLARE_MSG_DUMPER()
	DECLARE_WORKDAY_CANLENDAR()
	DECLARE_DATABASE_TIME_RANGE()

private:
	// bool need_res_info;
	FinanceAnalyzerStatistics* parent_obj;

	// void enable_res_info(bool enable);
	// const char* get_last_res_info()const;
	FinanceAnalyzerGraphTableStatistics(FinanceAnalyzerStatistics* parent);
	~FinanceAnalyzerGraphTableStatistics();

public:
	unsigned short fillout_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const;
	unsigned short plot_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const;
};

#endif
