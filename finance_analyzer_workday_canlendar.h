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
//	friend class const_iterator;
	DECLARE_MSG_DUMPER()

	typedef std::deque<int> DAY_DEQUE;
	typedef std::list<int> YEAR_LIST;
	typedef DAY_DEQUE* PDAY_DEQUE;
	typedef std::map<int, PDAY_DEQUE> YEAR_MAP;
	typedef YEAR_MAP* PYEAR_MAP;

private:
	static FinanceAnalyzerWorkdayCanlendar* instance;
	volatile int ref_cnt;
	PTIME_RANGE_CFG time_range_cfg;
	YEAR_MAP non_workday_map;
	YEAR_LIST non_workday_year_sort_list;
//	int* workday_value_array;

	FinanceAnalyzerWorkdayCanlendar();
	FinanceAnalyzerWorkdayCanlendar(const FinanceAnalyzerWorkdayCanlendar&);
	const FinanceAnalyzerWorkdayCanlendar& operator=(const FinanceAnalyzerWorkdayCanlendar&);
	~FinanceAnalyzerWorkdayCanlendar();

	unsigned short initialize();
	bool check_in_range(int year, int month, int day)const;
	bool check_in_range(const PTIME_CFG time_cfg)const;
	unsigned short find_data_pos(int year, int month, int day, int& year_key, int& month_index, int& day_index);

public:
	static FinanceAnalyzerWorkdayCanlendar* get_instance();
	int add_ref();
	int release();

	bool is_workday(int year, int month, int day);
	bool is_workday(const PTIME_CFG time_cfg);

//	unsigned short get_prev_workday(int year, int month, int day, int& prev_year, int& prev_month, int& prev_day);
//	unsigned short get_prev_workday(const PTIME_CFG time_cfg, PTIME_CFG* prev_time_cfg);
//	unsigned short get_next_workday(int year, int month, int day, int& prev_year, int& prev_month, int& prev_day);
//	unsigned short get_next_workday(const PTIME_CFG time_cfg, PTIME_CFG* prev_time_cfg);

//	class const_iterator
//	{
//	private:
//		PYEAR_MAP ptr_;
//		int year_key;
//		int month_index;
//		int day_index;
//
//		void get_next()const
//		{
//			ptr_
//		}
//	public:
//		const_iterator(PYEAR_MAP ptr) : ptr_(ptr) { }
//		const_iterator operator++()
//		{
//			const_iterator i = *this;
//			ptr_++;
//			return i;
//		}
//		const_iterator operator++(int junk)
//		{
//			ptr_++; return *this;
//		}
//		const reference operator*() { return *ptr_; }
//		const pointer operator->() { return ptr_; }
//	};
//	const_iterator begin() const
//	{
//		return const_iterator(non_workday_map);
//	}
//	const_iterator end() const
//	{
//		return const_iterator(NULL);
//	}
};
typedef FinanceAnalyzerWorkdayCanlendar* PFINANCE_ANALYZER_WORKDAY_CANLENDAR;

#endif
