#ifndef COMMON_CLASS_TIME_H
#define COMMON_CLASS_TIME_H

#include "msg_dumper_wrapper.h"
#include "common_definition.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
struct DateCfg
{
	int year;
	int month;
	int day;
};

struct MonthCfg
{
	int year;
	int month;
};

struct QuarterCfg
{
	int year;
	int quarter;
};

enum TimeUnit{TIME_UNIT_DATE, TIME_UNIT_MONTH, TIME_UNIT_QUARTER, TIME_UNIT_NONE};

class TimeParam
{
	DECLARE_MSG_DUMPER()
private:
	static unsigned short parse_time_string(const char* time_str, TimeParam& time_param);

public:
	static unsigned short create_instance_from_string(const char* time_str, TimeParam& time_param);
	static unsigned short create_instance_from_string(const char* time_str, TimeParam** time_param);

private:
	TimeUnit time_unit;
	union
	{
		int value[3];
		DateCfg date_cfg;
		MonthCfg month_cfg;
		QuarterCfg quarter_cfg;
	}param;
	char time_str[16];

	int get_date_int_value()const;
	int get_month_int_value()const;
	int get_quarter_int_value()const;
	int get_int_value()const;
	void find_date_time_string();
	void find_month_time_string();
	void find_quarter_time_string();
	void find_time_string();
	void update_param_from_value_array(TimeUnit cur_time_unit, const int* cur_value_array);
	bool check_param_equal_from_value_array(TimeUnit another_time_unit, const int* another_value_array)const;

public:
	TimeParam();
	TimeParam(const char* cur_time_str); // Format: "2015-09-04" or "2015-09" or "2015q3"
	TimeParam(TimeUnit cur_time_unit, int* cur_value_array);
	TimeParam(const TimeParam* another_time_param);
	TimeParam& operator=(const TimeParam& another);
	~TimeParam();

	const char* to_string()const;
	TimeUnit get_time_unit()const;
	bool equal_to(const TimeParam* another_time_param)const;

	void reset(const char* new_time_str); // Format: "2015-09" or "2015-09-04"
	void reset(TimeUnit new_time_unit, int* new_value_array);
	void reset(const TimeParam* new_time_param);

	bool operator<(const TimeParam& another_time_cfg)const;
	bool operator>(const TimeParam& another_time_cfg)const;
	bool operator==(const TimeParam& another_time_cfg)const;
	bool operator>=(const TimeParam& another_time_cfg)const;
	bool operator<=(const TimeParam& another_time_cfg)const;
	bool operator!=(const TimeParam& another_time_cfg)const;
	bool operator<(const char* another_time_str)const;
	bool operator>(const char* another_time_str)const;
	bool operator==(const char* another_time_str)const;
	bool operator>=(const char* another_time_str)const;
	bool operator<=(const char* another_time_str)const;
	bool operator!=(const char* another_time_str)const;
};
typedef TimeParam* PTIME_PARAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum TimeInRangeType{TIME_BEFORE_RANGE, TIME_IN_RANGE, TIME_AFTER_RANGE};

class TimeRangeParam
{
private:
	DECLARE_MSG_DUMPER()

public:
	static TimeInRangeType time_in_range_type(const TimeRangeParam* time_range_param, const TimeParam* time_cfg);
	static TimeInRangeType time_in_range_type(const TimeRangeParam* time_range_param, const char* time_str);
	static bool time_in_range(const TimeRangeParam* time_range_param, const TimeParam* time_cfg);
	static unsigned short create_instance_from_string(const char* time_str, TimeRangeParam& time_range_param);
	static unsigned short create_instance_from_string(const char* time_str, TimeRangeParam** time_range_param);

protected:
	PTIME_PARAM time_start_param;
	PTIME_PARAM time_end_param;
	char* time_range_description;
	TimeUnit time_unit;
	bool add_done;

	void update_time_unit(const TimeParam* time_param_start, const TimeParam* time_param_end);

public:
	TimeRangeParam();
	TimeRangeParam(const char* time_str_start, const char* time_str_end);
	TimeRangeParam(TimeUnit time_unit_start, int* value_array_start, TimeUnit time_unit_end, int* value_array_end);
	TimeRangeParam(const TimeParam* time_param_start, const TimeParam* time_param_end);
	TimeRangeParam(const TimeRangeParam& another_time_range_param);
	~TimeRangeParam();

	void add_start_time(const PTIME_PARAM new_time_param_start);
	void add_end_time(const PTIME_PARAM new_time_param_end);
	void add_time(const PTIME_PARAM new_time_param_start, const PTIME_PARAM new_time_param_end);
	void add_start_time(const char* new_time_str_start);
	void add_end_time(const char* new_time_str_end);
	void add_time(const char* new_time_str_start, const char* new_time_str_end);
	unsigned short add_time_done();

	void reset_start_time(const PTIME_PARAM new_time_param_start);
	void reset_end_time(const PTIME_PARAM new_time_param_end);
	void reset_time(const PTIME_PARAM new_time_param_start, const PTIME_PARAM new_time_param_end);
	void reset_start_time(const char* new_time_str_start);
	void reset_end_time(const char* new_time_str_end);
	void reset_time(const char* new_time_str_start, const char* new_time_str_end);

	bool is_single_time()const;
	bool is_add_time_done()const;
	const char* to_string();
	const PTIME_PARAM get_start_time()const;
	const PTIME_PARAM get_end_time()const;
// Don't modify the start/end time in the following way
	// PTIME_PARAM get_start_time();
	// PTIME_PARAM get_end_time();
};
typedef TimeRangeParam* PTIME_RANGE_PARAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SingleTimeRangeParam : public TimeRangeParam
{
public:
	SingleTimeRangeParam(const char* time_str); // Single day
	SingleTimeRangeParam(TimeUnit time_unit, int* value_array); // Single month
	SingleTimeRangeParam(const TimeParam* time_param); // Single day
};
typedef SingleTimeRangeParam* PSINGLE_TIME_RANGE_PARAM;


#endif
