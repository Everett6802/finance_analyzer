#include <assert.h>
#include <math.h>
#include <string>
#include <stdexcept>
#include <new>
#include <algorithm>
#include "finance_analyzer_common_class_base.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

// template <typename T>
// SmartPointer<T>::SmartPointer() :
// 	need_release(true)
// {
// 	data_ptr = NULL;
// }

// template <typename T>
// SmartPointer<T>::SmartPointer(T* ptr) :
// 	need_release(true)
// {
// 	data_ptr = ptr;
// }

// template <typename T>
// SmartPointer<T>::~SmartPointer()
// {
// 	if (need_release && data_ptr != NULL)
// 	{
// 		delete data_ptr;
// 		data_ptr = NULL;
// 	}
// }

// template <typename T>
// T& SmartPointer<T>::operator*()
// {
// 	return *data_ptr;
// }

// template <typename T>
// const T& SmartPointer<T>::operator*() const
// {
// 	return *data_ptr;
// }

// template <typename T>
// T* SmartPointer<T>::operator->()
// {
// 	return data_ptr;
// }

// template <typename T>
// const T* SmartPointer<T>::operator->() const
// {
// 	return data_ptr;
// }

// template <typename T>
// void SmartPointer<T>::set_new(T* ptr)
// {
// 	if (data_ptr != NULL)
// 	{
// 		delete data_ptr;
// 		data_ptr = NULL;
// 	}
// 	data_ptr = ptr;
// }

// template <typename T>
// T* SmartPointer<T>::get_instance()
// {
// 	return data_ptr;
// }

// template <typename T>
// const T* SmartPointer<T>::get_const_instance()const
// {
// 	return (const T*)data_ptr;
// }

// template <typename T>
// void SmartPointer<T>::disable_release(){need_release = false;}

// template class SmartPointer<int>;
// template class SmartPointer<long>;
// template class SmartPointer<float>;
// template class SmartPointer<TimeCfg>;
// template class SmartPointer<TimeRangeCfg>;
// template class SmartPointer<QuerySet>;
// template class SmartPointer<ResultSet>;
// template class SmartPointer<ResultSetAccessParam>;
// template class SmartPointer<FinanceIntDataArray>;
// template class SmartPointer<FinanceLongDataArray>;
// template class SmartPointer<FinanceFloatDataArray>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TimeCfg::parse_time_string(const char* time_str, int&year, int&month, int& day, TimeType& time_type)
{
	static const char *DELIM = "-";
	if (time_str == NULL)
		throw invalid_argument(string("time_str should NOT be NULL"));

	char time_tmp_str[16];
	memset(time_tmp_str, 0x0, sizeof(char) * 16);
	memcpy(time_tmp_str, time_str, sizeof(char) * strlen(time_str));
	char* pch;
	pch = strtok(time_tmp_str, DELIM);
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
	{
		day = 0;
		time_type = TIME_MONTH;
	}
	else if (count == 3)
		time_type = TIME_DATE;
	else
	{
		char errmsg[64];
		snprintf(errmsg, 64, "Incorrect time format: %s, count: %d", time_str, count);
		throw invalid_argument(errmsg);
	}
}

int TimeCfg::get_int_value(int year, int month, int day)
{
	return ((year & 0xFFFF) << 16) | ((month & 0xFF) << 8) | (day & 0xFF);
}

int TimeCfg::get_int_value(const TimeCfg* time_cfg)
{
	assert(time_cfg != NULL && "time_cfg should NOT be NULL");
	return get_int_value(time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day());
}

int TimeCfg::get_int_value(const char* time_str)
{
	assert(time_str != NULL && "time_str should NOT be NULL");
	int year, month, day;
	TimeType time_type;
	parse_time_string(time_str, year, month, day, time_type);
	return get_int_value(year, month, day);
}

TimeCfg::TimeCfg(const char* cur_time_str)
{
	IMPLEMENT_MSG_DUMPER()
	parse_time_string(cur_time_str, year, month, day, time_type);
	snprintf(time_str, 16, "%s", cur_time_str);
}

TimeCfg::TimeCfg(int cur_year, int cur_month)
{
	IMPLEMENT_MSG_DUMPER()
	year = cur_year;
	month = cur_month;
	day = 0;
	snprintf(time_str, 16, "%04d-%02d", year, month);
	time_type = TIME_MONTH;
}

TimeCfg::TimeCfg(int cur_year, int cur_month, int cur_day)
{
	IMPLEMENT_MSG_DUMPER()
	year = cur_year;
	month = cur_month;
	day = cur_day;
	snprintf(time_str, 16, "%04d-%02d-%02d", year, month, day);
	time_type = TIME_DATE;
}

TimeCfg::TimeCfg(const TimeCfg* another_time_cfg)
{
	IMPLEMENT_MSG_DUMPER()
	assert(another_time_cfg != NULL && "another_time_cfg should NOT be NULL");
	time_type = another_time_cfg->is_month_type() ? TIME_MONTH : TIME_DATE;
	year = another_time_cfg->get_year();
	month = another_time_cfg->get_month();
	day = another_time_cfg->is_month_type() ? 0 : another_time_cfg->get_day();
	if (time_type == TIME_MONTH)
		snprintf(time_str, 16, "%04d-%02d", year, month);
	else
		snprintf(time_str, 16, "%04d-%02d-%02d", year, month, day);
}

TimeCfg::~TimeCfg()
{
	RELEASE_MSG_DUMPER()
}

TimeCfg& TimeCfg::operator=(const TimeCfg& another)
{
	IMPLEMENT_MSG_DUMPER()

	if (this == &another)
		return *this;

	year = another.year;
	month = another.month;
	day = another.day;
	time_type = another.time_type;
	if (time_type == TIME_MONTH)
		snprintf(time_str, 16, "%04d-%02d", year, month);
	else
		snprintf(time_str, 16, "%04d-%02d-%02d", year, month, day);
	return *this;
}

int TimeCfg::get_year()const{return year;}
int TimeCfg::get_month()const{return month;}
int TimeCfg::get_day()const{return day;}
const char* TimeCfg::to_string()const{return time_str;}
bool TimeCfg::is_month_type()const{return (time_type == TIME_MONTH);}

bool TimeCfg::equal_to(const TimeCfg* another_time_cfg)const
{
	assert(another_time_cfg != NULL && "another_time_cfg should NOT be NULL");
	if (year != another_time_cfg->get_year())
		return false;
	if (month != another_time_cfg->get_month())
		return false;
	if (!is_month_type() && day != another_time_cfg->get_day())
		return false;
	return true;
}

void TimeCfg::reset(const char* new_time_str)
{
	assert(new_time_str != NULL && "new_time_str should NOT be NULL");
	parse_time_string(new_time_str, year, month, day, time_type);
	snprintf(time_str, 16, "%s", new_time_str);
}

