#ifndef FINANCE_ANALYZER_COMMON_H
#define FINANCE_ANALYZER_COMMON_H

#include "msg_dumper_wrapper.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macro

#ifndef CHECK_SUCCESS
#define CHECK_SUCCESS(X) (X == RET_SUCCESS ? true : false)
#endif

#ifndef CHECK_FAILURE
#define CHECK_FAILURE(X) !CHECK_SUCCESS(X)
#endif

extern const char* FINANCE_DATABASE_NAME_LIST[];
extern const int FINANCE_DATABASE_NAME_LIST_LEN;
extern const char* FINANCE_DATABASE_DESCRIPTION_LIST[];
extern const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DEFINITION[];
extern const char* FUTURE_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DEFINITION[];
extern const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DEFINITION[];

// Return values
extern const unsigned short RET_SUCCESS;

extern const unsigned short RET_FAILURE_UNKNOWN;
extern const unsigned short RET_FAILURE_INVALID_ARGUMENT;
extern const unsigned short RET_FAILURE_INVALID_POINTER;
extern const unsigned short RET_FAILURE_INSUFFICIENT_MEMORY;
extern const unsigned short RET_FAILURE_INCORRECT_OPERATION;
extern const unsigned short RET_FAILURE_OPEN_FILE;
extern const unsigned short RET_FAILURE_NOT_FOUND;
extern const unsigned short RET_FAILURE_INCORRECT_CONFIG;
extern const unsigned short RET_FAILURE_INCORRECT_PATH;
extern const unsigned short RET_FAILURE_IO_OPERATION;
extern const unsigned short RET_FAILURE_HANDLE_THREAD;
extern const unsigned short RET_FAILURE_SYSTEM_API;
extern const unsigned short RET_FAILURE_MYSQL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class

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
