#include <assert.h>
// #include <math.h>
// #include <string>
#include <stdexcept>
// #include <new>
// #include <algorithm>
#include "common_class_time.h"


using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned short TimeParam::parse_time_string(const char* time_str, TimeParam& time_param)
{
	if (time_str == NULL)
		throw invalid_argument(string("time_str should NOT be NULL"));
	static const char *QUARTER_DELIM = "q";
	static const char *DELIM = "-";
	char time_tmp_str[16];
	memset(time_tmp_str, 0x0, sizeof(char) * 16);
	memcpy(time_tmp_str, time_str, sizeof(char) * strlen(time_str));
	time_param.time_unit = TIME_UNIT_NONE;
	char* pos = strcasestr(time_tmp_str, QUARTER_DELIM);
	if (pos != NULL)
	{
// Is Quarter format
		int pos_index = pos - time_tmp_str;
		time_tmp_str[pos_index] = '\0';
		time_param.time_unit = TIME_UNIT_QUARTER;
		time_param.param.quarter_cfg.year = atoi(time_tmp_str);
		time_param.param.quarter_cfg.quarter = atoi(&time_tmp_str[pos_index + 1]);
	}
	else
	{
		char* pch;
		pch = strtok(time_tmp_str, DELIM);
		int count = 0;
		while (pch != NULL)
		{
			time_param.param.value[count++] = atoi(pch);
	//		printf ("%s, %d\n", pch, atoi(pch));
			pch = strtok(NULL, DELIM);
			// count++;
		}
		if (count == 2)
			time_param.time_unit = TIME_UNIT_MONTH;
		else if (count == 3)
			time_param.time_unit = TIME_UNIT_DATE;
	}
	if (time_param.time_unit == TIME_UNIT_NONE)
	{
		char errmsg[64];
		snprintf(errmsg, 64, "Incorrect time format: %s", time_str);
		throw invalid_argument(errmsg);
	}
	return RET_SUCCESS;
}

unsigned short TimeParam::create_instance_from_string(const char* time_str, TimeParam& time_param)
{
	return parse_time_string(time_str, time_param);
}