void TimeCfg::reset(int new_year, int new_month)
{
	year = new_year;
	month = new_month;
	day = 0;
	snprintf(time_str, 16, "%04d-%02d", year, month);
	time_type = TIME_MONTH;
}

void TimeCfg::reset(int new_year, int new_month, int new_day)
{
	year = new_year;
	month = new_month;
	day = new_day;
	snprintf(time_str, 16, "%04d-%02d-%02d", year, month, day);
	time_type = TIME_DATE;
}

void TimeCfg::reset(const TimeCfg* new_time_cfg)
{
	assert(new_time_cfg != NULL && "new_time_cfg should NOT be NULL");
	*this = *new_time_cfg;
}

bool TimeCfg::operator<(const TimeCfg& another_time_cfg)const
{
	if (this == &another_time_cfg)
		return false;
	assert(time_type == another_time_cfg.time_type && "The time_type is NOT identical");
//	fprintf(stderr, "< %d, %d, %s\n", TimeCfg::get_int_value(year, month, day), TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day), ((TimeCfg::get_int_value(year, month, day) < TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day) ? "True" : "False")));
	return (TimeCfg::get_int_value(year, month, day) < TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day));
}

bool TimeCfg::operator>(const TimeCfg& another_time_cfg)const
{
	if (this == &another_time_cfg)
		return false;
	assert(time_type == another_time_cfg.time_type && "The time_type is NOT identical");
//	fprintf(stderr, "> %d, %d, %s\n", TimeCfg::get_int_value(year, month, day), TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day), ((TimeCfg::get_int_value(year, month, day) > TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day) ? "True" : "False")));
	return (TimeCfg::get_int_value(year, month, day) > TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day));
}

bool TimeCfg::operator==(const TimeCfg& another_time_cfg)const
{
	if (this == &another_time_cfg)
		return false;
	assert(time_type == another_time_cfg.time_type && "The time_type is NOT identical");
//	fprintf(stderr, "== %d, %d, %s\n", TimeCfg::get_int_value(year, month, day), TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day), ((TimeCfg::get_int_value(year, month, day) < TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day) ? "True" : "False")));
	return (TimeCfg::get_int_value(year, month, day) == TimeCfg::get_int_value(another_time_cfg.year, another_time_cfg.month, another_time_cfg.day));
}

bool TimeCfg::operator>=(const TimeCfg& another_time_cfg)const {return !(*this < another_time_cfg);}
bool TimeCfg::operator<=(const TimeCfg& another_time_cfg)const {return !(*this > another_time_cfg);}
bool TimeCfg::operator!=(const TimeCfg& another_time_cfg)const {return !(*this == another_time_cfg);}

bool TimeCfg::operator<(const char* another_time_str)const
{
	TimeType another_time_type;
	int another_year, another_month, another_day;
	TimeCfg::parse_time_string(another_time_str, another_year, another_month, another_day, another_time_type);
	assert(time_type == another_time_type && "The time_type is NOT identical");
	return (TimeCfg::get_int_value(year, month, day) < TimeCfg::get_int_value(another_year, another_month, another_day));
}

bool TimeCfg::operator>(const char* another_time_str)const
{
	TimeType another_time_type;
	int another_year, another_month, another_day;
	TimeCfg::parse_time_string(another_time_str, another_year, another_month, another_day, another_time_type);
	assert(time_type == another_time_type && "The time_type is NOT identical");
	return (TimeCfg::get_int_value(year, month, day) > TimeCfg::get_int_value(another_year, another_month, another_day));
}

bool TimeCfg::operator==(const char* another_time_str)const
{
	TimeType another_time_type;
	int another_year, another_month, another_day;
	TimeCfg::parse_time_string(another_time_str, another_year, another_month, another_day, another_time_type);
	assert(time_type == another_time_type && "The time_type is NOT identical");
	return (TimeCfg::get_int_value(year, month, day) == TimeCfg::get_int_value(another_year, another_month, another_day));
}

bool TimeCfg::operator>=(const char* another_time_str)const {return !(*this < another_time_str);}
bool TimeCfg::operator<=(const char* another_time_str)const {return !(*this > another_time_str);}
bool TimeCfg::operator!=(const char* another_time_str)const {return !(*this == another_time_str);}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TimeRangeCfg::time_in_range(const TimeRangeCfg* time_range_cfg, const TimeCfg* time_cfg)
{
	assert(time_range_cfg != NULL && "time_range_cfg should NOT be NULL");
	assert(time_range_cfg->get_start_time() != NULL && "start time in time_range_cfg should NOT be NULL");
	assert(time_range_cfg->get_end_time() != NULL && "end time in time_range_cfg should NOT be NULL");
	assert(time_cfg != NULL && "time_cfg should NOT be NULL");
	return (*time_cfg >= *(time_range_cfg->get_start_time()) && *time_cfg <= *(time_range_cfg->get_end_time()));
}

bool TimeRangeCfg::time_in_range(const TimeRangeCfg* time_range_cfg, int year, int month, int day)
{
	TimeCfg *time_cfg = new TimeCfg(year, month, day);
	return time_in_range(time_range_cfg, time_cfg);
}

unsigned short TimeRangeCfg::create_instance_from_string(const char* source_string, TimeRangeCfg& time_range_cfg)
{
	assert(source_string != NULL && "soruce_string should NOT be NULL");
	int source_string_copy_size = strlen(source_string) + 1;
	char* source_string_copy = new char[source_string_copy_size];
	if (source_string_copy == NULL)
	{
		STATIC_WRITE_ERROR("Fail to allocate the memory: source_string_copy");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	memcpy(source_string_copy, source_string, sizeof(char) * source_string_copy_size);
	unsigned short ret = RET_SUCCESS;
	char* time_start_str = NULL; 
	char* time_end_str = NULL;
	char* comma_pos = strchr(source_string_copy, ',');
	if (comma_pos == NULL)
	{
// Only the start time
		time_start_str = source_string_copy;
	}
	else
	{
		int comma_index = comma_pos - source_string_copy;
		if (comma_index == 0)
		{
// Only the end time
			time_end_str = &source_string_copy[1];
		}
		else
		{
// The time range
			source_string_copy[comma_index] = '\0';
			time_start_str = source_string_copy;
			time_end_str = &source_string_copy[comma_index + 1];
		}
	}
	time_range_cfg.add_time(time_start_str, time_end_str);
	ret = time_range_cfg.add_time_done();
	if (source_string_copy != NULL)
	{
		delete source_string_copy;
		source_string_copy = NULL;
	}
	return ret;
}

unsigned short TimeRangeCfg::create_instance_from_string(const char* source_string, TimeRangeCfg** time_range_cfg)
{
	assert(time_range_cfg != NULL && "time_range_cfg should NOT be NULL");
	TimeRangeCfg* time_range_cfg_tmp = new TimeRangeCfg();
	if (time_range_cfg_tmp == NULL)
	{
		STATIC_WRITE_ERROR("Fail to allocate the memory: time_range_cfg_tmp");
		if (time_range_cfg_tmp != NULL) delete time_range_cfg_tmp;
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	unsigned short ret = create_instance_from_string(source_string, *time_range_cfg_tmp);
	if (CHECK_FAILURE(ret))
	{
		delete time_range_cfg_tmp;
		time_range_cfg_tmp = NULL;
	}
	*time_range_cfg = time_range_cfg_tmp;
	return ret;
}

TimeRangeCfg::TimeRangeCfg() :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false),
	add_done(false)
{
	IMPLEMENT_MSG_DUMPER()
}

TimeRangeCfg::TimeRangeCfg(const char* time_start_str, const char* time_end_str) :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()

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

TimeRangeCfg::TimeRangeCfg(int year_start, int month_start, int year_end, int month_end) :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()
	time_start_cfg = new TimeCfg(year_start, month_start);
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(year_end, month_end);
	if (time_end_cfg == NULL)
		throw bad_alloc();
	type_is_month = true;
}

