#ifndef FINANCE_ANALYZER_DATABASE_TIME_RANGE_H
#define FINANCE_ANALYZER_DATABASE_TIME_RANGE_H

#include <pthread.h>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include "finance_analyzer_common.h"


#define DECLARE_DATABASE_TIME_RANGE() PFINANCE_ANALYZER_DATABASE_TIME_RANGE database_time_range;
#define IMPLEMENT_DATABASE_TIME_RANGE() database_time_range = FinanceAnalyzerDatabaseTimeRange::get_instance();
#define RELEASE_DATABASE_TIME_RANGE() SAFE_RELEASE(database_time_range)

#define DECLARE_AND_IMPLEMENT_STATIC_DATABASE_TIME_RANGE()\
static PFINANCE_ANALYZER_DATABASE_TIME_RANGE database_time_range = FinanceAnalyzerDatabaseTimeRange::get_instance();

class FinanceAnalyzerDatabaseTimeRange
{
	DECLARE_MSG_DUMPER()

	typedef std::deque<PTIME_RANGE_CFG> TIME_RANGE_DEQUE;

private:
	static FinanceAnalyzerDatabaseTimeRange* instance;
	volatile int ref_cnt;
	TIME_RANGE_DEQUE database_time_range_deque;
	PTIME_RANGE_CFG max_time_range_cfg;

	FinanceAnalyzerDatabaseTimeRange();
	FinanceAnalyzerDatabaseTimeRange(const FinanceAnalyzerDatabaseTimeRange&);
	const FinanceAnalyzerDatabaseTimeRange& operator=(const FinanceAnalyzerDatabaseTimeRange&);
	~FinanceAnalyzerDatabaseTimeRange();

	unsigned short initialize();

public:
	static FinanceAnalyzerDatabaseTimeRange* get_instance();
	int add_ref();
	int release();

	unsigned short restrict_time_range(const std::set<int>& source_type_index_set, PTIME_RANGE_CFG time_range_cfg);
	unsigned short restrict_time_range(const PQUERY_SET query_set, PTIME_RANGE_CFG time_range_cfg);
	unsigned short get_max_database_time_range(SmartPointer<TimeRangeCfg>& sp_max_database_time_range_cfg);
};
typedef FinanceAnalyzerDatabaseTimeRange* PFINANCE_ANALYZER_DATABASE_TIME_RANGE;

#endif
