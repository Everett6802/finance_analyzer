#ifndef FINANCE_ANALYZER_MGR_BASE_H
#define FINANCE_ANALYZER_MGR_BASE_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_mgr_inf.h"


class FinanceAnalyzerSqlReader;
class FinanceAnalyzerStatistics;
// class FinanceAnalyzerMathFormulaStatistics;
// class FinanceAnalyzerGraphTableStatistics;

class FinanceAnalyzerMgrBase : public IFinanceAnalyzerMgr
{
protected:
	enum ConfigFieldType{CONFIG_FIELD_UNKNOWN, CONFIG_FIELD_EMAIL_ADDRESS, CONFIG_FIELD_SIZE};
	DECLARE_MSG_DUMPER()

	// FinanceAnalyzerMathFormulaStatistics* finance_analyzer_math_formula_statistics;
	// FinanceAnalyzerGraphTableStatistics* finance_analyzer_graph_table_statistics;
	STRING_LIST email_address_list;
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader;
	FinanceAnalyzerStatistics* finance_analyzer_statistics;

	unsigned short parse_config();

public:
	FinanceAnalyzerMgrBase();
	~FinanceAnalyzerMgrBase();

	unsigned short initialize();

	// unsigned short query(const PTIME_RANGE_CFG time_range_cfg, const PQUERY_SET query_set, PRESULT_SET result_set)const;
	// unsigned short correlate(const SmartPointer<ResultSetAccessParam>& sp_access_param1, const SmartPointer<ResultSetAccessParam>& sp_access_param2, float& correlation, const PTIME_RANGE_CFG time_range_cfg=NULL)const;
	// unsigned short output_2d(const SmartPointer<ResultSetAccessParam>& sp_access_param1, const SmartPointer<ResultSetAccessParam>& sp_access_param2, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT, const PTIME_RANGE_CFG time_range_cfg=NULL)const;

	// unsigned short update_daily(int show_result_type)const;
	// unsigned short analyze_daily(int show_result_type, int offset=1)const;
	// unsigned short output_daily(int offset=1)const;
	// unsigned short get_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& time_range_cfg);

#ifdef DO_DEBUG
	unsigned short test();
#endif
};

#endif