TimeRangeCfg::TimeRangeCfg(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end) :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()
	time_start_cfg = new TimeCfg(year_start, month_start, day_start);
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(year_end, month_end, day_end);
	if (time_end_cfg == NULL)
		throw bad_alloc();
	type_is_month = false;
}

TimeRangeCfg::TimeRangeCfg(const TimeRangeCfg& another_time_range_cfg) :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()
	time_start_cfg = new TimeCfg(*another_time_range_cfg.get_start_time());
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(*another_time_range_cfg.get_end_time());
	if (time_end_cfg == NULL)
		throw bad_alloc();
	assert(time_start_cfg->is_month_type() == time_end_cfg->is_month_type() && "The month type of start/end time config is NOT identical");
	type_is_month = time_start_cfg->is_month_type();
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
	RELEASE_MSG_DUMPER()
}

void TimeRangeCfg::add_start_time(const PTIME_CFG new_start_time_cfg)
{
	if (add_done)
	{
		WRITE_ERROR("the add_done flag is true");
		throw runtime_error(string("the add_done flag is true"));
	}
	assert(new_start_time_cfg != NULL && "new_start_time_cfg should NOT be NULL");
	time_start_cfg = new TimeCfg(new_start_time_cfg);
	if (time_start_cfg == NULL)
		throw bad_alloc();
}

void TimeRangeCfg::add_end_time(const PTIME_CFG new_end_time_cfg)
{
	if (add_done)
	{
		WRITE_ERROR("the add_done flag is true");
		throw runtime_error(string("the add_done flag is true"));
	}
	assert(new_end_time_cfg != NULL && "new_end_time_cfg should NOT be NULL");
	time_end_cfg = new TimeCfg(new_end_time_cfg);
	if (time_end_cfg == NULL)
		throw bad_alloc();
}

void TimeRangeCfg::add_time(const PTIME_CFG new_start_time_cfg, const PTIME_CFG new_end_time_cfg)
{
	assert((new_start_time_cfg != NULL || new_end_time_cfg != NULL) && "new_start_time_cfg/new_end_time_cfg should NOT be both NULL");
	if (new_start_time_cfg != NULL)
		add_start_time(new_start_time_cfg);
	if (new_end_time_cfg != NULL)
		add_end_time(new_end_time_cfg);
}

void TimeRangeCfg::add_start_time(const char* new_start_time_str)
{
	assert(new_start_time_str != NULL && "new_start_time_str should NOT be NULL");
	TimeCfg new_start_time_cfg(new_start_time_str);
	add_start_time((const PTIME_CFG)&new_start_time_cfg);
}

void TimeRangeCfg::add_end_time(const char* new_end_time_str)
{
	assert(new_end_time_str != NULL && "new_end_time_str should NOT be NULL");
	TimeCfg new_end_time_cfg(new_end_time_str);
	add_end_time((const PTIME_CFG)&new_end_time_cfg);
}

void TimeRangeCfg::add_time(const char* new_start_time_str, const char* new_end_time_str)
{
	assert((new_start_time_str != NULL || new_end_time_str != NULL) && "new_start_time_str/new_end_time_str should NOT be both NULL");
	if (new_start_time_str != NULL)
		add_start_time(new_start_time_str);
	if (new_end_time_str != NULL)
		add_end_time(new_end_time_str);
}

unsigned short TimeRangeCfg::add_time_done()
{
	if (add_done)
	{
		WRITE_ERROR("the add_done flag is true");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	if (time_range_description != NULL)
	{
		delete time_range_description;
		time_range_description = NULL;
	}
	add_done = true;
	return RET_SUCCESS;
}

void TimeRangeCfg::reset_start_time(const PTIME_CFG new_start_time_cfg)
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
	assert(new_start_time_cfg != NULL && "new_start_time_cfg should NOT be NULL");
	add_done = false;
	if (time_start_cfg == NULL)
		add_start_time(new_start_time_cfg);
	else
		time_start_cfg->reset(new_start_time_cfg);
	add_time_done();
}

void TimeRangeCfg::reset_end_time(const PTIME_CFG new_end_time_cfg)
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
	assert(new_end_time_cfg != NULL && "new_end_time_cfg should NOT be NULL");
	add_done = false;
	if (time_end_cfg == NULL)
		add_end_time(new_end_time_cfg);
	else
		time_end_cfg->reset(new_end_time_cfg);
	add_time_done();
}

void TimeRangeCfg::reset_time(const PTIME_CFG new_start_time_cfg, const PTIME_CFG new_end_time_cfg)
{
	if (new_start_time_cfg != NULL)
		reset_start_time(new_start_time_cfg);
	if (new_end_time_cfg != NULL)
		reset_end_time(new_end_time_cfg);
}

void TimeRangeCfg::reset_start_time(const char* new_start_time_str)
{
	assert(new_start_time_str != NULL && "new_start_time_str should NOT be NULL");
	TimeCfg new_start_time_cfg(new_start_time_str);
	reset_start_time((const PTIME_CFG)&new_start_time_cfg);
}

void TimeRangeCfg::reset_end_time(const char* new_end_time_str)
{
	assert(new_end_time_str != NULL && "new_end_time_str should NOT be NULL");
	TimeCfg new_end_time_cfg(new_end_time_str);
	reset_end_time((const PTIME_CFG)&new_end_time_cfg);
}

void TimeRangeCfg::reset_time(const char* new_start_time_str, const char* new_end_time_str)
{
	if (new_start_time_str != NULL)
		reset_start_time(new_start_time_str);
	if (new_end_time_str != NULL)
		reset_end_time(new_end_time_str);
}

bool TimeRangeCfg::is_month_type()const{return type_is_month;}

bool TimeRangeCfg::is_single_time()const
{
	if (time_start_cfg != NULL && time_end_cfg != NULL)
		return time_start_cfg->equal_to(time_end_cfg);
	return false;
}