unsigned short TimeParam::create_instance_from_string(const char* time_str, TimeParam** time_param)
{
	assert(time_param != NULL && "time_param should NOT be NULL");
	TimeParam* time_param_tmp = new TimeParam();
	if (time_param_tmp == NULL)
	{
		STATIC_WRITE_ERROR("Fail to allocate the memory: time_param_tmp");
		if (time_param_tmp != NULL) delete time_param_tmp;
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	unsigned short ret = create_instance_from_string(time_str, *time_param_tmp);
	if (CHECK_FAILURE(ret))
	{
		delete time_param_tmp;
		time_param_tmp = NULL;
	}
	*time_param = time_param_tmp;
	return ret;
}

int TimeParam::get_date_int_value()const
{
	return ((param.value[0] & 0xFFFF) << 16) | ((param.value[1] & 0xFF) << 8) | (param.value[2] & 0xFF);
}

int TimeParam::get_month_int_value()const
{
	return ((param.value[0] & 0xFF) << 8) | (param.value[1] & 0xFF);
}

int TimeParam::get_quarter_int_value()const
{
	return ((param.value[0] & 0xFF) << 4) | (param.value[1] & 0xF);
}

int TimeParam::get_int_value()const
{
	typedef int (TimeParam::*GET_INT_VALUE_FUNC_PTR)()const;
	static GET_INT_VALUE_FUNC_PTR get_int_value_func_array[] =
	{
		&TimeParam::get_date_int_value,
		&TimeParam::get_month_int_value,
		&TimeParam::get_quarter_int_value
	};
	return (this->*(get_int_value_func_array[time_unit]))();
}

void TimeParam::find_date_time_string()
{
	snprintf(time_str, 16, "%04d-%02d-%02d", param.value[0], param.value[1], param.value[2]);
}

void TimeParam::find_month_time_string()
{
	snprintf(time_str, 16, "%04d-%02d", param.value[0], param.value[1]);
}

void TimeParam::find_quarter_time_string()
{
	snprintf(time_str, 16, "%04dq%d", param.value[0], param.value[1]);
}

void TimeParam::find_time_string()
{
	typedef void (TimeParam::*FIND_TIME_STRING_FUNC_PTR)();
	static FIND_TIME_STRING_FUNC_PTR find_time_string_func_array[] =
	{
		&TimeParam::find_date_time_string,
		&TimeParam::find_month_time_string,
		&TimeParam::find_quarter_time_string
	};
	(this->*(find_time_string_func_array[time_unit]))();
}

void TimeParam::update_param_from_value_array(TimeUnit cur_time_unit, const int* cur_value_array)
{
	assert(cur_value_array != NULL && "cur_value_array should NOT be NULL");
	time_unit = cur_time_unit;
	switch (time_unit)
	{
		case TIME_UNIT_DATE:
			memcpy(param.value, cur_value_array, sizeof(int) * 3);
			break;
		case TIME_UNIT_MONTH:
		case TIME_UNIT_QUARTER:
			memcpy(param.value, cur_value_array, sizeof(int) * 2);
			break;
		default:
		{
			char errmsg[64];
			snprintf(errmsg, 64, "Unknown time unit: %d", time_unit);
			throw invalid_argument(errmsg);
		}
		break;
	}
	find_time_string();
}

bool TimeParam::check_param_equal_from_value_array(TimeUnit another_time_unit, const int* another_value_array)const
{
	assert(another_value_array != NULL && "another_value_array should NOT be NULL");
	if (time_unit != another_time_unit)
		return false;
	switch (time_unit)
	{
		case TIME_UNIT_DATE:
		{
			if (memcmp(param.value, another_value_array, sizeof(int) * 3) != 0)
				return false;
		}
		break;
		case TIME_UNIT_MONTH:
		case TIME_UNIT_QUARTER:
		{
			if (memcmp(param.value, another_value_array, sizeof(int) * 2) != 0)
				return false;
		}
		break;
		default:
		{
			char errmsg[64];
			snprintf(errmsg, 64, "Unknown time unit: %d", time_unit);
			throw invalid_argument(errmsg);
		}
		break;
	}
	return true;
}

TimeParam::TimeParam() :
	time_unit(TIME_UNIT_NONE)
{
	IMPLEMENT_MSG_DUMPER()
}

TimeParam::TimeParam(const char* cur_time_str)
{
	IMPLEMENT_MSG_DUMPER()
	parse_time_string(cur_time_str, *this);
	snprintf(time_str, 16, "%s", cur_time_str);
}

TimeParam::TimeParam(TimeUnit cur_time_unit, int* cur_value_array)
{
	IMPLEMENT_MSG_DUMPER()
	update_param_from_value_array(cur_time_unit, cur_value_array);
}

TimeParam::TimeParam(const TimeParam* another_time_param)
{
	IMPLEMENT_MSG_DUMPER()
	assert(another_time_param != NULL && "another_time_param should NOT be NULL");
	update_param_from_value_array(another_time_param->time_unit, another_time_param->param.value);
}

TimeParam::~TimeParam()
{
	RELEASE_MSG_DUMPER()
}

TimeParam& TimeParam::operator=(const TimeParam& another)
{
	if (this == &another)
		return *this;
	update_param_from_value_array(another.time_unit, another.param.value);
	return *this;
}

const char* TimeParam::to_string()const{return time_str;}
TimeUnit TimeParam::get_time_unit()const{return time_unit;}

bool TimeParam::equal_to(const TimeParam* another_time_param)const
{
	assert(another_time_param != NULL && "another_time_param should NOT be NULL");
	return check_param_equal_from_value_array(another_time_param->time_unit, another_time_param->param.value);
}

void TimeParam::reset(const char* new_time_str)
{
	assert(new_time_str != NULL && "new_time_str should NOT be NULL");
	parse_time_string(new_time_str, *this);
	snprintf(time_str, 16, "%s", new_time_str);
}

void TimeParam::reset(TimeUnit new_time_unit, int* new_value_array)
{
	update_param_from_value_array(new_time_unit, new_value_array);
}

void TimeParam::reset(const TimeParam* new_time_param)
{
	assert(new_time_param != NULL && "new_time_param should NOT be NULL");
	update_param_from_value_array(new_time_param->time_unit, new_time_param->param.value);
}

bool TimeParam::operator<(const TimeParam& another_time_param)const
{
	if (this == &another_time_param)
		return false;
	assert(time_unit == another_time_param.time_unit && "The time_unit is NOT identical");
//	fprintf(stderr, "< %d, %d, %s\n", get_int_value(), another_time_param.get_int_value(), ((get_int_value() < another_time_param.get_int_value() ? "True" : "False")));
	return (get_int_value() < another_time_param.get_int_value());
}

bool TimeParam::operator>(const TimeParam& another_time_param)const
{
	if (this == &another_time_param)
		return false;
	assert(time_unit == another_time_param.time_unit && "The time_unit is NOT identical");
//	fprintf(stderr, "> %d, %d, %s\n", get_int_value(), another_time_param.get_int_value(), ((get_int_value() > another_time_param.get_int_value() ? "True" : "False")));
	return (get_int_value() > another_time_param.get_int_value());
}

bool TimeParam::operator==(const TimeParam& another_time_param)const
{
	if (this == &another_time_param)
		return false;
	assert(time_unit == another_time_param.time_unit && "The time_unit is NOT identical");
//	fprintf(stderr, "== %d, %d, %s\n", get_int_value(), another_time_param.get_int_value(), ((get_int_value() < another_time_param.get_int_value() ? "True" : "False")));
	return (get_int_value() == another_time_param.get_int_value());
}

bool TimeParam::operator>=(const TimeParam& another_time_param)const {return !(*this < another_time_param);}
bool TimeParam::operator<=(const TimeParam& another_time_param)const {return !(*this > another_time_param);}
bool TimeParam::operator!=(const TimeParam& another_time_param)const {return !(*this == another_time_param);}

bool TimeParam::operator<(const char* another_time_str)const
{
	TimeParam another_time_param;
	parse_time_string(another_time_str, another_time_param);
	assert(time_unit == another_time_param.time_unit && "The time_unit is NOT identical");
	return (get_int_value() < another_time_param.get_int_value());
}

bool TimeParam::operator>(const char* another_time_str)const
{
	TimeParam another_time_param;
	parse_time_string(another_time_str, another_time_param);
	assert(time_unit == another_time_param.time_unit && "The time_unit is NOT identical");
	return (get_int_value() > another_time_param.get_int_value());
}

bool TimeParam::operator==(const char* another_time_str)const
{
	TimeParam another_time_param;
	parse_time_string(another_time_str, another_time_param);
	assert(time_unit == another_time_param.time_unit && "The time_unit is NOT identical");
	return (get_int_value() == another_time_param.get_int_value());
}

bool TimeParam::operator>=(const char* another_time_str)const {return !(*this < another_time_str);}
bool TimeParam::operator<=(const char* another_time_str)const {return !(*this > another_time_str);}
bool TimeParam::operator!=(const char* another_time_str)const {return !(*this == another_time_str);}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TimeInRangeType TimeRangeParam::time_in_range_type(const TimeRangeParam* time_range_param, const TimeParam* time_param)
{
	assert(time_range_param != NULL && "time_range_param should NOT be NULL");
	assert(time_range_param->is_add_time_done() && "time_range_param is NOT add-time-done");
	if (*time_param < *(time_range_param->get_start_time()))
		return TIME_BEFORE_RANGE;
	else if (*time_param > *(time_range_param->get_end_time()))
		return TIME_AFTER_RANGE;
	else
		return TIME_IN_RANGE;
}

TimeInRangeType TimeRangeParam::time_in_range_type(const TimeRangeParam* time_range_param, const char* time_str)
{
	assert(time_str != NULL && "time_str should NOT be NULL");
	TimeParam time_param(time_str);
	return time_in_range_type(time_range_param, &time_param);
}

bool TimeRangeParam::time_in_range(const TimeRangeParam* time_range_param, const TimeParam* time_param)
{
	return (time_in_range_type(time_range_param, time_param) == TIME_IN_RANGE);
}

unsigned short TimeRangeParam::create_instance_from_string(const char* time_str, TimeRangeParam& time_range_param)
{
	assert(time_str != NULL && "time_str should NOT be NULL");
	int time_str_copy_size = strlen(time_str) + 1;
	char* time_str_copy = new char[time_str_copy_size];
	if (time_str_copy == NULL)
	{
		STATIC_WRITE_ERROR("Fail to allocate the memory: time_str_copy");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	memcpy(time_str_copy, time_str, sizeof(char) * time_str_copy_size);
	unsigned short ret = RET_SUCCESS;
	char* time_str_start = NULL; 
	char* time_str_end = NULL;
	char* comma_pos = strchr(time_str_copy, ',');
	if (comma_pos == NULL)
	{
// Only the start time
		time_str_start = time_str_copy;
	}
	else
	{
		int comma_index = comma_pos - time_str_copy;
		if (comma_index == 0)
		{
// Only the end time
			time_str_end = &time_str_copy[1];
		}
		else
		{
// The time range
			time_str_copy[comma_index] = '\0';
			time_str_start = time_str_copy;
			time_str_end = &time_str_copy[comma_index + 1];
		}
	}
	time_range_param.add_time(time_str_start, time_str_end);
	ret = time_range_param.add_time_done();
	if (time_str_copy != NULL)
	{
		delete time_str_copy;
		time_str_copy = NULL;
	}
	return ret;
}

unsigned short TimeRangeParam::create_instance_from_string(const char* time_str, TimeRangeParam** time_range_param)
{
	assert(time_range_param != NULL && "time_range_param should NOT be NULL");
	TimeRangeParam* time_range_param_tmp = new TimeRangeParam();
	if (time_range_param_tmp == NULL)
	{
		STATIC_WRITE_ERROR("Fail to allocate the memory: time_range_param_tmp");
		if (time_range_param_tmp != NULL) delete time_range_param_tmp;
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	unsigned short ret = create_instance_from_string(time_str, *time_range_param_tmp);
	if (CHECK_FAILURE(ret))
	{
		delete time_range_param_tmp;
		time_range_param_tmp = NULL;
	}
	*time_range_param = time_range_param_tmp;
	return ret;
}

void TimeRangeParam::update_time_unit(const TimeParam* time_param_start, const TimeParam* time_param_end)
{
	if (time_param_start != NULL && time_param_end != NULL)
	{
		if (time_param_start->get_time_unit() != time_param_end->get_time_unit())
		{
			char errmsg[64];
			snprintf(errmsg, 64, "The time unit is NOT identical, start: %s, end: %s", time_param_start->to_string(), time_param_end->to_string());
			throw invalid_argument(errmsg);
		}
	}
	else if (time_param_start != NULL)
		time_unit = time_param_start->get_time_unit();
	else if (time_param_end != NULL)
		time_unit = time_param_end->get_time_unit();
	else
		throw invalid_argument("time_param_start and time_param_end should NOT be NULL simultaneously");
}

TimeRangeParam::TimeRangeParam() :
	time_start_param(NULL),
	time_end_param(NULL),
	time_range_description(NULL),
	time_unit(TIME_UNIT_NONE),
	add_done(false)
{
	IMPLEMENT_MSG_DUMPER()
}

TimeRangeParam::TimeRangeParam(const char* time_str_start, const char* time_str_end) :
	time_start_param(NULL),
	time_end_param(NULL),
	time_range_description(NULL),
	time_unit(TIME_UNIT_NONE),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()

	if (time_str_start != NULL)
	{
		time_start_param = new TimeParam(time_str_start);
		if (time_start_param == NULL)
			throw bad_alloc();
	}
	if (time_str_end != NULL)
	{
		time_end_param = new TimeParam(time_str_end);
		if (time_end_param == NULL)
			throw bad_alloc();
	}
	update_time_unit(time_start_param, time_end_param);
}

TimeRangeParam::TimeRangeParam(TimeUnit time_unit_start, int* value_array_start, TimeUnit time_unit_end, int* value_array_end) :
	time_start_param(NULL),
	time_end_param(NULL),
	time_range_description(NULL),
	time_unit(TIME_UNIT_NONE),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()
	time_start_param = new TimeParam(time_unit_start, value_array_start);
	if (time_start_param == NULL)
		throw bad_alloc();
	time_end_param = new TimeParam(time_unit_end, value_array_end);
	if (time_end_param == NULL)
		throw bad_alloc();
	update_time_unit(time_start_param, time_end_param);
}

TimeRangeParam::TimeRangeParam(const TimeParam* time_param_start, const TimeParam* time_param_end) :
	time_start_param(NULL),
	time_end_param(NULL),
	time_range_description(NULL),
	time_unit(TIME_UNIT_NONE),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()
	time_start_param = new TimeParam(time_param_start);
	if (time_start_param == NULL)
		throw bad_alloc();
	time_end_param = new TimeParam(time_param_end);
	if (time_end_param == NULL)
		throw bad_alloc();
	update_time_unit(time_start_param, time_end_param);
}

