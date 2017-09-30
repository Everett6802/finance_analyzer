#ifndef DATABASE_TIME_RANGE_H
#define DATABASE_TIME_RANGE_H

#include <pthread.h>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include "common.h"


#define DECLARE_DATABASE_TIME_RANGE() PDATABASE_TIME_RANGE database_time_range;
#define IMPLEMENT_DATABASE_TIME_RANGE() database_time_range = DatabaseTimeRange::get_instance();
#define RELEASE_DATABASE_TIME_RANGE() SAFE_RELEASE(database_time_range)

#define DECLARE_AND_IMPLEMENT_STATIC_DATABASE_TIME_RANGE()\
static PDATABASE_TIME_RANGE database_time_range = DatabaseTimeRange::get_instance();

class DatabaseTimeRange
{
	DECLARE_MSG_DUMPER()

	typedef std::deque<PTIME_RANGE_CFG> TIME_RANGE_DEQUE;

private:
	static DatabaseTimeRange* instance;
	volatile int ref_cnt;
	TIME_RANGE_DEQUE database_time_range_deque;
	PTIME_RANGE_CFG max_time_range_cfg;

	DatabaseTimeRange();
	DatabaseTimeRange(const DatabaseTimeRange&);
	const DatabaseTimeRange& operator=(const DatabaseTimeRange&);
	virtual ~DatabaseTimeRange();

	unsigned short initialize();
	void deinitialize();

public:
	static DatabaseTimeRange* get_instance();
	int add_ref();
	int release();

	unsigned short restrict_time_range(const std::set<int>& source_type_index_set, PTIME_RANGE_CFG time_range_cfg);
	unsigned short restrict_time_range(const PQUERY_SET query_set, PTIME_RANGE_CFG time_range_cfg);
	unsigned short get_max_database_time_range(SmartPointer<TimeRangeCfg>& sp_max_database_time_range_cfg);
};
typedef DatabaseTimeRange* PDATABASE_TIME_RANGE;

#endif