bool TimeRangeCfg::is_add_time_done()const{return add_done;}

const char* TimeRangeCfg::to_string()
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
	if (time_range_description == NULL)
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

const PTIME_CFG TimeRangeCfg::get_start_time()const
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
//	assert(time_start_cfg != NULL && "time_start_cfg should NOT be NULL");
	return time_start_cfg;
}

const PTIME_CFG TimeRangeCfg::get_end_time()const
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
//	assert(time_end_cfg != NULL && "time_end_cfg should NOT be NULL");
	return time_end_cfg;
}

// PTIME_CFG TimeRangeCfg::get_start_time()
// {
// //	assert(time_start_cfg != NULL && "time_start_cfg should NOT be NULL");
// 	return time_start_cfg;
// }

// PTIME_CFG TimeRangeCfg::get_end_time()
// {
// //	assert(time_end_cfg != NULL && "time_end_cfg should NOT be NULL");
// 	return time_end_cfg;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SingleTimeRangeCfg::SingleTimeRangeCfg(const char* time_str) : TimeRangeCfg(time_str, time_str){} // Single day
SingleTimeRangeCfg::SingleTimeRangeCfg(int year, int month) : TimeRangeCfg(year, month, year, month){} // Single month
SingleTimeRangeCfg::SingleTimeRangeCfg(int year, int month, int day) : TimeRangeCfg(year, month, day, year, month, day){} // Single day

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int FinanceDataArrayBase::DEF_ARRAY_SIZE = 512;

FinanceDataArrayBase::FinanceDataArrayBase() :
	array_type(FinanceField_UNKNOWN),
	array_size(DEF_ARRAY_SIZE),
	array_pos(0)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceDataArrayBase::~FinanceDataArrayBase()
{
	RELEASE_MSG_DUMPER()
}