TimeRangeParam::TimeRangeParam(const TimeRangeParam& another_time_range_param) :
	time_start_param(NULL),
	time_end_param(NULL),
	time_range_description(NULL),
	time_unit(TIME_UNIT_NONE),
	add_done(true)
{
	IMPLEMENT_MSG_DUMPER()
	time_start_param = new TimeParam(another_time_range_param.time_start_param);
	if (time_start_param == NULL)
		throw bad_alloc();
	time_end_param = new TimeParam(another_time_range_param.time_end_param);
	if (time_end_param == NULL)
		throw bad_alloc();
	update_time_unit(time_start_param, time_end_param);
}

TimeRangeParam::~TimeRangeParam()
{
	if (time_start_param != NULL)
	{
		delete time_start_param;
		time_start_param = NULL;
	}
	if (time_end_param != NULL)
	{
		delete time_end_param;
		time_end_param = NULL;
	}
	if (time_range_description != NULL)
	{
		delete time_range_description;
		time_range_description = NULL;
	}
	RELEASE_MSG_DUMPER()
}

void TimeRangeParam::add_start_time(const PTIME_PARAM new_time_param_start)
{
	if (add_done)
	{
		WRITE_ERROR("the add_done flag is true");
		throw runtime_error(string("the add_done flag is true"));
	}
	assert(new_time_param_start != NULL && "new_time_param_start should NOT be NULL");
	time_start_param = new TimeParam(new_time_param_start);
	if (time_start_param == NULL)
		throw bad_alloc();
}

