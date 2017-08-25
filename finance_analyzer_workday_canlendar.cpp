#include <assert.h>
#include <stdexcept>
#include <string>
#include <algorithm>
#include "finance_analyzer_workday_canlendar.h"


using namespace std;

static 	pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

FinanceAnalyzerWorkdayCanlendar* FinanceAnalyzerWorkdayCanlendar::instance = NULL;

FinanceAnalyzerWorkdayCanlendar* FinanceAnalyzerWorkdayCanlendar::get_instance()
{
	if (instance == NULL)
	{
		pthread_mutex_lock(&mtx);
		if (instance == NULL)
		{
			instance = new FinanceAnalyzerWorkdayCanlendar();
			if (instance == NULL)
				throw bad_alloc();
			unsigned short ret = instance->initialize();
			if (CHECK_FAILURE(ret))
			{
				char errmsg[256];
				snprintf(errmsg, 256, "Fail to initialize the FinanceAnalyzerWorkdayCanlendar object , due to: %s", get_ret_description(ret));
				throw runtime_error(string(errmsg));
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	instance->add_ref();
	return instance;
}

FinanceAnalyzerWorkdayCanlendar::FinanceAnalyzerWorkdayCanlendar() :
	ref_cnt(0),
	time_range_cfg(NULL),
	map_start_time_cfg(NULL),
	map_end_time_cfg(NULL),
	workday_year_sort_queue_size(0)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerWorkdayCanlendar::~FinanceAnalyzerWorkdayCanlendar()
{
	for(YEAR_MAP::iterator iter = workday_map.begin() ; iter != workday_map.end() ; iter++)
	{
		PDAY_DEQUE day_deque_ptr = (PDAY_DEQUE)iter->second;
		delete[] day_deque_ptr;
	}
	workday_map.clear();

	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerWorkdayCanlendar::initialize()
{
	CREATE_PROJECT_FILEPATH(file_path, CONFIG_FOLDER_NAME, WORKDAY_CANLENDAR_CONF_FILENAME)

// First check if the config file exists
	if (!check_file_exist(file_path))
	{
		char errmsg[256];
		snprintf(errmsg, 256, "The %s config file does NOT exist", WORKDAY_CANLENDAR_CONF_FILENAME);
		WRITE_ERROR(errmsg);
		fprintf(stderr, "%s\n", errmsg);
		return RET_FAILURE_NOT_FOUND;
	}

	unsigned int BUF_SIZE = 1024;
	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
	if (buf == NULL)
	{
		WRITE_ERROR("Fail to allocate memoery: buf");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}

	unsigned short ret = RET_SUCCESS;
	WRITE_FORMAT_DEBUG("Parse the config file: %s", WORKDAY_CANLENDAR_CONF_FILENAME);
	FILE* fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the config file: %s, due to: %s", WORKDAY_CANLENDAR_CONF_FILENAME, strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
// Check if the buffer size is enough
		if (strlen(buf) == BUF_SIZE - 1 && buf[BUF_SIZE - 1] == '\0')
		{
			do
			{
				int BUF_SIZE_OLD = BUF_SIZE;
				char* buf_old = buf;
				BUF_SIZE <<= 1;
				buf = (char*)realloc(buf_old, sizeof(char) * BUF_SIZE);
				fgets(&buf[BUF_SIZE_OLD - 1], BUF_SIZE - BUF_SIZE_OLD, fp);
//				fprintf(stderr, "buf size: %d, last char: %d\n", strlen(buf), buf[strlen(buf)]);
//				fprintf(stderr, "buf: %s\n", buf);
			}while(strlen(buf) == BUF_SIZE - 1 && buf[BUF_SIZE - 1] == '\0');
		}
		if (time_range_cfg == NULL)
		{
			char start_time_str[16];
			char end_time_str[16];
			sscanf(buf, "%s %s", start_time_str, end_time_str);
			time_range_cfg = new TimeRangeCfg(start_time_str, end_time_str);
			WRITE_FORMAT_DEBUG("Find the time range [%s %s] in %s", start_time_str, end_time_str, WORKDAY_CANLENDAR_CONF_FILENAME);
			if (time_range_cfg == NULL)
			{
				WRITE_ERROR("Fail to allocate the memory: time_range_cfg");
				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
				goto OUT1;
			}
		}
		else
		{
			int year;
			sscanf(buf, "[%d]", &year);
			PDAY_DEQUE day_deque_ptr = new DAY_DEQUE[12];
			if (day_deque_ptr == NULL)
			{
				WRITE_ERROR("Fail to allocate memory: day_list_ptr");
				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
				goto OUT1;
			}
			workday_year_sort_queue.push_back(year);

			strtok(buf, "]");
			char* year_workday_list_str = strtok(NULL, "]");
			char* month_workday_list_str =  strtok(year_workday_list_str, ";");
			list<char*> month_workday_list;
			while (month_workday_list_str != NULL)
			{
				int month;
				char month_tmp[4];
				sscanf(month_workday_list_str, "%d:", &month);
				snprintf(month_tmp, 4, "%d:", month);
				int month_tmp_len = strlen(month_tmp);
				char* month_workday_list_str_tmp = new char[strlen(month_workday_list_str) - month_tmp_len + 1];
				if (month_workday_list_str_tmp == NULL)
				{
					WRITE_ERROR("Fail to allocate memory: month_workday_list_str_tmp");
					ret = RET_FAILURE_INSUFFICIENT_MEMORY;
					goto OUT1;
				}
				strcpy(month_workday_list_str_tmp, &month_workday_list_str[month_tmp_len]);
				month_workday_list.push_back(month_workday_list_str_tmp);
				if (month != (int)month_workday_list.size())
				{
					WRITE_FORMAT_ERROR("Incorrect month, expected: %d, actual: %d", month, (int)month_workday_list.size());
					ret = RET_FAILURE_INCORRECT_CONFIG;
					goto OUT1;
				}
				month_workday_list_str =  strtok(NULL, ";");
			}
			list<char*>::iterator iter = month_workday_list.begin();
			int month_count = 0;
			while (iter != month_workday_list.end())
			{
				char* month_workday_list_str = (char*)*iter;
				char* day_workday_str =  strtok(month_workday_list_str, ",");
				while (day_workday_str != NULL)
				{
					day_deque_ptr[month_count].push_back(atoi(day_workday_str));
					day_workday_str = strtok(NULL, ",");
				}
				delete month_workday_list_str;
				iter++;
				month_count++;
			}
			workday_map[year] = day_deque_ptr;
			month_workday_list.clear();
		}
	}
OUT1:
	fclose(fp);
	fp = NULL;
OUT:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
//	workday_year_sort_queue.sort();
	sort(workday_year_sort_queue.begin(), workday_year_sort_queue.end());
	workday_year_sort_queue_size = workday_year_sort_queue.size();
	return ret;
}

int FinanceAnalyzerWorkdayCanlendar::add_ref()
{
	__sync_fetch_and_add(&ref_cnt, 1);
	return ref_cnt;
}

int FinanceAnalyzerWorkdayCanlendar::release()
{
	__sync_fetch_and_sub(&ref_cnt, 1);
	if (ref_cnt == 0)
	{
		delete this;
		return 0;
	}
	return ref_cnt;
}

bool FinanceAnalyzerWorkdayCanlendar::check_in_range(int year, int month, int day)const
{
	return TimeRangeCfg::time_in_range(time_range_cfg, year, month, day);
}

bool FinanceAnalyzerWorkdayCanlendar::check_in_range(const PTIME_CFG time_cfg)const
{
	return TimeRangeCfg::time_in_range(time_range_cfg, time_cfg);
}

bool FinanceAnalyzerWorkdayCanlendar::check_greater_than_start(int year, int month, int day)const
{
	PTIME_CFG time_cfg = new TimeCfg(year, month, day);
	bool check = check_greater_than_start(time_cfg);
	if (time_cfg != NULL)
	{
		delete time_cfg;
		time_cfg = NULL;
	}
	return check;
}

bool FinanceAnalyzerWorkdayCanlendar::check_greater_than_start(const PTIME_CFG time_cfg)const
{
	if (time_cfg == NULL)
	{
		char errmsg[64];
		snprintf(errmsg, 64, "time_cfg should NOT be NULL");
		throw invalid_argument(errmsg);
	}

	return (*time_cfg >= *time_range_cfg->get_start_time());
}

bool FinanceAnalyzerWorkdayCanlendar::check_less_than_end(int year, int month, int day)const
{
	PTIME_CFG time_cfg = new TimeCfg(year, month, day);
	bool check = check_less_than_end(time_cfg);
	if (time_cfg != NULL)
	{
		delete time_cfg;
		time_cfg = NULL;
	}
	return check;
}

bool FinanceAnalyzerWorkdayCanlendar::check_less_than_end(const PTIME_CFG time_cfg)const
{
	if (time_cfg == NULL)
	{
		char errmsg[64];
		snprintf(errmsg, 64, "time_cfg should NOT be NULL");
		throw invalid_argument(errmsg);
	}

	return (*time_cfg <= *time_range_cfg->get_end_time());
}

bool FinanceAnalyzerWorkdayCanlendar::is_workday(int year, int month, int day)
{
	int year_key_dummy;
	int month_index_dummy;
	int day_index_dummy;

	return CHECK_SUCCESS(find_data_pos(year, month, day, year_key_dummy, month_index_dummy, day_index_dummy));
}

bool FinanceAnalyzerWorkdayCanlendar::is_workday(const PTIME_CFG time_cfg)
{
	assert(time_cfg != NULL && "time_cfg should NOT be NULL");
	return is_workday(time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day());
}

unsigned short FinanceAnalyzerWorkdayCanlendar::find_data_pos(int year, int month, int day, int& year_key, int& month_index, int& day_index, TRAVERSE_SEARCH_TYPE traverse_search_type)
{
// Check boundary first
//	if (traverse_search_type == TRAVERSE_SEARCH_PREV)
//	{
//		if (!check_greater_than_start(year, month, day))
//		{
//			WRITE_FORMAT_ERROR("The date [%04d-%02d-%02d] is NOT greater than the start time [%s]", year, month, day, time_range_cfg->get_start_time()->to_string());
//			return RET_FAILURE_INVALID_ARGUMENT;
//		}
//	}
//	else if (traverse_search_type == TRAVERSE_SEARCH_NEXT)
//	{
//		if (!check_less_than_end(year, month, day))
//		{
//			WRITE_FORMAT_ERROR("The date [%04d-%02d-%02d] is NOT less than the end time [%s]", year, month, day, time_range_cfg->get_end_time()->to_string());
//			return RET_FAILURE_INVALID_ARGUMENT;
//		}
//	}
//	else
//	{
//	}
	if (!check_in_range(year, month, day))
	{
//		char errmsg[256];
//		snprintf(errmsg, 256, "The date [%04d-%02d-%02d] is out of range [%s]", year, month, day, time_range_cfg->to_string());
		WRITE_FORMAT_ERROR("The date [%04d-%02d-%02d] is out of range [%s]", year, month, day, time_range_cfg->to_string());
		return RET_FAILURE_INVALID_ARGUMENT;
	}

	YEAR_MAP::iterator iter = workday_map.find(year);
	if (iter == workday_map.end())
	{
		WRITE_FORMAT_ERROR("Incorrect year: %04d", year);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
//	PDAY_LIST workday_month_array = workday_map[year];
//	PDAY_LIST workday_list = &workday_month_array[month - 1];
	PDAY_DEQUE day_deque = &workday_map[year][month - 1];
	assert(day_deque != NULL && "day_deque should NOT be NULL");
	int day_deque_size = (int)day_deque->size();
// Find the closest previous workday
	bool found = false;
	if (traverse_search_type == TRAVERSE_SEARCH_PREV)
	{
		for (int i = day_deque_size - 1 ; i >= 0 ; i--)
		{
			if (day > (*day_deque)[i])
			{
				year_key = year;
				month_index = month - 1;
				day_index = i;
				found = true;
				break;
			}
		}
// Find the last day of last month
		if (!found)
		{
			if (month == 1)
			{
				year--;
				month = 12;
			}
			else
				month--;
			if (workday_map.find(year) == workday_map.end())
			{
				WRITE_FORMAT_ERROR("The year [%04d] does NOT exist", year);
				return RET_FAILURE_NOT_FOUND;
			}
			PDAY_DEQUE day_deque_prev_month = &workday_map[year][month - 1];
			int day_deque_size = (int)day_deque_prev_month->size();
			if (day_deque_size == 0)
			{
				WRITE_FORMAT_ERROR("The day list in [%04d-%02d] should NOT be Empty", year, month);
				return RET_FAILURE_NOT_FOUND;
			}
			year_key = year;
			month_index = month - 1;
			day_index = day_deque_size - 1;
			found = true;
		}
		if (!found)
		{
			WRITE_FORMAT_ERROR("Fails to find the closest previous workday from [%04d-%02d-%02d]", year, month, day);
			return RET_FAILURE_NOT_FOUND;
		}
	}
// Find the closest next workday
	else if (traverse_search_type == TRAVERSE_SEARCH_NEXT)
	{
		for (int i = 0 ; i < day_deque_size ; i++)
		{
			if (day < (*day_deque)[i])
			{
				year_key = year;
				month_index = month - 1;
				day_index = i;
				found = true;
				break;
			}
		}
// Find the first day of next month
		if (!found)
		{
			if (month == 12)
			{
				year++;
				month = 1;
			}
			else
				month++;
			if (workday_map.find(year) == workday_map.end())
			{
				WRITE_FORMAT_ERROR("The year [%04d] does NOT exist", year);
				return RET_FAILURE_NOT_FOUND;
			}
			PDAY_DEQUE day_deque_next_month = &workday_map[year][month - 1];
			int day_deque_size = (int)day_deque_next_month->size();
			if (day_deque_size == 0)
			{
				WRITE_FORMAT_ERROR("The day list in [%04d-%02d] should NOT be Empty", year, month);
				return RET_FAILURE_NOT_FOUND;
			}
			year_key = year;
			month_index = month - 1;
			day_index = 0;
			found = true;
		}
		if (!found)
		{
			WRITE_FORMAT_ERROR("Fails to find the closest next workday from [%04d-%02d-%02d]", year, month, day);
			return RET_FAILURE_NOT_FOUND;
		}
	}
	else
	{
		for (int i = 0 ; i < day_deque_size ; i++)
		{
			if (day == (*day_deque)[i])
			{
				year_key = year;
				month_index = month - 1;
				day_index = i;
				found = true;
			}
		}
//		if (find(day_deque->begin(), day_deque->end(), day) != day_deque->end())
//			return false;
		if (!found)
		{
			WRITE_FORMAT_WARN("The date [%04d-%02d-%02d] is NOT FOUND", year, month, day);
			return RET_FAILURE_NOT_FOUND;
		}
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_date(int year_key, int month_index, int day_index, int& year, int& month, int& day)
{
// Check year key
	YEAR_MAP::iterator iter = workday_map.find(year_key);
	if (iter == workday_map.end())
	{
		WRITE_FORMAT_ERROR("Incorrect year key: %04d", year_key);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
// Check month index
	if (month_index < 0 || month_index >= 12)
	{
		WRITE_FORMAT_ERROR("Incorrect month index: %d, should be in the range[0, 12)", month_index);
		return RET_FAILURE_INVALID_ARGUMENT;
	}

	PDAY_DEQUE day_deque = &workday_map[year_key][month_index];
	assert(day_deque != NULL && "day_deque should NOT be NULL");
	int day_deque_size = (int)day_deque->size();
	if (day_index < 0 || day_index >= day_deque_size)
	{
		WRITE_FORMAT_ERROR("Incorrect day index: %d, should be in the range[0, %d)", day_index, day_deque_size);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	year = year_key;
	month = month_index + 1;
	day = (*day_deque)[day_index];
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_date(int year_key, int month_index, int day_index, SmartPointer<TimeCfg>& sp_time_cfg/*PTIME_CFG* time_cfg*/)
{
//	if (time_cfg == NULL)
//	{
//		WRITE_ERROR("time_cfg should NOT be NULL");
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
	int year;
	int month;
	int day;
//	*time_cfg = NULL;

	unsigned short ret = get_date(year_key, month_index, day_index, year, month, day);
	if (CHECK_SUCCESS(ret))
	{
		PTIME_CFG time_cfg_tmp = new TimeCfg(year, month, day);
		if (time_cfg_tmp == NULL)
		{
			WRITE_ERROR("Fail to allocate the memory: time_cfg_tmp");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
//		*time_cfg = time_cfg_tmp;
		sp_time_cfg.set_new(time_cfg_tmp);
	}
	return ret;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_prev_workday_array(int year_base, int month_base, int day_base, std::deque<PTIME_CFG>& workday_deque, int max_workday_amount)
{
	int start_year_key;
	int start_month_index;
	int start_day_index;
	WRITE_FORMAT_DEBUG("Try to search for the previous workday list from the date %04d-%02d-%02d......", year_base, month_base, day_base);
	unsigned short ret = find_data_pos(year_base, month_base, day_base, start_year_key, start_month_index, start_day_index, TRAVERSE_SEARCH_PREV);
	if (CHECK_FAILURE(ret))
		return ret;

	int year, month, day;
	ret = get_date(start_year_key, start_month_index, start_day_index, year, month, day);
// Should NOT fail
	if (CHECK_FAILURE(ret))
	{
		char errmsg[256];
		snprintf(errmsg, 256, "Fail to date from the parameter: year_key: %d, month_index: %d, day_index: %d, due to: %s", start_year_key, start_month_index, start_day_index, get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	WRITE_FORMAT_INFO("First workday in the previous workday list: %04d-%02d-%02d......", year, month, day);

	bool first_month = true;
	int start_year_index = start_year_key - workday_year_sort_queue[0];
	int workday_deque_count = 0;
	for (int year_index = start_year_index ; year_index >= 0 ; year_index--)
	{
		int cur_year = workday_year_sort_queue[year_index];
//		YEAR_MAP::iterator iter = workday_map.find(workday_year_sort_queue[year_index]);
		for (int month_index = start_month_index ; month_index >= 0 ; month_index--)
		{
			int cur_month = month_index + 1;
			PDAY_DEQUE day_deque = &workday_map[cur_year][month_index];
			if (!first_month)
				start_day_index = day_deque->size();
			for (int day_index = start_day_index ; day_index >= 0 ; day_index--)
			{
				PTIME_CFG time_cfg = new TimeCfg(cur_year, cur_month, (*day_deque)[day_index]);
				if (time_cfg == NULL)
				{
					WRITE_ERROR("Fail to allocate the memory: time_cfg");
					return RET_FAILURE_INSUFFICIENT_MEMORY;
				}
				workday_deque.push_back(time_cfg);
				workday_deque_count++;
				if (max_workday_amount != -1 && workday_deque_count == max_workday_amount)
					goto OUT;
			}
			if (first_month)
			{
				start_month_index = 11;
				first_month = false;
			}
		}
	}
OUT:
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_next_workday_array(int year_base, int month_base, int day_base, std::deque<PTIME_CFG>& workday_deque, int max_workday_amount)
{
	int start_year_key;
	int start_month_index;
	int start_day_index;
	WRITE_FORMAT_DEBUG("Try to search for the next workday list from the date %04d-%02d-%02d......", year_base, month_base, day_base);
	unsigned short ret = find_data_pos(year_base, month_base, day_base, start_year_key, start_month_index, start_day_index, TRAVERSE_SEARCH_NEXT);
	if (CHECK_FAILURE(ret))
		return ret;

	int year, month, day;
	ret = get_date(start_year_key, start_month_index, start_day_index, year, month, day);
// Should NOT fail
	if (CHECK_FAILURE(ret))
	{
		char errmsg[256];
		snprintf(errmsg, 256, "Fail to date from the parameter: year_key: %d, month_index: %d, day_index: %d, due to: %s", start_year_key, start_month_index, start_day_index, get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	WRITE_FORMAT_INFO("First workday in the next workday list: %04d-%02d-%02d......", year, month, day);

	bool first_month = true;
	int start_year_index = start_year_key - workday_year_sort_queue[0];
	int workday_deque_count = 0;
	int day_deque_size;
	for (int year_index = start_year_index ; year_index < workday_year_sort_queue_size ; year_index++)
	{
		int cur_year = workday_year_sort_queue[year_index];
//		YEAR_MAP::iterator iter = workday_map.find(workday_year_sort_queue[year_index]);
		for (int month_index = start_month_index ; month_index < 12 ; month_index++)
		{
			int cur_month = month_index + 1;
			PDAY_DEQUE day_deque = &workday_map[cur_year][month_index];
			if (!first_month)
				start_day_index = 0;
			day_deque_size = day_deque->size();
			for (int day_index = start_day_index ; day_index < day_deque_size ; day_index++)
			{
				PTIME_CFG time_cfg = new TimeCfg(cur_year, cur_month, (*day_deque)[day_index]);
				if (time_cfg == NULL)
				{
					WRITE_ERROR("Fail to allocate the memory: time_cfg");
					return RET_FAILURE_INSUFFICIENT_MEMORY;
				}
				workday_deque.push_back(time_cfg);
				workday_deque_count++;
				if (max_workday_amount != -1 && workday_deque_count == max_workday_amount)
					goto OUT;
			}
			if (first_month)
			{
				start_month_index = 0;
				first_month = false;
			}
		}
	}
OUT:
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_prev_workday(int year_base, int month_base, int day_base, int& prev_year, int& prev_month, int& prev_day)
{
// Find the date
	deque<PTIME_CFG> workday_deque;
	unsigned short ret = get_prev_workday_array(year_base, month_base, day_base, workday_deque, 1);
	if (CHECK_FAILURE(ret))
		return ret;
	if (workday_deque.empty())
	{
		WRITE_WARN("No data is FOUND");
		return RET_FAILURE_NOT_FOUND;
	}
// Update the data
	PTIME_CFG time_cfg = workday_deque[0];
	prev_year = time_cfg->get_year();
	prev_month = time_cfg->get_month();
	prev_day = time_cfg->get_day();
// Clear up
	delete time_cfg;
	workday_deque.clear();

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_prev_workday(const PTIME_CFG time_cfg, SmartPointer<TimeCfg>& sp_prev_time_cfg/*PTIME_CFG* prev_time_cfg*/)
{
	if (time_cfg == NULL/*|| prev_time_cfg == NULL*/)
	{
		WRITE_ERROR("time_cfg == NULL or prev_time_cfg == NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	int prev_year, prev_month, prev_day;
//	*prev_time_cfg = NULL;
	unsigned short ret = get_prev_workday(time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day(), prev_year, prev_month, prev_day);
	if (CHECK_SUCCESS(ret))
	{
		PTIME_CFG prev_time_cfg_tmp = new TimeCfg(prev_year, prev_month, prev_day);
		if (prev_time_cfg_tmp == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: prev_time_cfg_tmp");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
//		*prev_time_cfg = prev_time_cfg_tmp;
		sp_prev_time_cfg.set_new(prev_time_cfg_tmp);
	}

	return ret;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_next_workday(int year_base, int month_base, int day_base, int& next_year, int& next_month, int& next_day)
{
// Find the date
	deque<PTIME_CFG> workday_deque;
	unsigned short ret = get_next_workday_array(year_base, month_base, day_base, workday_deque, 1);
	if (CHECK_FAILURE(ret))
		return ret;
	if (workday_deque.empty())
	{
		WRITE_WARN("No data is FOUND");
		return RET_FAILURE_NOT_FOUND;
	}
// Update the data
	PTIME_CFG time_cfg = workday_deque[0];
	next_year = time_cfg->get_year();
	next_month = time_cfg->get_month();
	next_day = time_cfg->get_day();
// Clear up
	delete time_cfg;
	workday_deque.clear();

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_next_workday(const PTIME_CFG time_cfg, SmartPointer<TimeCfg>& sp_next_time_cfg/*PTIME_CFG* next_time_cfg*/)
{
	if (time_cfg == NULL/*|| next_time_cfg == NULL*/)
	{
		WRITE_ERROR("time_cfg == NULL or next_time_cfg == NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	int next_year, next_month, next_day;
//	*next_time_cfg = NULL;
	unsigned short ret = get_next_workday(time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day(), next_year, next_month, next_day);
	if (CHECK_SUCCESS(ret))
	{
		PTIME_CFG next_time_cfg_tmp = new TimeCfg(next_year, next_month, next_day);
		if (next_time_cfg_tmp == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: next_time_cfg_tmp");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
//		*next_time_cfg = next_time_cfg_tmp;
		sp_next_time_cfg.set_new(next_time_cfg_tmp);
	}

	return ret;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_first_workday(int& first_year, int& first_month, int& first_day)
{
	first_year = *workday_year_sort_queue.begin();
	for (int month_index = 0 ; month_index < 12 ; month_index++)
	{
		PDAY_DEQUE day_deque = &workday_map[first_year][month_index];
		if (!day_deque->empty())
		{
			first_month = month_index + 1;
			first_day = *day_deque->begin();
			break;
		}
	}
	WRITE_FORMAT_DEBUG("The first workday: %04d-%02d-%02d", first_year, first_month, first_day);
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_first_workday(SmartPointer<TimeCfg>& sp_first_time_cfg/*PTIME_CFG* first_time_cfg*/)
{
//	if (first_time_cfg == NULL)
//	{
//		WRITE_ERROR("first_time_cfg should NOT be NULL");
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}

	int first_year, first_month, first_day;
//	*first_time_cfg = NULL;
	unsigned short ret = get_first_workday(first_year, first_month, first_day);
	if (CHECK_SUCCESS(ret))
	{
		PTIME_CFG first_time_cfg_tmp = new TimeCfg(first_year, first_month, first_day);
		if (first_time_cfg_tmp == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: first_time_cfg_tmp");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
//		*first_time_cfg = first_time_cfg_tmp;
		sp_first_time_cfg.set_new(first_time_cfg_tmp);
	}
	return ret;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_last_workday(int& last_year, int& last_month, int& last_day)
{
	last_year = *workday_year_sort_queue.rbegin();
	for (int month_index = 11 ; month_index >= 0 ; month_index--)
	{
		PDAY_DEQUE day_deque = &workday_map[last_year][month_index];
		if (!day_deque->empty())
		{
			last_month = month_index + 1;
			last_day = *day_deque->rbegin();
			break;
		}
	}
	WRITE_FORMAT_DEBUG("The last workday: %04d-%02d-%02d", last_year, last_month, last_day);
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerWorkdayCanlendar::get_last_workday(SmartPointer<TimeCfg>& sp_last_time_cfg/*PTIME_CFG* last_time_cfg*/)
{
//	if (last_time_cfg == NULL)
//	{
//		WRITE_ERROR("last_time_cfg should NOT be NULL");
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}

	int last_year, last_month, last_day;
//	*last_time_cfg = NULL;
	unsigned short ret = get_last_workday(last_year, last_month, last_day);
	if (CHECK_SUCCESS(ret))
	{
		PTIME_CFG last_time_cfg_tmp = new TimeCfg(last_year, last_month, last_day);
		if (last_time_cfg_tmp == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: last_time_cfg_tmp");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
//		*last_time_cfg = last_time_cfg_tmp;
		sp_last_time_cfg.set_new(last_time_cfg_tmp);
	}
	return ret;
}
