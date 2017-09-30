#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_graph_table_statistics.h"
#include "finance_analyzer_statistics.h"
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_algorithm.h"
#include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_output.h"


using namespace std;

static const int RES_INFO_LENGTH = 1024;
static char res_info[RES_INFO_LENGTH];

FinanceAnalyzerGraphTableStatistics::FinanceAnalyzerGraphTableStatistics(FinanceAnalyzerStatistics* parent) :
	parent_obj(parent)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_WORKDAY_CANLENDAR()
	IMPLEMENT_DATABASE_TIME_RANGE()
	// res_info[0] = '\0';
	// finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
	// if (finance_analyzer_sql_reader == NULL)
	// {
	// 	assert(false && "Fail to allocate memory: finance_analyzer_sql_reader");
	// 	throw bad_alloc(); 
	// }
}

FinanceAnalyzerGraphTableStatistics::~FinanceAnalyzerGraphTableStatistics()
{
	// if (finance_analyzer_sql_reader != NULL)
	// {
	// 	delete finance_analyzer_sql_reader;
	// 	finance_analyzer_sql_reader = NULL;
	// }
	RELEASE_DATABASE_TIME_RANGE()
	RELEASE_WORKDAY_CANLENDAR()
	RELEASE_MSG_DUMPER()
}

// void FinanceAnalyzerGraphTableStatistics::enable_res_info(bool enable)
// {
// 	need_res_info = enable;
// }

// const char* FinanceAnalyzerGraphTableStatistics::get_last_res_info()const
// {
// 	return res_info;
// }

unsigned short FinanceAnalyzerGraphTableStatistics::fillout_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const
{
	typedef unsigned short (FinanceAnalyzerGraphTableStatistics::*fillout_statistics_func_ptr)(const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const;
	static fillout_statistics_func_ptr fillout_statistics_func_array[] =
	{
	};
	int index = statistics_method - StatisticsTable_Start;
	return (this->*(fillout_statistics_func_array[index]))(sp_time_range_cfg);
}

unsigned short FinanceAnalyzerGraphTableStatistics::plot_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const
{
	typedef unsigned short (FinanceAnalyzerGraphTableStatistics::*plot_statistics_func_ptr)(const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const;
	static plot_statistics_func_ptr plot_statistics_func_array[] =
	{
	};
	int index = statistics_method - StatisticsGraph_Start;
	return (this->*(plot_statistics_func_array[index]))(sp_time_range_cfg);
}