void TimeRangeParam::add_end_time(const PTIME_PARAM new_time_param_end)
{
	if (add_done)
	{
		WRITE_ERROR("the add_done flag is true");
		throw runtime_error(string("the add_done flag is true"));
	}
	assert(new_time_param_end != NULL && "new_time_param_end should NOT be NULL");
	time_end_param = new TimeParam(new_time_param_end);
	if (time_end_param == NULL)
		throw bad_alloc();
}

void TimeRangeParam::add_time(const PTIME_PARAM new_time_param_start, const PTIME_PARAM new_time_param_end)
{
	assert((new_time_param_start != NULL || new_time_param_end != NULL) && "new_time_param_start/new_time_param_end should NOT be both NULL");
	if (new_time_param_start != NULL)
		add_start_time(new_time_param_start);
	if (new_time_param_end != NULL)
		add_end_time(new_time_param_end);
}

void TimeRangeParam::add_start_time(const char* new_time_str_start)
{
	assert(new_time_str_start != NULL && "new_time_str_start should NOT be NULL");
	TimeParam new_time_param_start(new_time_str_start);
	add_start_time((const PTIME_PARAM)&new_time_param_start);
}

void TimeRangeParam::add_end_time(const char* new_time_str_end)
{
	assert(new_time_str_end != NULL && "new_time_str_end should NOT be NULL");
	TimeParam new_time_param_end(new_time_str_end);
	add_end_time((const PTIME_PARAM)&new_time_param_end);
}

