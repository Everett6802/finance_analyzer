#ifndef FINANCE_ANALYZER_DATABASE_TIME_RANGE_H
#define FINANCE_ANALYZER_DATABASE_TIME_RANGE_H

#include <pthread.h>
#include <deque>
#include <list>
#include <map>
#include <string>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


#define DECLARE_DATABASE_TIME_RANGE()\
static PFINANCE_ANALYZER_DATABASE_TIME_RANGE database_time_range = FinanceAnalyzerDatabaseTimeRange::get_instance();

class FinanceAnalyzerDatabaseTimeRange
{
	DECLARE_MSG_DUMPER()

	typedef std::map<int, PTIME_RANGE_CFG> TIME_RANGE_MAP;
	typedef std::deque<int> SOURCE_TYPE_INDEX_DEQUE;

private:
	static FinanceAnalyzerDatabaseTimeRange* instance;
	volatile int ref_cnt;
	TIME_RANGE_MAP database_time_range_map;
	SOURCE_TYPE_INDEX_DEQUE source_type_index_sort_queue;
	int source_type_index_sort_queue_size;

	FinanceAnalyzerDatabaseTimeRange();
	FinanceAnalyzerDatabaseTimeRange(const FinanceAnalyzerDatabaseTimeRange&);
	const FinanceAnalyzerDatabaseTimeRange& operator=(const FinanceAnalyzerDatabaseTimeRange&);
	~FinanceAnalyzerDatabaseTimeRange();

	unsigned short initialize();

public:
	static FinanceAnalyzerDatabaseTimeRange* get_instance();
	int add_ref();
	int release();
};
typedef FinanceAnalyzerDatabaseTimeRange* PFINANCE_ANALYZER_DATABASE_TIME_RANGE;

#endif
