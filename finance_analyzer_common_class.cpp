#include <assert.h>
#include <math.h>
#include <string>
#include <stdexcept>
#include <new>
#include <algorithm>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

template <typename T>
SmartPointer<T>::SmartPointer() :
	need_release(true)
{
	data_ptr = NULL;
}

template <typename T>
SmartPointer<T>::SmartPointer(T* ptr) :
	need_release(true)
{
	data_ptr = ptr;
}

template <typename T>
SmartPointer<T>::~SmartPointer()
{
	if (need_release && data_ptr != NULL)
	{
		delete data_ptr;
		data_ptr = NULL;
	}
}

template <typename T>
T& SmartPointer<T>::operator*()
{
	return *data_ptr;
}

template <typename T>
const T& SmartPointer<T>::operator*() const
{
	return *data_ptr;
}

template <typename T>
T* SmartPointer<T>::operator->()
{
	return data_ptr;
}

template <typename T>
const T* SmartPointer<T>::operator->() const
{
	return data_ptr;
}

template <typename T>
void SmartPointer<T>::set_new(T* ptr)
{
	if (data_ptr != NULL)
	{
		delete data_ptr;
		data_ptr = NULL;
	}
	data_ptr = ptr;
}

template <typename T>
T* SmartPointer<T>::get_instance()
{
	return data_ptr;
}

template <typename T>
void SmartPointer<T>::disable_release(){need_release = false;}

template class SmartPointer<TimeCfg>;
template class SmartPointer<TimeRangeCfg>;
template class SmartPointer<QuerySet>;
template class SmartPointer<ResultSet>;
template class SmartPointer<ResultSetAccessParam>;
template class SmartPointer<FinanceIntDataArray>;
template class SmartPointer<FinanceLongDataArray>;
template class SmartPointer<FinanceFloatDataArray>;

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

TimeRangeCfg::TimeRangeCfg(const char* time_start_str, const char* time_end_str) :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false)
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
	type_is_month(false)
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
	type_is_month(false)
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
	type_is_month(false)
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

bool TimeRangeCfg::is_single_time()const
{
	if (time_start_cfg != NULL && time_end_cfg != NULL)
		return time_start_cfg->equal_to(time_end_cfg);
	return false;
}

const char* TimeRangeCfg::to_string()
{
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
//	assert(time_start_cfg != NULL && "time_start_cfg should NOT be NULL");
	return time_start_cfg;
}

const PTIME_CFG TimeRangeCfg::get_end_time()const
{
//	assert(time_end_cfg != NULL && "time_end_cfg should NOT be NULL");
	return time_end_cfg;
}

PTIME_CFG TimeRangeCfg::get_start_time()
{
//	assert(time_start_cfg != NULL && "time_start_cfg should NOT be NULL");
	return time_start_cfg;
}

PTIME_CFG TimeRangeCfg::get_end_time()
{
//	assert(time_end_cfg != NULL && "time_end_cfg should NOT be NULL");
	return time_end_cfg;
}