void TimeRangeParam::add_time(const char* new_time_str_start, const char* new_time_str_end)
{
	assert((new_time_str_start != NULL || new_time_str_end != NULL) && "new_time_str_start/new_time_str_end should NOT be both NULL");
	if (new_time_str_start != NULL)
		add_start_time(new_time_str_start);
	if (new_time_str_end != NULL)
		add_end_time(new_time_str_end);
}

unsigned short TimeRangeParam::add_time_done()
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
	if (time_start_param == NULL)
		add_start_time(START_DATE_STR);
	if (time_end_param == NULL)
		add_end_time(END_DATE_STR);
	add_done = true;
	return RET_SUCCESS;
}

void TimeRangeParam::reset_start_time(const PTIME_PARAM new_time_param_start)
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
	assert(new_time_param_start != NULL && "new_time_param_start should NOT be NULL");
	add_done = false;
	if (time_start_param == NULL)
		add_start_time(new_time_param_start);
	else
		time_start_param->reset(new_time_param_start);
	add_time_done();
}

void TimeRangeParam::reset_end_time(const PTIME_PARAM new_time_param_end)
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
	assert(new_time_param_end != NULL && "new_time_param_end should NOT be NULL");
	add_done = false;
	if (time_end_param == NULL)
		add_end_time(new_time_param_end);
	else
		time_end_param->reset(new_time_param_end);
	add_time_done();
}