void FinanceDataArrayBase::set_type(FinanceFieldType type){array_type = type;}
FinanceFieldType FinanceDataArrayBase::get_type()const{return array_type;}
bool FinanceDataArrayBase::is_empty()const{return (array_pos == 0);}
int FinanceDataArrayBase::get_size()const{return array_pos;}
int FinanceDataArrayBase::get_array_size()const{return array_size;}
void FinanceDataArrayBase::reset_array(){array_pos = 0;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//template <typename T>
//int FinanceDataArrayTemplate<T>::DEF_ARRAY_SIZE = 512;

template <typename T>
FinanceDataArrayTemplate<T>::FinanceDataArrayTemplate()
{
//	IMPLEMENT_MSG_DUMPER()
	array_size = DEF_ARRAY_SIZE;
	array_pos = 0;
	array_data = (T*)calloc(array_size, sizeof(T));
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
FinanceDataArrayTemplate<T>::FinanceDataArrayTemplate(const FinanceDataArrayTemplate& another)
{
	array_size = another.array_size;
	array_data = (T*)malloc(sizeof(T) * array_size);
	if (array_data == NULL)
		throw bad_alloc();
	array_pos = another.array_pos;
	memcpy(array_data, another.array_data, sizeof(T) * array_pos);
}

template <typename T>
FinanceDataArrayTemplate<T>::~FinanceDataArrayTemplate()
{
	if (array_data != NULL)
	{
		free(array_data);
		array_data = NULL;
	}
//	RELEASE_MSG_DUMPER()
}

template <typename T>
void FinanceDataArrayTemplate<T>::alloc_new(int new_array_size)
{
	T* array_data_old = array_data;
	do{
		array_size <<= 1;
	}while(array_size >= new_array_size);
// Restrict the memory size.
	if (new_array_size * sizeof(T) > MAX_MEMORY_SIZE)
	{
		array_size = (int)(MAX_MEMORY_SIZE / sizeof(T));
		over_max_memroy_size = true;
	}
	array_data = (T*)realloc(array_data_old, sizeof(T) * array_size);
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
bool FinanceDataArrayTemplate<T>::is_over_max_memroy_size()const{return over_max_memroy_size;}

template <typename T>
void FinanceDataArrayTemplate<T>::set_data_array(const T* array, int size)
{
	assert(array_data != NULL && "array_data should NOT be NULL");
	assert(array != NULL && "array should NOT be NULL");

	bool need_allocate_memory = (size > array_size ? true : false);
	if (need_allocate_memory)
		alloc_new(size);
	memcpy(array_data, array, sizeof(T) * size);
	array_pos = size;
}

template <typename T>
const T* FinanceDataArrayTemplate<T>::get_data_array()const{return array_data;}

template <typename T>
void FinanceDataArrayTemplate<T>::add_data(T data)
{
	if (array_pos + 1 >= array_size)
		alloc_new(array_pos + 1);
	array_data[array_pos++] = data;
}

template <typename T>
void FinanceDataArrayTemplate<T>::add_data_array(const T* data, int size)
{
	if (array_pos + size >= array_size)
		alloc_new(array_pos + size);
	memcpy(&array_data[array_pos], data, sizeof(T) * size);
	array_pos += size;
}
// template <typename T>
// unsigned short FinanceDataArrayTemplate<T>::get_data_range(T& data_min, T& data_max)const
// {
// 	if (is_empty())
// 	{
// 		WRITE_ERROR("This array is Empty");
// 		return RET_FAILURE_INCORRECT_OPERATION;
// 	}
// 	data_min = array_data[0];
// 	data_max = array_data[0];
// 	for (int i = 1 ; i < array_pos ; i++)
// 	{
// 		if (data_min > array_data[i])
// 			data_min = array_data[i];
// 		if (data_max < array_data[i])
// 			data_max = array_data[i];
// //		printf("%d %d %d\n", data_min, data_max, array_data[i]);
// 	}
// 	return RET_SUCCESS;
// }

// template <typename T>
// unsigned short FinanceDataArrayTemplate<T>::get_histogram_interval(int interval_amount, SmartPointer<T> &sp_histogram_interval)const
// {
// 	if (interval_amount == 0)
// 	{
// 		WRITE_ERROR("The histogram interval amount should NOT be 0");
// 		return RET_FAILURE_INVALID_ARGUMENT;
// 	}
// 	T data_min, data_max;
// 	unsigned short ret = get_data_range(data_min, data_max);
// 	if (CHECK_FAILURE(ret))
// 		return ret;
// 	T interval_size = (T)(data_max - data_min) / interval_amount;
// 	T* histogram_interval = new T [interval_amount + 1];
// 	assert(histogram_interval != NULL && "histogram_interval should NOT be NULL");
// 	*histogram_interval = data_min;
// 	for (int i = 1 ; i < interval_amount ; i++)
// 		*(histogram_interval + i) = (T)(data_min + interval_size * i);
// 	*(histogram_interval + interval_amount) = data_max;
// 	sp_histogram_interval.set_new(histogram_interval);
// 	return ret;
// }

// template <typename T>
// unsigned short FinanceDataArrayTemplate<T>::get_histogram(int interval_amount, const T* histogram_interval, SmartPointer<int> &sp_histogram_statistics)const
// {
// 	int* histogram_statistics = new int[interval_amount + 1];
// 	assert(histogram_statistics != NULL && "histogram_statistics should NOT be NULL");
// 	memset(histogram_statistics, 0x0, sizeof(int) * (interval_amount + 1));
	
// 	int interval_index;
// 	for (int i = 0 ; i < array_pos ; i++)
// 	{
// 		interval_index = binary_search_interval(histogram_interval, array_data[i], 0, interval_amount);
// 		if (interval_index < 0 || interval_index > interval_amount)
// 		{
// 			WRITE_FORMAT_ERROR("Fail to find the correct histogram index[%d]", interval_index);
// 			return RET_FAILURE_NOT_FOUND;
// 		}
// 		histogram_statistics[interval_index]++;
// 	}
// 	sp_histogram_statistics.set_new(histogram_statistics);
// 	return RET_SUCCESS;
// }

// template <typename T>
// unsigned short FinanceDataArrayTemplate<T>::get_histogram(int interval_amount, SmartPointer<int> &sp_histogram_statistics)const
// {
// 	SmartPointer<T> sp_histogram_interval;
// 	unsigned short ret = get_histogram_interval(interval_amount, sp_histogram_interval);
// 	if (CHECK_FAILURE(ret))
// 		return ret;
// 	return get_histogram(interval_amount, (const T*)sp_histogram_interval.get_const_instance(), sp_histogram_statistics);
// }

template <typename T>
const T FinanceDataArrayTemplate<T>::operator[](int index)const
{
	assert(array_data != NULL && "array_data should NOT be NULL");
	static const int ERRMSG_LEN = 64;
	static char errmsg[ERRMSG_LEN];
	if (index >= 0)
	{
		if (index >= array_pos)
		{
			snprintf(errmsg, ERRMSG_LEN, "positive index[%d] is out of range: [0, %d)", index, array_pos);
			goto OUT;
		}
	}
	else if (index < 0)
	{
		if (abs(index) > array_pos)
		{
			snprintf(errmsg, ERRMSG_LEN, "negative index[%d] is out of range: (0, %d]", index, array_pos);
			goto OUT;
		}
		index = get_end_index_ex(index, array_pos);
	}

	return array_data[index];
OUT:
	WRITE_ERROR(errmsg);
	throw out_of_range(errmsg);	
}

template class FinanceDataArrayTemplate<int>;
template class FinanceDataArrayTemplate<long>;
template class FinanceDataArrayTemplate<float>;
template class FinanceDataArrayTemplate<bool>;

//template <typename T>
//unsigned short FinanceDataArrayTemplate<T>::get_sub_array(FinanceDataArrayTemplate& new_data_array, int start_index, int end_index)
//{
//	unsigned short ret = RET_SUCCESS;
////	if (sub_int_data_array == NULL)
////	{
////		WRITE_ERROR("sub_int_data_array should NOT be NULL");
////		return RET_FAILURE_INVALID_ARGUMENT;
////	}
//// Semi-open interval
//	if (end_index == -1)
//		end_index = array_pos;
//	if (start_index < 0 || start_index >= array_pos)
//	{
//		WRITE_FORMAT_ERROR("start_index is NOT in the range[0, %d)", array_pos);
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	if (end_index < 0 || end_index >= array_pos)
//	{
//		WRITE_FORMAT_ERROR("end_index is NOT in the range[0, %d)", array_pos);
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	if ((end_index - start_index) < 1)
//	{
//		WRITE_ERROR("The array size is NOT more than 1");
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
////	FinanceDataArrayTemplate* sub_int_data_array_tmp = new FinanceDataArrayTemplate();
////	if (sub_int_data_array_tmp == NULL)
////	{
////		WRITE_ERROR("Fail to allocate memory: sub_int_data_array_tmp");
////		return RET_FAILURE_INSUFFICIENT_MEMORY;
////	}
//
//	for (int i = start_index + 1 ; i < end_index ; i++)
//		new_data_array.add(array_data[i]);
////	sub_int_data_array = &sub_int_data_array_tmp;
//
//	return ret;
//}
//
//template <typename T>
//unsigned short FinanceDataArrayTemplate<T>::get_diff_array(FinanceDataArrayTemplate& new_data_array, int start_index, int end_index)
//{
//	unsigned short ret = RET_SUCCESS;
////	if (sub_int_data_array == NULL)
////	{
////		WRITE_ERROR("sub_int_data_array should NOT be NULL");
////		return RET_FAILURE_INVALID_ARGUMENT;
////	}
//// Semi-open interval
//	if (end_index == -1)
//		end_index = array_pos;
//	if (start_index < 0 || start_index >= array_pos)
//	{
//		WRITE_FORMAT_ERROR("start_index is NOT in the range[0, %d)", array_pos);
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	if (end_index < 0 || end_index >= array_pos)
//	{
//		WRITE_FORMAT_ERROR("end_index is NOT in the range[0, %d)", array_pos);
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	if ((end_index - start_index) < 2)
//	{
//		WRITE_ERROR("The array size is NOT more than 2");
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
////	FinanceDataArrayTemplate* sub_int_data_array_tmp = new FinanceDataArrayTemplate();
////	if (sub_int_data_array_tmp == NULL)
////	{
////		WRITE_ERROR("Fail to allocate memory: sub_int_data_array_tmp");
////		return RET_FAILURE_INSUFFICIENT_MEMORY;
////	}
//
//	for (int i = start_index + 1 ; i < end_index ; i++)
//		new_data_array.add(array_data[i] - array_data[i - 1]);
////	sub_int_data_array = &sub_int_data_array_tmp;
//
//	return ret;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
FinanceDataPtrArrayTemplate<T>::FinanceDataPtrArrayTemplate()
{
//	IMPLEMENT_MSG_DUMPER()
	array_size = DEF_ARRAY_SIZE;
	array_pos = 0;
	array_data = (T**)calloc(array_size, sizeof(T*));
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
FinanceDataPtrArrayTemplate<T>::FinanceDataPtrArrayTemplate(const FinanceDataPtrArrayTemplate& another)
{
	array_size = another.array_size;
	array_data = (T**)malloc(sizeof(T*) * array_size);
	if (array_data == NULL)
		throw bad_alloc();
	array_pos = another.array_pos;
	for (int i = 0 ; i < array_pos ; i++)
	{
		T* array_element_data = (T*)malloc(sizeof(T));
		if (array_element_data == NULL)
			throw bad_alloc();
		memcpy(array_element_data, another.array_data[i], sizeof(T));
		array_data[i] = array_element_data;
	}
}

template <typename T>
FinanceDataPtrArrayTemplate<T>::~FinanceDataPtrArrayTemplate()
{
	if (array_data != NULL)
	{
		for (int i = 0 ; i < array_pos ; i++)
		{
			free(array_data[i]);
			array_data[i] = NULL;
		}
		free(array_data);
		array_data = NULL;
	}
}

template <typename T>
void FinanceDataPtrArrayTemplate<T>::alloc_new(int new_array_size)
{
	T** array_data_old = array_data;
	do{
		array_size <<= 1;
	}while(array_size >= new_array_size);
	// array_size <<= 1;
	array_data = (T**)realloc(array_data_old, sizeof(T*) * array_size);
	if (array_data == NULL)
		throw bad_alloc();
}

// template <typename T>
// void FinanceDataPtrArrayTemplate<T>::set_data_array(const T** array, int size)
// {
// 	assert(array_data != NULL && "array_data should NOT be NULL");
// 	assert(array != NULL && "array should NOT be NULL");

// 	bool need_allocate_memory = (size > array_size ? true : false);
// 	if (need_allocate_memory)
// 	{
// 		do{
// 			array_size <<= 1;
// 		}while(array_size >= size);
// 		alloc_new();
// 	}
// 	memcpy(array_data, array, sizeof(T) * size);
// 	array_pos = size;
// }

// template <typename T>
// const T* FinanceDataPtrArrayTemplate<T>::get_data_array()const{return array_data;}

template <typename T>
void FinanceDataPtrArrayTemplate<T>::add_data(const T* data, size_t data_size)
{
	if (array_pos + 1 >= array_size)
		alloc_new(array_pos + 1);
	T* data_new = (T*)calloc(data_size, sizeof(char));
	if (data_new == NULL)
		throw bad_alloc();
	memcpy((void*)data_new, (void*)data, sizeof(char) * data_size);
	array_data[array_pos++] = data_new;
}

template <typename T>
void FinanceDataPtrArrayTemplate<T>::add_data_array(const T** data, size_t data_size, int size)
{
	if (array_pos + size >= array_size)
		alloc_new(array_pos + size);
	for (int i = 0 ; i < size ; i++)
	{
		T* data_new = (T*)calloc(data_size, sizeof(char));
		if (data_new == NULL)
			throw bad_alloc();
		memcpy((void*)data_new, (void*)*(data + i), sizeof(char) * data_size);
		array_data[array_pos + i] = data_new;		
	}
	array_pos += size;
}

template <typename T>
const T* FinanceDataPtrArrayTemplate<T>::operator[](int index)const
{
	assert(array_data != NULL && "array_data should NOT be NULL");
	static const int ERRMSG_LEN = 64;
	static char errmsg[ERRMSG_LEN];
	if (index >= 0)
	{
		if (index >= array_pos)
		{
			snprintf(errmsg, ERRMSG_LEN, "positive index[%d] is out of range: [0, %d)", index, array_pos);
			goto OUT;
		}
	}
	else if (index < 0)
	{
		if (abs(index) > array_pos)
		{
			snprintf(errmsg, ERRMSG_LEN, "negative index[%d] is out of range: (0, %d]", index, array_pos);
			goto OUT;
		}
		index = get_end_index_ex(index, array_pos);
	}
	return array_data[index];
OUT:
	WRITE_ERROR(errmsg);
	throw out_of_range(errmsg);	
}

template class FinanceDataPtrArrayTemplate<char>;

//const ResultSetAccessParam* FinanceResultSetAccessParamDataPtrArray::operator[](int index)const
//{
////	assert(array_data != NULL && "array_data == NULL");
////	if (index >= 0)
////	{
////		if (index >= array_pos)
////		{
////			char errmsg[64];
////			snprintf(errmsg, 64, "positive index[%d] is out of range: [0, %d)", index, array_pos);
////			WRITE_ERROR(errmsg);
////			throw out_of_range(errmsg);
////		}
////	}
////	else if (index < 0)
////	{
////		if (abs(index) > abs(array_pos))
////		{
////			char errmsg[64];
////			snprintf(errmsg, 64, "negative index[%d] is out of range: (0, %d]", index, array_pos);
////			WRITE_ERROR(errmsg);
////			throw out_of_range(errmsg);
////		}
////		index = get_end_index_ex(index, array_pos);
////	}
//
//	return array_data[index];
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_DATA_ARRAY_OPERATOR_EQUAL(m, n)\
bool Finance##m##DataArray::operator==(const Finance##m##DataArray& another)\
{\
	if (array_pos != another.array_pos)\
		return false;\
	return this->operator==(another.get_data_array());\
}\
bool Finance##m##DataArray::operator!=(const n* another_array)\
{\
	return !this->operator==(another_array);\
}\
bool Finance##m##DataArray::operator!=(const Finance##m##DataArray& another)\
{\
	return !this->operator==(another);\
}

#define IMPLEMENT_DATA_ARRAY_OPERATOR(m, n)\
Finance##m##DataArray& Finance##m##DataArray::operator=(const Finance##m##DataArray& another)\
{\
	if (this == &another)\
		return *this;\
	if (array_data != NULL)\
		delete[] array_data;\
	array_size = another.array_size;\
	array_data = (n*)malloc(sizeof(n) * array_size);\
	if (array_data == NULL)\
		throw bad_alloc();\
	array_pos = another.array_pos;\
	memcpy(array_data, another.array_data, sizeof(n) * array_pos);\
	return *this;\
}\
Finance##m##DataArray& Finance##m##DataArray::operator+=(const Finance##m##DataArray& another)\
{\
	if (this == &another)\
		return *this;\
	assert(array_data != NULL && "array_data should NOT be NULL");\
	if (array_pos != another.array_pos)\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "The dimension is NOT identical, expected: %d, actual: %d", array_pos, another.array_pos);\
		throw invalid_argument(errmsg);\
	}\
	for (int i = 0 ; i < array_pos ; i++)\
		array_data[i] += another.array_data[i];\
	return *this;\
}\
Finance##m##DataArray Finance##m##DataArray::operator+(const Finance##m##DataArray& another)\
{\
	return Finance##m##DataArray(*this) += another;\
}\
Finance##m##DataArray& Finance##m##DataArray::operator-=(const Finance##m##DataArray& another)\
{\
	if (this == &another)\
		return *this;\
	assert(array_data != NULL && "array_data should NOT be NULL");\
	if (array_pos != another.array_pos)\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "The dimension is NOT identical, expected: %d, actual: %d", array_pos, another.array_pos);\
		throw invalid_argument(errmsg);\
	}\
	for (int i = 0 ; i < array_pos ; i++)\
		array_data[i] -= another.array_data[i];\
	return *this;\
}\
Finance##m##DataArray Finance##m##DataArray::operator-(const Finance##m##DataArray& another)\
{\
	return Finance##m##DataArray(*this) -= another;\
}\
IMPLEMENT_DATA_ARRAY_OPERATOR_EQUAL(m, n)