void TimeRangeCfg::reset_time(const char* new_time_start_str, const char* new_time_end_str)
{
	if (new_time_start_str != NULL)
		time_start_cfg->reset(new_time_start_str);
	if (new_time_end_str != NULL)
		time_start_cfg->reset(new_time_end_str);
}

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
void FinanceDataArrayTemplate<T>::alloc_new()
{
	T* array_data_old = array_data;
	array_size <<= 1;
	array_data = (T*)realloc(array_data_old, sizeof(T) * array_size);
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
const T* FinanceDataArrayTemplate<T>::get_data_array()const{return array_data;}

template <typename T>
void FinanceDataArrayTemplate<T>::add(T data)
{
	if (array_pos + 1 >= array_size)
		alloc_new();

	array_data[array_pos++] = data;
}

template <typename T>
const T FinanceDataArrayTemplate<T>::operator[](int index)const
{
	assert(array_data != NULL && "array_data == NULL");
	if (index >= 0)
	{
		if (index >= array_pos)
		{
			char errmsg[64];
			snprintf(errmsg, 64, "positive index[%d] is out of range: [0, %d)", index, array_pos);
			WRITE_ERROR(errmsg);
			throw out_of_range(errmsg);
		}
	}
	else if (index < 0)
	{
		if (abs(index) > abs(array_pos))
		{
			char errmsg[64];
			snprintf(errmsg, 64, "negative index[%d] is out of range: (0, %d]", index, array_pos);
			WRITE_ERROR(errmsg);
			throw out_of_range(errmsg);
		}
		index = get_end_index_ex(index, array_pos);
	}

	return array_data[index];
}

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
FinanceDataPtrArrayTemplate<T>::~FinanceDataPtrArrayTemplate()
{
	if (FinanceDataArrayTemplate<T>::array_data != NULL)
	{
		for (int i = 0 ; i < FinanceDataArrayTemplate<T>::array_pos ; i++)
		{
			free(FinanceDataArrayTemplate<T>::array_data[i]);
			FinanceDataArrayTemplate<T>::array_data[i] = NULL;
		}
	}
}

template <typename T>
void FinanceDataPtrArrayTemplate<T>::add(T data, size_t data_size)
{
	if (FinanceDataArrayTemplate<T>::array_pos + 1 >= FinanceDataArrayTemplate<T>::array_size)
		FinanceDataArrayTemplate<T>::alloc_new();

	T data_new = (T)calloc(data_size, sizeof(char));
	if (data_new == NULL)
		throw bad_alloc();
	memcpy((void*)data_new, (void*)data, sizeof(char) * data_size);
	FinanceDataArrayTemplate<T>::array_data[FinanceDataArrayTemplate<T>::array_pos++] = data_new;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		if ((array_data[i] - another_array[i]) > TOLERANCE)\
			return false;\
	}\
	return true;\
}

#define IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(m, n)\
unsigned short Finance##m##DataArray::get_sub_array(Finance##m##DataArray& new_data_array, int start_index, int end_index)\
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
	for (int i = start_index ; i < end_index ; i++)\
		new_data_array.add(array_data[i]);\
	return RET_SUCCESS;\
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
		new_data_array.add(array_data[i] - array_data[i - 1]);\
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
		new_data_array.add(sum);\
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
		new_data_array.add(average);\
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
		new_data_array.add(weighted_average);\
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

IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(Int, int)
IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(Long, long)
IMPLEMENT_DATA_ARRAY_ELEMENT_CALCULATION(Float, float)

FinanceBoolDataArray::FinanceBoolDataArray() :
	true_cnt(0)
{
}

