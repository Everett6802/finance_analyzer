#include <assert.h>
#include <string>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


using namespace std;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
int FinanceDataArrayBase<T>::DEF_ARRAY_SIZE = 512;

template <typename T>
FinanceDataArrayBase<T>::FinanceDataArrayBase() :
	array_size(DEF_ARRAY_SIZE),
	array_pos(0)
{
	array_data = calloc(array_size, sizeof(T));
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
FinanceDataArrayBase<T>::~FinanceDataArrayBase()
{
	if (array_data != NULL)
	{
		free(array_data);
		array_data = NULL;
	}
}

template <typename T>
void FinanceDataArrayBase<T>::add(T data)
{
	if (array_pos + 1 >= array_size)
	{
		T* array_data_old = array_data;
		array_size <<= 1;
		array_data = realloc(array_data_old, sizeof(T) * array_size);
		if (array_data == NULL)
			throw bad_alloc();
	}
	array_data[array_pos++] = data;
}

template <typename T>
T FinanceDataArrayBase<T>::operator[](int index)const
{
	assert(array_data != NULL && "array_data == NULL");
	assert((index >= 0 && index < array_pos) && "index is out of range");
	return array_data[index];
}

template <typename T>
bool FinanceDataArrayBase<T>::is_empty()const{return (array_pos == 0);}

template <typename T>
int FinanceDataArrayBase<T>::get_size()const{return array_pos;}

template <typename T>
int FinanceDataArrayBase<T>::get_array_size()const{return array_size;}

template <typename T>
const T* FinanceDataArrayBase<T>::get_data_array()const{return array_data;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
FinanceDataPtrArrayBase<T>::~FinanceDataPtrArrayBase()
{
	if (FinanceDataArrayBase<T>::array_data != NULL)
	{
		for (int i = 0 ; i < FinanceDataArrayBase<T>::array_pos ; i++)
		{
			free(FinanceDataArrayBase<T>::array_data[i]);
			FinanceDataArrayBase<T>::array_data[i] = NULL;
		}
	}
}