#define IMPLEMENT_DATA_ARRAY_INTEGER_EQUAL_OPERATOR(m, n)\
bool Finance##m##DataArray::operator==(const n* another_array)\
{\
	assert(another_array != NULL && "another_array should NOT be NULL");\
	for (int i = 0 ; i < array_pos ; i++)\
	{\
		if (array_data[i] != another_array[i])\
			return false;\
	}\
	return true;\
}

#define IMPLEMENT_DATA_ARRAY_FLOAT_EQUAL_OPERATOR(m, n)\
bool Finance##m##DataArray::operator==(const n* another_array)\
{\
	static const float TOLERANCE = 0.01f;\
	assert(another_array != NULL && "another_array should NOT be NULL");\
	for (int i = 0 ; i < array_pos ; i++)\
	{\
		if (abs(array_data[i] - another_array[i]) > TOLERANCE)\
			return false;\
	}\
	return true;\
}

#define IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(m, n)\
unsigned short Finance##m##DataArray::get_sub_array(Finance##m##DataArray& new_data_array, const FinanceBoolDataArray* filter_array, int start_index, int end_index)\
{\
	if (start_index < 0 || start_index >= array_pos)\
	{\
		WRITE_FORMAT_ERROR("start_index is NOT in the range[0, %d)", array_pos);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	end_index = get_end_index_ex(end_index, array_pos);\
	if ((end_index - start_index) < 1)\
	{\
		WRITE_ERROR("The array size is NOT more than 1");\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	if (filter_array != NULL)\
	{\
		if (filter_array->get_size() != (end_index - start_index))\
		{\
			WRITE_FORMAT_ERROR("The filter array size is NOT identical to the array size, filter array: %d, array: %d", filter_array->get_size(), (end_index - start_index));\
			return RET_FAILURE_INVALID_ARGUMENT;\
		}\
	}\
	for (int i = start_index ; i < end_index ; i++)\
	{\
		if (filter_array != NULL && !(*filter_array)[i])\
			continue;\
		new_data_array.add_data(array_data[i]);\
	}\
	return RET_SUCCESS;\
}\
unsigned short Finance##m##DataArray::get_sub_array(Finance##m##DataArray& new_data_array, int start_index, int end_index)\
{\
	return get_sub_array(new_data_array, NULL, start_index, end_index);\
}\
unsigned short Finance##m##DataArray::get_diff_array(Finance##m##DataArray& new_data_array, int start_index, int end_index)\
{\
	if (start_index < 0 || start_index >= array_pos)\
	{\
		WRITE_FORMAT_ERROR("start_index is NOT in the range[0, %d)", array_pos);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	end_index = get_end_index_ex(end_index, array_pos);\
	if ((end_index - start_index) < 2)\
	{\
		WRITE_ERROR("The array size is NOT more than 2");\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	for (int i = start_index + 1; i < end_index ; i++)\
		new_data_array.add_data(array_data[i] - array_data[i - 1]);\
	return RET_SUCCESS;\
}\
unsigned short Finance##m##DataArray::get_rate_array(FinanceFloatDataArray& new_data_array, int start_index, int end_index)\
{\
	if (start_index < 0 || start_index >= array_pos)\
	{\
		WRITE_FORMAT_ERROR("start_index is NOT in the range[0, %d)", array_pos);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	end_index = get_end_index_ex(end_index, array_pos);\
	if ((end_index - start_index) < 2)\
	{\
		WRITE_ERROR("The array size is NOT more than 2");\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	for (int i = start_index + 1; i < end_index ; i++)\
	{\
		assert (array_data[i - 1] != 0 && "The data should NOT be zero");\
		new_data_array.add_data((float)(array_data[i] - array_data[i - 1])/array_data[i - 1]);\
	}\
	return RET_SUCCESS;\
}\
unsigned short Finance##m##DataArray::get_sum_array(Finance##m##DataArray& new_data_array, int N, int start_index, int end_index)\
{\
	end_index = get_end_index_ex(end_index, array_pos);\
	INDEX_IN_RANGE(start_index, end_index, 0, array_pos);\
	if ((end_index - start_index) < N)\
	{\
		WRITE_FORMAT_ERROR("The array size is NOT more than %d", N);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	n *buffer = new n[N];\
	if (buffer == NULL)\
	{\
		WRITE_ERROR("Fail to allocate memory: buffer");\
		return RET_FAILURE_INSUFFICIENT_MEMORY;\
	}\
	n sum = 0;\
	for (int i = start_index ; i < start_index + N ; i++)\
	{\
		buffer[i - start_index] = array_data[i];\
		sum += buffer[i - start_index];\
	}\
	int buffer_pos = 0;\
	int i = start_index + N;\
	do{\
		new_data_array.add_data(sum);\
		if (i >= end_index)\
			break;\
		sum -= buffer[buffer_pos];\
		buffer[buffer_pos] = array_data[i];\
		sum += buffer[buffer_pos];\
		buffer_pos ++;\
		if (buffer_pos == N)\
			buffer_pos = 0;\
		i++;\
	}while (true);\
	if (buffer != NULL)\
	{\
		delete[] buffer;\
		buffer = NULL;\
 	}\
	return RET_SUCCESS;\
}\
unsigned short Finance##m##DataArray::get_avg_array(FinanceFloatDataArray& new_data_array, int N, int start_index, int end_index)\
{\
	end_index = get_end_index_ex(end_index, array_pos);\
	INDEX_IN_RANGE(start_index, end_index, 0, array_pos);\
	if ((end_index - start_index) < N)\
	{\
		WRITE_FORMAT_ERROR("The array size is NOT more than %d", N);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	n *buffer = new n[N];\
	if (buffer == NULL)\
	{\
		WRITE_ERROR("Fail to allocate memory: buffer");\
		return RET_FAILURE_INSUFFICIENT_MEMORY;\
	}\
	n sum = (n)0;\
	for (int i = start_index ; i < start_index + N ; i++)\
	{\
		buffer[i - start_index] = array_data[i];\
		sum += buffer[i - start_index];\
	}\
	int buffer_pos = 0;\
	float average;\
	int i = start_index + N;\
	do{\
		average = (float)sum / N;\
		new_data_array.add_data(average);\
		if (i >= end_index)\
			break;\
		sum -= buffer[buffer_pos];\
		buffer[buffer_pos] = array_data[i];\
		sum += buffer[buffer_pos];\
		buffer_pos ++;\
		if (buffer_pos == N)\
			buffer_pos = 0;\
		i++;\
	}while (true);\
	if (buffer != NULL)\
	{\
		delete[] buffer;\
		buffer = NULL;\
 	}\
	return RET_SUCCESS;\
}\
unsigned short Finance##m##DataArray::get_weighted_avg_array(FinanceFloatDataArray& new_data_array, int N, int start_index, int end_index)\
{\
	static const int WEIGHTED_AVG_TYPE[] = {5, 10, 20, 60};\
	static const int WEIGHTED_AVG_TYPE_SIZE = sizeof(WEIGHTED_AVG_TYPE)/sizeof(WEIGHTED_AVG_TYPE[0]);\
	static const int N_5_WEIGHTED_PARAM[] = {1, 1, 2, 2, 3};\
	static const int N_10_WEIGHTED_PARAM[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3};\
	static const int N_20_WEIGHTED_PARAM[] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4};\
	static const int N_60_WEIGHTED_PARAM[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4};\
	static const int N_5_WEIGHTED_PARAM_SUM = get_array_sum(N_5_WEIGHTED_PARAM, WEIGHTED_AVG_TYPE[0]);\
	static const int N_10_WEIGHTED_PARAM_SUM =get_array_sum(N_10_WEIGHTED_PARAM, WEIGHTED_AVG_TYPE[1]);\
	static const int N_20_WEIGHTED_PARAM_SUM = get_array_sum(N_20_WEIGHTED_PARAM, WEIGHTED_AVG_TYPE[2]);\
	static const int N_60_WEIGHTED_PARAM_SUM = get_array_sum(N_60_WEIGHTED_PARAM, WEIGHTED_AVG_TYPE[3]);\
	static const int* WEIGHTED_PARAM[] = {N_5_WEIGHTED_PARAM, N_10_WEIGHTED_PARAM, N_20_WEIGHTED_PARAM, N_60_WEIGHTED_PARAM};\
	static const int WEIGHTED_PARAM_SUM[] = {N_5_WEIGHTED_PARAM_SUM, N_10_WEIGHTED_PARAM_SUM, N_20_WEIGHTED_PARAM_SUM, N_60_WEIGHTED_PARAM_SUM};\
	int weighted_index = get_array_index(N, WEIGHTED_AVG_TYPE, WEIGHTED_AVG_TYPE_SIZE);\
	if (weighted_index == -1)\
	{\
		WRITE_FORMAT_ERROR("Unsupported argument for weighted average: %d", N);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	end_index = get_end_index_ex(end_index, array_pos);\
	INDEX_IN_RANGE(start_index, end_index, 0, array_pos);\
	if ((end_index - start_index) < N)\
	{\
		WRITE_FORMAT_ERROR("The array size is NOT more than %d", N);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	n *buffer = new n[N];\
	if (buffer == NULL)\
	{\
		WRITE_ERROR("Fail to allocate memory: buffer");\
		return RET_FAILURE_INSUFFICIENT_MEMORY;\
	}\
	float weighted_average;\
	n sum;\
	for (int i = start_index ; i < end_index - N + 1 ; i++)\
	{\
		sum = (n)0;\
		for (int j = 0 ; j < N ; j++)\
		{\
			sum += WEIGHTED_PARAM[weighted_index][j] * array_data[i + j];\
		}\
		weighted_average = (float)sum / WEIGHTED_PARAM_SUM[weighted_index];\
		new_data_array.add_data(weighted_average);\
	}\
	if (buffer != NULL)\
	{\
		delete[] buffer;\
		buffer = NULL;\
 	}\
	return RET_SUCCESS;\
}

#define IMPLEMENT_DATA_ARRAY_OSTREAM(m, n)\
ostream& operator<< (ostream &out, const Finance##m##DataArray &finance_##n##_data_array)\
{\
	int finance_##n##_data_array_len = finance_##n##_data_array.get_size();\
	for (int i = 0 ; i < finance_##n##_data_array_len ; i++)\
		out << finance_##n##_data_array[i] << " ";\
	return out;\
}

IMPLEMENT_DATA_ARRAY_OPERATOR(Int, int)
IMPLEMENT_DATA_ARRAY_INTEGER_EQUAL_OPERATOR(Int, int)
IMPLEMENT_DATA_ARRAY_OPERATOR(Long, long)
IMPLEMENT_DATA_ARRAY_INTEGER_EQUAL_OPERATOR(Long, long)
IMPLEMENT_DATA_ARRAY_OPERATOR(Float, float)
IMPLEMENT_DATA_ARRAY_FLOAT_EQUAL_OPERATOR(Float, float)

IMPLEMENT_DATA_ARRAY_OPERATOR_EQUAL(Bool, bool)
IMPLEMENT_DATA_ARRAY_INTEGER_EQUAL_OPERATOR(Bool, bool)

IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(Int, int)
IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(Long, long)
IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(Float, float)

FinanceBoolDataArray::FinanceBoolDataArray() :
	true_cnt(0)
{
}

void FinanceBoolDataArray::reset_array()
{
	true_cnt = 0;
	FinanceDataArrayBase::reset_array();
}

void FinanceBoolDataArray::add_data(bool data)
{
	FinanceDataArrayTemplate<bool>::add_data(data);
	if (data)
		true_cnt++;
}

void FinanceBoolDataArray::add_data_array(bool* data, int size)
{
	FinanceDataArrayTemplate<bool>::add_data_array(data, size);
	for (int i = 0 ; i < size ; i++)
		if (data[i])
			true_cnt++;
}

int FinanceBoolDataArray::get_true_cnt()const{return true_cnt;}

float FinanceBoolDataArray::get_probability()const
{
	return (float)true_cnt / array_pos;
}

IMPLEMENT_DATA_ARRAY_OSTREAM(Int, int)
IMPLEMENT_DATA_ARRAY_OSTREAM(Long, long)
IMPLEMENT_DATA_ARRAY_OSTREAM(Float, float)
IMPLEMENT_DATA_ARRAY_OSTREAM(Bool, bool)

ostream& operator<<(ostream &out, const FinanceDataArrayBase &finance_data_array)
{
	switch (finance_data_array.get_type())
	{
	case FinanceField_INT:
		return operator<<(out, *(PFINANCE_INT_DATA_ARRAY)&finance_data_array);
	case FinanceField_LONG:
		return operator<<(out, *(PFINANCE_LONG_DATA_ARRAY)&finance_data_array);
	case FinanceField_FLOAT:
		return operator<<(out, *(PFINANCE_FLOAT_DATA_ARRAY)&finance_data_array);
	default:
		break;
	}
	char errmsg[64];
	snprintf(errmsg, 64, "Unknown type value: %d", finance_data_array.get_type());
	throw invalid_argument(errmsg);
	return out;
}

#define IMPLEMENT_DATA_PTR_ARRAY_OSTREAM(m, n)\
ostream& operator<< (ostream &out, const Finance##m##DataPtrArray &finance_##n##_data_array)\
{\
	int finance_##n##_data_array_len = finance_##n##_data_array.get_size();\
	for (int i = 0 ; i < finance_##n##_data_array_len ; i++)\
		out << finance_##n##_data_array[i] << " ";\
	return out;\
}

IMPLEMENT_DATA_PTR_ARRAY_OSTREAM(Char, char)