void FinanceBoolDataArray::add(bool data)
{
	FinanceDataArrayTemplate<bool>::add(data);
	if (data)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QuerySet::QuerySet() :
	add_done(false)
{
	IMPLEMENT_MSG_DUMPER()
}

QuerySet::~QuerySet()
{
	RELEASE_MSG_DUMPER()
}

unsigned short QuerySet::add_query(int source_index, int field_index)
{
	if (add_done)
	{
		WRITE_ERROR("Fail to add another data");
		return RET_FAILURE_INCORRECT_OPERATION;
	}

// Check if the index is out of range
	if(source_index < 0 && source_index >= FinanceSourceSize)
	{
		WRITE_ERROR("source_index is out of range in QuerySet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}

	if(field_index < 0 && field_index >= FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index])
	{
// If field_index == -1, it means select all field in the table
		if (field_index != -1)
		{
			WRITE_ERROR("field_index is out of range in QuerySet");
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}
// Check the index is duplicate
	DEQUE_INT::iterator it = find(query_set[source_index].begin(), query_set[source_index].end(), 10);
	if (it != query_set[source_index].end())
	{
		WRITE_FORMAT_WARN("Duplicate index: %d in %s", field_index, FINANCE_DATABASE_DESCRIPTION_LIST[source_index]);
		return RET_WARN_INDEX_DUPLICATE;
	}
// If all fields are selected, it's no need to add extra index
	if (!query_set[source_index].empty() && query_set[source_index][0] == -1)
	{
		WRITE_FORMAT_WARN("Ignore index: %d in %s", field_index, FINANCE_DATABASE_DESCRIPTION_LIST[source_index]);
		return RET_WARN_INDEX_IGNORE;
	}

// Add the index
	if (field_index == -1)
		query_set[source_index].clear();
	query_set[source_index].push_back(field_index);
	return RET_SUCCESS;
}

unsigned short QuerySet::add_query_done()
{
	if (add_done)
	{
		WRITE_ERROR("Fail to add another data");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	for (int i = 0 ; i < FinanceSourceSize ; i++)
	{
		if (query_set[i].empty())
			continue;
//		WRITE_FORMAT_DEBUG("Transform the query data[source_index: %d]", i);
		if (query_set[i][0] == -1)
		{
			query_set[i].clear();
			for (int field_index = 1 ; field_index < FINANCE_DATABASE_FIELD_AMOUNT_LIST[i] ; field_index++) // Caution: Don't include the "date" field
				query_set[i].push_back(field_index);
		}
	}
	add_done = true;
	return RET_SUCCESS;
}

bool QuerySet::is_add_query_done()const{ return add_done; }

const DEQUE_INT& QuerySet::operator[](int index)const
{
	if (index < 0 || index >= FinanceSourceSize)
	{
		char errmsg[64];
		snprintf(errmsg, 64, "The index[%d] is out of range(0, %d)", index , FinanceSourceSize - 1);
		WRITE_ERROR(errmsg);
		throw out_of_range(errmsg);
	}
	return query_set[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResultSetAccessParam::ResultSetAccessParam(FinanceSourceType new_finance_source_type, int new_finance_field_no, ArrayElementCalculationType new_calculation_type, int new_start_index, int new_end_index)
{
	finance_source_type = new_finance_source_type;
	finance_field_no = new_finance_field_no;
	calculation_type = new_calculation_type;
	start_index = new_start_index;
	end_index = new_end_index;
}

void ResultSetAccessParam::set_finance_source_type(FinanceSourceType new_finance_source_type){finance_source_type = new_finance_source_type;}
FinanceSourceType ResultSetAccessParam::get_finance_source_type()const{return finance_source_type;}

void ResultSetAccessParam::set_finance_field_no(int new_finance_field_no){finance_field_no = new_finance_field_no;}
int ResultSetAccessParam::get_finance_field_no()const{return finance_field_no;}

void ResultSetAccessParam::set_calculation_type(ArrayElementCalculationType new_calculation_type){calculation_type = new_calculation_type;}
ArrayElementCalculationType ResultSetAccessParam::get_calculation_type()const{return calculation_type;}

void ResultSetAccessParam::set_start_index(int new_start_index){start_index = new_start_index;}
int ResultSetAccessParam::get_start_index()const{return start_index;}

void ResultSetAccessParam::set_end_index(int new_end_index){end_index = new_end_index;}
int ResultSetAccessParam::get_end_index()const{return end_index;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned short ResultSet::get_combined_index(int x, int y)
{
	return (unsigned short)(((x & 0xFF) << 8) | (y & 0xFF));
}

unsigned long ResultSet::get_combined_index_ex(int x, int y, ArrayElementCalculationType calculation_type)
{
	if (calculation_type == ArrayElementCalculation_None)
		return get_combined_index(x, y);
	return (unsigned long)(((calculation_type & 0xFFFF) << 16) | (get_combined_index(x, y) & 0xFFFF));
}

unsigned short ResultSet::get_upper_subindex(unsigned short x)
{
	return (unsigned short)((x >> 8) & 0xFF);
}

unsigned short ResultSet::get_lower_subindex(unsigned short x)
{
	return (unsigned short)(x & 0xFF);
}

unsigned short ResultSet::get_calculation_subindex(unsigned long x)
{
	return (unsigned short)((x >> 16) & 0xFF);
}

ResultSet::ResultSet() :
	check_date_data_mode(false),
	data_set_mapping_size(0),
	date_data_size(0),
	date_data_cur_pos(0),
	int_data_set_size(0),
	long_data_set_size(0),
	float_data_set_size(0)
{
	IMPLEMENT_MSG_DUMPER()
}

ResultSet::~ResultSet()
{
	int data_size = 0;
	if (!int_data_set.empty())
	{
		data_size = int_data_set.size();
		for(int i = 0 ; i < data_size ; i++)
		{
			delete int_data_set[i];
			int_data_set[i] = NULL;
		}
		int_data_set.clear();
	}
	if (!long_data_set.empty())
	{
		data_size = long_data_set.size();
		for(int i = 0 ; i < data_size ; i++)
		{
			delete long_data_set[i];
			long_data_set[i] = NULL;
		}
		long_data_set.clear();
	}
	if (!float_data_set.empty())
	{
		data_size = float_data_set.size();
		for(int i = 0 ; i < data_size ; i++)
		{
			delete float_data_set[i];
			float_data_set[i] = NULL;
		}
		float_data_set.clear();
	}

	RELEASE_MSG_DUMPER()
}

unsigned short ResultSet::add_set(int source_index, int field_index)
{
	if(source_index < 0 && source_index >= FinanceSourceSize)
	{
		WRITE_ERROR("source_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(field_index < 0 && field_index >= FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index])
	{
// If field_index == -1, it means select all field in the table
//		if (field_index != -1)
//		{
			WRITE_ERROR("field_index is out of range in ResultSet");
			return RET_FAILURE_INVALID_ARGUMENT;
//		}
	}

	deque<int> field_list;
//	if (field_index != -1)
		field_list.push_back(field_index);
//	else
//	{
//		for(int i = 1 ; i < FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index] ; i++)
//			field_list.push_back(i);
//	}
	deque<int>::iterator iter = field_list.begin();
	while(iter != field_list.end())
	{
		field_index = *iter;
		unsigned short key = get_combined_index(source_index, field_index);
		if (data_set_mapping.find(key) != data_set_mapping.end())
		{
			WRITE_FORMAT_ERROR("The key[%d] from (%d, %d) is duplicate", key, source_index, field_index);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		unsigned short value;
//		int next_index = -1;
		switch(FINANCE_DATABASE_FIELD_TYPE_LIST[source_index][field_index])
		{
		case FinanceField_INT:
		{
//			PFINANCE_INT_DATA_ARRAY new_finance_int_data = new FinanceIntDataArray();
//			if (new_finance_int_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_int_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_INT, int_data_set.size());
			PFINANCE_INT_DATA_ARRAY new_array = new FinanceIntDataArray();
			assert(new_array != NULL && "Fail to allocate the FinanceIntDataArray object");
			new_array->set_type(FinanceField_INT);
			int_data_set.push_back(new_array);
			int_data_set_size = int_data_set.size();
		}
			break;
		case FinanceField_LONG:
		{
//			PFINANCE_LONG_DATA_ARRAY new_finance_long_data = new FinanceLongDataArray();
//			if (new_finance_long_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_long_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_LONG, long_data_set.size());
			PFINANCE_LONG_DATA_ARRAY new_array = new FinanceLongDataArray();
			assert(new_array != NULL && "Fail to allocate the FinanceLongDataArray object");
			new_array->set_type(FinanceField_LONG);
			long_data_set.push_back(new_array);
			long_data_set_size = long_data_set.size();
		}
			break;
		case FinanceField_FLOAT:
		{
//			PFINANCE_FLOAT_DATA_ARRAY new_finance_float_data = new FinanceFloatDataArray();
//			if (new_finance_float_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_float_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_FLOAT, float_data_set.size());
			PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
			assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
			new_array->set_type(FinanceField_FLOAT);
			float_data_set.push_back(new_array);
			float_data_set_size = float_data_set.size();
		}
			break;
		case FinanceField_DATE:
			WRITE_ERROR("The DATE field type is NOT supported");
			return RET_FAILURE_INVALID_ARGUMENT;
		default:
			WRITE_FORMAT_ERROR("The unsupported field type: %d", FINANCE_DATABASE_FIELD_TYPE_LIST[source_index][field_index]);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		data_set_mapping[key] = value;
		data_set_mapping_size++;
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::add_set(int source_index, const DEQUE_INT& field_set)
{
	unsigned short ret = RET_SUCCESS;
	int field_set_size = field_set.size();
	for (int index = 0 ; index < field_set_size ; index++)
	{
		ret = add_set(source_index, field_set[index]);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return ret;
}

unsigned short ResultSet::add_calculation_set_dummy(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex)
{
	assert("No need to do the element calculation");
	return RET_SUCCESS;
}

unsigned short ResultSet::add_calculation_set_diff(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	unsigned short value;
	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY new_array = new FinanceIntDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceIntDataArray object");
		new_array->set_type(FinanceField_INT);

		value = get_combined_index(FinanceField_INT, int_data_set.size());

		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_diff_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		int_data_set.push_back(new_array);
		int_data_set_size = int_data_set.size();
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY new_array = new FinanceLongDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceLongDataArray object");
		new_array->set_type(FinanceField_LONG);

		value = get_combined_index(FinanceField_LONG, long_data_set.size());

		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_diff_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		long_data_set.push_back(new_array);
		long_data_set_size = long_data_set.size();
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
		new_array->set_type(FinanceField_FLOAT);

		value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_diff_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		float_data_set.push_back(new_array);
		float_data_set_size = float_data_set.size();
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_sum(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	unsigned short value;

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY new_array = new FinanceIntDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceIntDataArray object");
		new_array->set_type(FinanceField_INT);

		value = get_combined_index(FinanceField_INT, int_data_set.size());

		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_sum_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		int_data_set.push_back(new_array);
		int_data_set_size = int_data_set.size();
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY new_array = new FinanceLongDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceLongDataArray object");
		new_array->set_type(FinanceField_LONG);

		value = get_combined_index(FinanceField_LONG, long_data_set.size());

		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_sum_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		long_data_set.push_back(new_array);
		long_data_set_size = long_data_set.size();
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
		new_array->set_type(FinanceField_FLOAT);

		value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_sum_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		float_data_set.push_back(new_array);
		float_data_set_size = float_data_set.size();
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_sum5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 5);}
unsigned short ResultSet::add_calculation_set_sum10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 10);}
unsigned short ResultSet::add_calculation_set_sum20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 20);}
unsigned short ResultSet::add_calculation_set_sum60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 60);}

unsigned short ResultSet::add_calculation_set_avg(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
	new_array->set_type(FinanceField_FLOAT);

	unsigned short value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	float_data_set.push_back(new_array);
	float_data_set_size = float_data_set.size();

	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_avg5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 5);}
unsigned short ResultSet::add_calculation_set_avg10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 10);}
unsigned short ResultSet::add_calculation_set_avg20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 20);}
unsigned short ResultSet::add_calculation_set_avg60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 60);}

unsigned short ResultSet::add_calculation_set_weighted_avg(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
	new_array->set_type(FinanceField_FLOAT);

	unsigned short value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_weighted_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_weighted_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_weighted_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	float_data_set.push_back(new_array);
	float_data_set_size = float_data_set.size();

	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_weighted_avg5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 5);}
