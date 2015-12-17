#ifndef FINANCE_ANALYZER_WORKDAY_CANLENDAR_H
#define FINANCE_ANALYZER_WORKDAY_CANLENDAR_H

#include <pthread.h>
#include <deque>
#include <list>
#include <map>
#include <string>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


#define DECLARE_WORKDAY_CANLENDAR()\
static PFINANCE_ANALYZER_WORKDAY_CANLENDAR workday_canlendar = FinanceAnalyzerWorkdayCanlendar::get_instance();

class FinanceAnalyzerWorkdayCanlendar
{
	DECLARE_MSG_DUMPER()

	enum TRAVERSE_SEARCH_TYPE{TRAVERSE_SEARCH_EQUAL, TRAVERSE_SEARCH_PREV, TRAVERSE_SEARCH_NEXT};

	typedef std::deque<int> DAY_DEQUE;
	typedef DAY_DEQUE YEAR_QUEUE;
	typedef DAY_DEQUE* PDAY_DEQUE;
	typedef std::map<int, PDAY_DEQUE> YEAR_MAP;
	typedef YEAR_MAP* PYEAR_MAP;

private:
	static FinanceAnalyzerWorkdayCanlendar* instance;
	volatile int ref_cnt;
	PTIME_RANGE_CFG time_range_cfg;
	PTIME_CFG map_start_time_cfg;
	PTIME_CFG map_end_time_cfg;
	YEAR_MAP workday_map;
	YEAR_QUEUE workday_year_sort_queue;
	int workday_year_sort_queue_size;

	FinanceAnalyzerWorkdayCanlendar();
	FinanceAnalyzerWorkdayCanlendar(const FinanceAnalyzerWorkdayCanlendar&);
	const FinanceAnalyzerWorkdayCanlendar& operator=(const FinanceAnalyzerWorkdayCanlendar&);
	~FinanceAnalyzerWorkdayCanlendar();

	unsigned short initialize();
	bool check_in_range(int year, int month, int day)const;
	bool check_in_range(const PTIME_CFG time_cfg)const;
	bool check_greater_than_start(int year, int month, int day)const;
	bool check_greater_than_start(const PTIME_CFG time_cfg)const;
	bool check_less_than_end(int year, int month, int day)const;
	bool check_less_than_end(const PTIME_CFG time_cfg)const;

	unsigned short find_data_pos(int year, int month, int day, int& year_key, int& month_index, int& day_index, TRAVERSE_SEARCH_TYPE traverse_search_type=TRAVERSE_SEARCH_EQUAL);
	unsigned short get_date(int year_key, int month_index, int day_index, int& year, int& month, int& day);
	unsigned short get_date(int year_key, int month_index, int day_index, PTIME_CFG* time_cfg);

public:
	static FinanceAnalyzerWorkdayCanlendar* get_instance();
	int add_ref();
	int release();

	bool is_workday(int year, int month, int day);
	bool is_workday(const PTIME_CFG time_cfg);

	unsigned short get_prev_workday_array(int year_base, int month_base, int day_base, std::deque<PTIME_CFG>& workday_deque, int max_workday_amount=-1);
	unsigned short get_next_workday_array(int year_base, int month_base, int day_base, std::deque<PTIME_CFG>& workday_deque, int max_workday_amount=-1);

	unsigned short get_prev_workday(int year_base, int month_base, int day_base, int& prev_year, int& prev_month, int& prev_day);
	unsigned short get_prev_workday(const PTIME_CFG time_cfg, PTIME_CFG* prev_time_cfg);
	unsigned short get_next_workday(int year_base, int month_base, int day_base, int& prev_year, int& prev_month, int& prev_day);
	unsigned short get_next_workday(const PTIME_CFG time_cfg, PTIME_CFG* prev_time_cfg);
};
typedef FinanceAnalyzerWorkdayCanlendar* PFINANCE_ANALYZER_WORKDAY_CANLENDAR;

#endif
