#include <assert.h>
#include <string>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common.h"


using namespace std;

const char* FINANCE_DATABASE_NAME_LIST[] =
{
	"stock_top3_legal_persons_net_buy_or_sell",
	"future_top3_legal_persons_open_interest",
	"future_top10_dealers_and_legal_persons"
};
const int FINANCE_DATABASE_NAME_LIST_LEN = sizeof(FINANCE_DATABASE_NAME_LIST) / sizeof(FINANCE_DATABASE_NAME_LIST[0]);
const char* FINANCE_DATABASE_DESCRIPTION_LIST[] =
{
	"三大法人現貨買賣超",
	"三大法人期貨留倉淨額",
	"十大交易人及特定法人期貨資訊"
};
const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DEFINITION[] =
{
	"date DATE NOT NULL PRIMARY KEY", // 日期
	"value1 BIGINT", // 自營商(自行買賣)_買進金額
	"value2 BIGINT", // 自營商(自行買賣)_賣出金額
	"value3 BIGINT", // 自營商(自行買賣)_買賣差額
	"value4 BIGINT", // 自營商(避險)_買進金額
	"value5 BIGINT", // 自營商(避險)_賣出金額
	"value6 BIGINT", // 自營商(避險)_買賣差額
	"value7 BIGINT", // 投信_買進金額
	"value8 BIGINT", // 投信_賣出金額
	"value9 BIGINT", // 投信_買賣差額
	"value10 BIGINT", // 外資及陸資_買進金額
	"value11 BIGINT", // 外資及陸資_賣出金額
	"value12 BIGINT", // 外資及陸資_買賣差額
};
const char* FUTURE_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DEFINITION[] =
{
	"date DATE NOT NULL PRIMARY KEY", // 日期
	"value1 INT", // 自營商_多方_口數 int",
	"value2 INT", // 自營商_多方_契約金額 int",
	"value3 INT", // 自營商_空方_口數 int",
	"value4 INT", // 自營商_空方_契約金額 int",
	"value5 INT", // 自營商_多空淨額_口數 int",
	"value6 INT", // 自營商_多空淨額_契約金額 int",
	"value7 INT", // "投信_多方_口數 int",
	"value8 INT", // 投信_多方_契約金額 int",
	"value9 INT", // 投信_空方_口數 int",
	"value10 INT", // 投信_空方_契約金額 int",
	"value11 INT", // 投信_多空淨額_口數 int",
	"value12 INT", // 投信_多空淨額_契約金額 int",
	"value13 INT", // 外資_多方_口數 int",
	"value14 INT", // 外資_多方_契約金額 int",
	"value15 INT", // 外資_空方_口數 int",
	"value16 INT", // 外資_空方_契約金額 int",
	"value17 INT", // 外資_多空淨額_口數 int",
	"value18 INT", // 外資_多空淨額_契約金額 int",
};
const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DEFINITION[] =
{
	"date DATE NOT NULL PRIMARY KEY", // 日期
	"value1 INT", // 臺股期貨_到期月份_買方_前五大交易人合計_部位數
	"value2 FLOAT", // 臺股期貨_到期月份_買方_前五大交易人合計_百分比
	"value3 INT", // 臺股期貨_到期月份_買方_前十大交易人合計_部位數
	"value4 FLOAT", // 臺股期貨_到期月份_買方_前十大交易人合計_百分比
	"value5 INT", // 臺股期貨_到期月份_賣方_前五大交易人合計_部位數
	"value6 FLOAT", // 臺股期貨_到期月份_賣方_前五大交易人合計_百分比
	"value7 INT", // 臺股期貨_到期月份_賣方_前十大交易人合計_部位數
	"value8 FLOAT", // 臺股期貨_到期月份_賣方_前十大交易人合計_百分比
	"value9 INT", // 臺股期貨_到期月份_全市場未沖銷部位數
	"value10 INT", // 臺股期貨_所有契約_買方_前五大交易人合計_部位數
	"value11 FLOAT", // 臺股期貨_所有契約_買方_前五大交易人合計_百分比
	"value12 INT", // 臺股期貨_所有契約_買方_前十大交易人合計_部位數
	"value13 FLOAT", // 臺股期貨_所有契約_買方_前十大交易人合計_百分比
	"value14 INT", // 臺股期貨_所有契約_賣方_前五大交易人合計_部位數
	"value15 FLOAT", // 臺股期貨_所有契約_賣方_前五大交易人合計_百分比
	"value16 INT", // 臺股期貨_所有契約_賣方_前十大交易人合計_部位數
	"value17 FLOAT", // 臺股期貨_所有契約_賣方_前十大交易人合計_百分比
	"value18 INT", // 臺股期貨_所有契約_全市場未沖銷部位數
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return values
const unsigned short RET_SUCCESS = 0;

const unsigned short RET_FAILURE_UNKNOWN = 1;
const unsigned short RET_FAILURE_INVALID_ARGUMENT = 2;
const unsigned short RET_FAILURE_INVALID_POINTER = 3;
const unsigned short RET_FAILURE_INSUFFICIENT_MEMORY = 4;
const unsigned short RET_FAILURE_INCORRECT_OPERATION = 5;
const unsigned short RET_FAILURE_OPEN_FILE = 6;
const unsigned short RET_FAILURE_NOT_FOUND = 7;
const unsigned short RET_FAILURE_INCORRECT_CONFIG = 8;
const unsigned short RET_FAILURE_INCORRECT_PATH = 9;
const unsigned short RET_FAILURE_IO_OPERATION = 10;
const unsigned short RET_FAILURE_HANDLE_THREAD = 11;
const unsigned short RET_FAILURE_SYSTEM_API = 12;
const unsigned short RET_FAILURE_MYSQL = 13;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class

TimeCfg::TimeCfg(const char* cur_time_str)
{
	static const char *DELIM = "-";
	if (cur_time_str == NULL)
		throw invalid_argument(string("cur_time_str should NOT be NULL"));
	snprintf(time_str, 16, "%s", cur_time_str);

	char * pch;
	pch = strtok(time_str, DELIM);
	int count = 0;
	while (pch != NULL)
	{
		switch (count)
		{
		case 0:
			year = atoi(pch);
			break;
		case 1:
			month = atoi(pch);
			break;
		case 2:
			day = atoi(pch);
			break;
		}
//		printf ("%s, %d\n", pch, atoi(pch));
		pch = strtok(NULL, DELIM);
		count++;
	}
	if (count == 2)
		time_type = TIME_MONTH;
	else if (count == 3)
		time_type = TIME_DATE;
	else
	{
		char errmsg[64];
		snprintf(errmsg, 64, "Incorrect time format: %s, count: %d", time_str, count);
		throw invalid_argument(errmsg);
	}
}

TimeCfg::TimeCfg(int cur_year, int cur_month)
{
	year = cur_year;
	month = cur_month;
	snprintf(time_str, 16, "%04d-%02d", year, month);
	time_type = TIME_MONTH;
}

TimeCfg::TimeCfg(int cur_year, int cur_month, int cur_day)
{
	year = cur_year;
	month = cur_month;
	day = cur_day;
	snprintf(time_str, 16, "%04d-%02d-%02d", year, month, day);
	time_type = TIME_DATE;
}

int TimeCfg::get_year()const{return year;}
int TimeCfg::get_month()const{return month;}
int TimeCfg::get_day()const{return day;}
const char* TimeCfg::to_string()const{return time_str;}
bool TimeCfg::is_month_type()const{return (time_type == TIME_MONTH);}


TimeRangeCfg::TimeRangeCfg(const char* time_start_str, const char* time_end_str)
{
	if (time_start_str != NULL)
	{
		time_start_cfg = new TimeCfg(time_start_str);
		if (time_start_cfg == NULL)
			throw bad_alloc();
	}
	if (time_end_str != NULL)
	{
		time_end_cfg = new TimeCfg(time_end_str);
		if (time_end_cfg == NULL)
			throw bad_alloc();
	}

	if (time_start_cfg != NULL && time_end_cfg != NULL)
	{
		if (time_start_cfg->is_month_type() != time_end_cfg->is_month_type())
		{
			char errmsg[64];
			snprintf(errmsg, 64, "The time format is NOT identical, start: %s, end: %s", time_start_cfg->to_string(), time_end_cfg->to_string());
			throw invalid_argument(errmsg);
		}
		type_is_month = time_start_cfg->is_month_type();
	}
	else if (time_start_cfg != NULL)
		type_is_month = time_start_cfg->is_month_type();
	else if (time_end_cfg != NULL)
		type_is_month = time_end_cfg->is_month_type();
	else
		throw invalid_argument("time_start_str and time_end_str should NOT be NULL simultaneously");
}

TimeRangeCfg::TimeRangeCfg(int year_start, int month_start, int year_end, int month_end)
{
	time_start_cfg = new TimeCfg(year_start, month_start);
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(year_end, month_end);
	if (time_end_cfg == NULL)
		throw bad_alloc();
	type_is_month = true;
}

TimeRangeCfg::TimeRangeCfg(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end)
{
	time_start_cfg = new TimeCfg(year_start, month_start, year_end);
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(year_end, month_end, day_end);
	if (time_end_cfg == NULL)
		throw bad_alloc();
	type_is_month = false;
}

TimeRangeCfg::~TimeRangeCfg()
{
	if (time_start_cfg != NULL)
	{
		delete time_start_cfg;
		time_start_cfg = NULL;
	}
	if (time_end_cfg != NULL)
	{
		delete time_end_cfg;
		time_end_cfg = NULL;
	}
	if (time_range_description != NULL)
	{
		delete time_range_description;
		time_range_description = NULL;
	}
}

const char* TimeRangeCfg::to_string()
{
	if (time_range_description != NULL)
	{
		time_range_description = new char[32];
		if (time_range_description == NULL)
			throw bad_alloc();
		if (time_start_cfg != NULL && time_end_cfg != NULL)
			snprintf(time_range_description, 32, "%s:%s", time_start_cfg->to_string(), time_end_cfg->to_string());
		else if (time_start_cfg != NULL)
			snprintf(time_range_description, 32, "%s", time_start_cfg->to_string());
		else if (time_end_cfg != NULL)
			snprintf(time_range_description, 32, "%s", time_end_cfg->to_string());
	}
	return time_range_description;
}

const PTIME_CFG TimeRangeCfg::get_start_time(){return time_start_cfg;}
const PTIME_CFG TimeRangeCfg::get_end_time(){return time_end_cfg;}