unsigned short ResultSet::add_calculation_set_weighted_avg10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 10);}
unsigned short ResultSet::add_calculation_set_weighted_avg20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 20);}
unsigned short ResultSet::add_calculation_set_weighted_avg60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 60);}

unsigned short ResultSet::add_calculation_set(int source_index, int field_index, ArrayElementCalculationType calculation_type)
{
	typedef unsigned short (ResultSet::*add_calculation_set_func_ptr)(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	static add_calculation_set_func_ptr add_calculation_set_func_array[] =
	{
		&ResultSet::add_calculation_set_dummy,
		&ResultSet::add_calculation_set_diff,
		&ResultSet::add_calculation_set_sum5,
		&ResultSet::add_calculation_set_sum10,
		&ResultSet::add_calculation_set_sum20,
		&ResultSet::add_calculation_set_sum60,
		&ResultSet::add_calculation_set_avg5,
		&ResultSet::add_calculation_set_avg10,
		&ResultSet::add_calculation_set_avg20,
		&ResultSet::add_calculation_set_avg60,
		&ResultSet::add_calculation_set_weighted_avg5,
		&ResultSet::add_calculation_set_weighted_avg10,
		&ResultSet::add_calculation_set_weighted_avg20,
		&ResultSet::add_calculation_set_weighted_avg60
	};

	if (calculation_type == ArrayElementCalculation_None)
	{
		WRITE_ERROR("Incorrect calculation type: ArrayElementCalculation_None");
		return RET_FAILURE_INVALID_ARGUMENT;
//		return add_set(source_index, field_index);
	}
// Check the index boundary
	if(source_index < 0 && source_index >= FinanceSourceSize)
	{
		WRITE_ERROR("source_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(field_index < 0 && field_index >= FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index])
	{
		WRITE_ERROR("field_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(calculation_type < 0 && calculation_type >= ArrayElementCalculationSize)
	{
		WRITE_ERROR("calculation_type is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
// Make sure the array should NOT exist
	unsigned long key_ex = get_combined_index_ex(source_index, field_index, calculation_type);
	if (data_calculation_set_mapping.find(key_ex) != data_calculation_set_mapping.end())
	{
		WRITE_FORMAT_ERROR("The key[%ld] from (%d, %d, %d) is duplicate", key_ex, source_index, field_index, calculation_type);
		return RET_FAILURE_INVALID_ARGUMENT;
	}

// Find the original array
	PFINANCE_DATA_ARRAY_BASE data_array_base = get_array(source_index, field_index);
	if (data_array_base == NULL)
	{
		WRITE_FORMAT_ERROR("The array from (%d, %d) should EXIST", source_index, field_index);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	return (this->*(add_calculation_set_func_array[calculation_type]))(data_array_base, key_ex);
}

unsigned short ResultSet::set_date(char* element_value)
{
	if (check_date_data_mode)
	{
		if (strcmp(date_data[date_data_cur_pos], element_value) != 0)
		{
			WRITE_FORMAT_ERROR("The date(%s, %s) is NOT equal", date_data[date_data_cur_pos], element_value);
			return RET_FAILURE_INCORRECT_OPERATION;
		}
		date_data_cur_pos++;
	}
	else
	{
		date_data.add(element_value, strlen(element_value) + 1);
		date_data_size++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::find_data_pos(int source_index, int field_index, unsigned short& field_type_index, unsigned short& field_type_pos)const
{
	unsigned short key = get_combined_index(source_index, field_index);
//	if (data_set_mapping.find(key) == data_set_mapping.end())
//	{
//		WRITE_FORMAT_ERROR("The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	unsigned short value = data_set_mapping[key];
	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.find(key);
	if (iter == data_set_mapping.end())
	{
		char errmsg[64];
		snprintf(errmsg, 64, "The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);
		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short value = iter->second;

	field_type_index = get_upper_subindex(value);
	field_type_pos = get_lower_subindex(value);

	return RET_SUCCESS;
}

unsigned short ResultSet::set_data(int source_index, int field_index, char* data_string)
{
	if (data_string == NULL)
	{
		WRITE_ERROR("data_string should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short ret = RET_SUCCESS;
	unsigned short field_type_index, field_type_pos;
	ret = find_data_pos(source_index, field_index, field_type_index, field_type_pos);
	if (CHECK_FAILURE(ret))
		return ret;

	switch(field_type_index)
	{
	case FinanceField_INT:
		int_data_set[field_type_pos]->add(atoi(data_string));
		break;
	case FinanceField_LONG:
		long_data_set[field_type_pos]->add(atol(data_string));
		break;
	case FinanceField_FLOAT:
		float_data_set[field_type_pos]->add(atof(data_string));
		break;
	default:
		WRITE_FORMAT_ERROR("Unsupported field_type_index: %d", field_type_index);
		ret = RET_FAILURE_INVALID_ARGUMENT;
		break;
	}
	return RET_SUCCESS;
}

#define DEFINE_GET_ARRAY_FUNC_BY_POS(n, m)\
const PFINANCE_##m##_DATA_ARRAY ResultSet::get_##n##_array_by_position(unsigned short field_type_index, unsigned short field_type_pos)const\
{\
	static const int BUF_SIZE = 64;\
	static char errmsg[BUF_SIZE];\
	if (field_type_index != FinanceField_##m)\
	{\
		snprintf(errmsg, BUF_SIZE, "The field type[%d] is NOT int", field_type_index);\
		WRITE_ERROR(errmsg);\
		throw invalid_argument(errmsg);\
	}\
	if (field_type_pos >= n##_data_set_size || field_type_pos < 0)\
	{\
		snprintf(errmsg, BUF_SIZE, "The field pos[%d] is out of range", field_type_pos);\
		WRITE_ERROR(errmsg);\
		throw out_of_range(errmsg);\
	}\
	return n##_data_set[field_type_pos];\
}

DEFINE_GET_ARRAY_FUNC_BY_POS(int, INT)
DEFINE_GET_ARRAY_FUNC_BY_POS(long, LONG)
DEFINE_GET_ARRAY_FUNC_BY_POS(float, FLOAT)

#define DEFINE_GET_ARRAY_FUNC(n, m)\
const PFINANCE_##m##_DATA_ARRAY ResultSet::get_##n##_array(int source_index, int field_index)const\
{\
	static const int BUF_SIZE = 64;\
	static char errmsg[BUF_SIZE];\
	unsigned short ret = RET_SUCCESS;\
	unsigned short field_type_index, field_type_pos;\
	ret = find_data_pos(source_index, field_index, field_type_index, field_type_pos);\
	if (CHECK_FAILURE(ret))\
	{\
		snprintf(errmsg, BUF_SIZE, "Fail to fail data position from (%d, %d)", source_index, field_index);\
		WRITE_ERROR(errmsg);\
		throw invalid_argument(errmsg);\
	}\
	return get_##n##_array_by_position(field_type_index, field_type_pos);\
}

DEFINE_GET_ARRAY_FUNC(int, INT)
DEFINE_GET_ARRAY_FUNC(long, LONG)
DEFINE_GET_ARRAY_FUNC(float, FLOAT)

const PFINANCE_DATA_ARRAY_BASE ResultSet::get_array(int source_index, int field_index)const
{
	int field_type = FINANCE_DATABASE_FIELD_TYPE_LIST[source_index][field_index];
	switch (field_type)
	{
	case FinanceField_INT:
		return get_int_array(source_index, field_index);
	case FinanceField_LONG:
		return get_long_array(source_index, field_index);
	case FinanceField_FLOAT:
		return get_float_array(source_index, field_index);
	default:
	{
		char errmsg[32];
		snprintf(errmsg, 32, "Unknown field type: %d", field_type);
		WRITE_ERROR(errmsg);
		throw invalid_argument(errmsg);
	}
	break;
	}
	return NULL	;
}

const PFINANCE_DATA_ARRAY_BASE ResultSet::get_array(int source_index, int field_index, ArrayElementCalculationType calculation_type)
{
	static const int BUF_SIZE = 32;
	static char errmsg[BUF_SIZE];

	if (calculation_type == ArrayElementCalculation_None)
		return get_array(source_index, field_index);

	unsigned short ret = RET_SUCCESS;
	unsigned long key_ex = get_combined_index_ex(source_index, field_index, calculation_type);
// If the array should NOT exist, create the array
	if (data_calculation_set_mapping.find(key_ex) == data_calculation_set_mapping.end())
	{
		ret = add_calculation_set(source_index, field_index, calculation_type);
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, BUF_SIZE, "Fail to set array from (%d, %d, %d), due to: %s", source_index, field_index, calculation_type, get_ret_description(ret));
			WRITE_ERROR(errmsg);
			throw runtime_error(string(errmsg));
		}
	}
// Access the calculation array
	unsigned short value = data_calculation_set_mapping[key_ex];
	unsigned short field_type_index = get_upper_subindex(value);
	unsigned short field_type_pos = get_lower_subindex(value);
	switch (field_type_index)
	{
	case FinanceField_INT:
		return int_data_set[field_type_pos];
	case FinanceField_LONG:
		return long_data_set[field_type_pos];
	case FinanceField_FLOAT:
		return float_data_set[field_type_pos];
	default:
	{
		snprintf(errmsg, BUF_SIZE, "Unknown field type: %d", field_index);
		WRITE_ERROR(errmsg);
		throw invalid_argument(errmsg);
	}
	break;
	}
	return NULL	;
}

const PFINANCE_CHAR_DATA_PTR_ARRAY ResultSet::get_date_array()const
{
	return (const PFINANCE_CHAR_DATA_PTR_ARRAY)&date_data;
}

#define DEFINE_GET_ARRAY_ELEMENT_FUNC(n, m)\
n ResultSet::get_##n##_array_element(int source_index, int field_index, int index)const\
{\
	PFINANCE_##m##_DATA_ARRAY data_array = get_##n##_array(source_index, field_index);\
	return (*data_array)[index];\
}
DEFINE_GET_ARRAY_ELEMENT_FUNC(int, INT)
DEFINE_GET_ARRAY_ELEMENT_FUNC(long, LONG)
DEFINE_GET_ARRAY_ELEMENT_FUNC(float, FLOAT)

const char* ResultSet::get_date_array_element(int index)const
{
	return date_data[index];
}

void ResultSet::switch_to_check_date_mode()
{
	check_date_data_mode = true;
	date_data_cur_pos = 0;
}

unsigned short ResultSet::check_data()const
{
	assert(date_data_size == date_data.get_size() && "The date_data_size is NOT identical");
//	int date_data_size_tmp = date_data.get_size();
	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.begin();

	unsigned short key;
	unsigned short value;
	unsigned short field_type_index;
	unsigned short field_type_pos;
	int data_size;
	while (iter != data_set_mapping.end())
	{
		value = iter->second;
		field_type_index = get_upper_subindex(value);
		field_type_pos = get_lower_subindex(value);
		switch(field_type_index)
		{
		case FinanceField_INT:
			data_size = int_data_set[field_type_pos]->get_size();
			break;
		case FinanceField_LONG:
			data_size = long_data_set[field_type_pos]->get_size();
			break;
		case FinanceField_FLOAT:
			data_size = float_data_set[field_type_pos]->get_size();
			break;
		default:
			WRITE_FORMAT_ERROR("Unsupported field_type_index: %d", field_type_index);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
// Check all the data size are equal
		if (date_data_size != data_size)
		{
			key = iter->first;
			unsigned short source_index = get_upper_subindex(key);
			unsigned short field_index = get_lower_subindex(key);
			WRITE_FORMAT_ERROR("Incorrect data size in %d, %d, expected: %d, actual: %d", source_index, field_index, date_data_size, data_size);
			return RET_FAILURE_INCORRECT_OPERATION;
		}
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::show_data()const
{
	static int STAR_LEN = 120;
	if (!SHOW_CONSOLE)
	{
		WRITE_WARN("Disabled; No data will be shown on STDOUT/STDERR");
		return RET_SUCCESS;
	}
	unsigned short key;
	unsigned short value;
	unsigned short source_index;
	unsigned short field_index;
	unsigned short field_type_index;
	unsigned short field_type_pos;

	assert(date_data_size == date_data.get_size() && "The date_data_size is NOT identical");

// Show the database:field info
	for(int i = 0 ; i < STAR_LEN ; i++)
		putchar('*');
	printf("\n| %s |", MYSQL_DATE_FILED_NAME);

	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.begin();
	while (iter != data_set_mapping.end())
	{
		key = iter->first;
		source_index = get_upper_subindex(key);
		field_index = get_lower_subindex(key);
		printf(" %s:%s%d |", FINANCE_DATABASE_DESCRIPTION_LIST[source_index], MYSQL_FILED_NAME_BASE, field_index);
		iter++;
	}
	printf("\n");
	for(int i = 0 ; i < STAR_LEN ; i++)
		putchar('*');
	printf("\n\n");

// Show the data info
//	int date_data_size_tmp = date_data.get_size();
	for(int i = 0 ; i < date_data_size ; i++)
	{
		printf("| %s |", date_data[i]);
		iter = data_set_mapping.begin();
		while (iter != data_set_mapping.end())
		{
			value = iter->second;
			field_type_index = get_upper_subindex(value);
			field_type_pos = get_lower_subindex(value);
			switch(field_type_index)
			{
			case FinanceField_INT:
				printf(" %10d |", (*int_data_set[field_type_pos])[i]);
				break;
			case FinanceField_LONG:
				printf(" %12ld |", (*long_data_set[field_type_pos])[i]);
				break;
			case FinanceField_FLOAT:
				printf(" %8.1f |", (*float_data_set[field_type_pos])[i]);
				break;
			default:
				WRITE_FORMAT_ERROR("Unsupported field_type_index: %d", field_type_index);
				return RET_FAILURE_INVALID_ARGUMENT;
			}
			iter++;
		}
		printf("\n");
	}
	return RET_SUCCESS;
}

int ResultSet::get_data_dimension()const{return data_set_mapping_size;}
int ResultSet::get_data_size()const{return date_data_size;}