void TimeRangeParam::reset_time(const PTIME_PARAM new_time_param_start, const PTIME_PARAM new_time_param_end)
{
	if (new_time_param_start != NULL)
		reset_start_time(new_time_param_start);
	if (new_time_param_end != NULL)
		reset_end_time(new_time_param_end);
}

void TimeRangeParam::reset_start_time(const char* new_time_str_start)
{
	assert(new_time_str_start != NULL && "new_time_str_start should NOT be NULL");
	TimeParam new_time_param_start(new_time_str_start);
	reset_start_time((const PTIME_PARAM)&new_time_param_start);
}

void TimeRangeParam::reset_end_time(const char* new_time_str_end)
{
	assert(new_time_str_end != NULL && "new_time_str_end should NOT be NULL");
	TimeParam new_time_param_end(new_time_str_end);
	reset_end_time((const PTIME_PARAM)&new_time_param_end);
}

void TimeRangeParam::reset_time(const char* new_time_str_start, const char* new_time_str_end)
{
	if (new_time_str_start != NULL)
		reset_start_time(new_time_str_start);
	if (new_time_str_end != NULL)
		reset_end_time(new_time_str_end);
}

bool TimeRangeParam::is_single_time()const
{
	if (time_start_param != NULL && time_end_param != NULL)
		return time_start_param->equal_to(time_end_param);
	return false;
}

bool TimeRangeParam::is_add_time_done()const{return add_done;}

const char* TimeRangeParam::to_string()
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
		if (time_start_param != NULL && time_end_param != NULL)
			snprintf(time_range_description, 32, "%s:%s", time_start_param->to_string(), time_end_param->to_string());
		else if (time_start_param != NULL)
			snprintf(time_range_description, 32, "%s", time_start_param->to_string());
		else if (time_end_param != NULL)
			snprintf(time_range_description, 32, ",%s", time_end_param->to_string());
	}
	return time_range_description;
}

const PTIME_PARAM TimeRangeParam::get_start_time()const
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
//	assert(time_start_param != NULL && "time_start_param should NOT be NULL");
	return time_start_param;
}

const PTIME_PARAM TimeRangeParam::get_end_time()const
{
	if (!add_done)
	{
		WRITE_ERROR("the add_done flag is NOT true");
		throw runtime_error(string("the add_done flag is NOT true"));
	}
//	assert(time_end_param != NULL && "time_end_param should NOT be NULL");
	return time_end_param;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SingleTimeRangeParam::SingleTimeRangeParam(const char* time_str) : TimeRangeParam(time_str, time_str){}
SingleTimeRangeParam::SingleTimeRangeParam(TimeUnit time_unit, int* value_array) : TimeRangeParam(time_unit, value_array, time_unit, value_array){}
SingleTimeRangeParam::SingleTimeRangeParam(const TimeParam* time_param) : TimeRangeParam(time_param, time_param){}
