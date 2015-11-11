#ifndef FINANCE_ANALYZER_COMMON_CLASS_H
#define FINANCE_ANALYZER_COMMON_CLASS_H

#include "msg_dumper_wrapper.h"


class TimeCfg
{
	enum TimeType{TIME_MONTH, TIME_DATE};
private:
	TimeType time_type;
	int year;
	int month;
	int day;
	char time_str[16];

public:
	TimeCfg(const char* cur_time_str); // Format: "2015-09" or "2015-09-04"
	TimeCfg(int cur_year, int cur_month);
	TimeCfg(int cur_year, int cur_month, int cur_day);

	int get_year()const;
	int get_month()const;
	int get_day()const;
	const char* to_string()const;
	bool is_month_type()const;
};
typedef TimeCfg* PTIME_CFG;

class TimeRangeCfg
{
private:
	PTIME_CFG time_start_cfg;
	PTIME_CFG time_end_cfg;
	char* time_range_description;
	bool type_is_month;

public:
	TimeRangeCfg(const char* time_start_str, const char* time_end_str);
	TimeRangeCfg(int year_start, int month_start, int year_end, int month_end);
	TimeRangeCfg(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end);
	~TimeRangeCfg();
	bool is_month_type()const{return type_is_month;}
	const char* to_string();
	const PTIME_CFG get_start_time();
	const PTIME_CFG get_end_time();
};
typedef TimeRangeCfg* PTIME_RANGE_CFG;


#endif
