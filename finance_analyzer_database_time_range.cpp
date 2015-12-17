#include <assert.h>
#include <stdexcept>
#include <string>
#include <algorithm>
#include "finance_analyzer_database_time_range.h"


using namespace std;

static 	pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerDatabaseTimeRange* FinanceAnalyzerDatabaseTimeRange::instance = NULL;

FinanceAnalyzerDatabaseTimeRange* FinanceAnalyzerDatabaseTimeRange::get_instance()
{
	if (instance == NULL)
	{
		pthread_mutex_lock(&mtx);
		if (instance == NULL)
		{
			instance = new FinanceAnalyzerDatabaseTimeRange();
			if (instance == NULL)
				throw bad_alloc();
			unsigned short ret = instance->initialize();
			if (CHECK_FAILURE(ret))
			{
				char errmsg[256];
				snprintf(errmsg, 256, "Fail to initialize the FinanceAnalyzerDatabaseTimeRange object , due to: %s", get_ret_description(ret));
				throw runtime_error(string(errmsg));
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	instance->add_ref();
	return instance;
}

FinanceAnalyzerDatabaseTimeRange::FinanceAnalyzerDatabaseTimeRange() :
	ref_cnt(0)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerDatabaseTimeRange::~FinanceAnalyzerDatabaseTimeRange()
{
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerDatabaseTimeRange::initialize()
{
	static const int FILE_PATH_SIZE = 256;
	static char file_path[FILE_PATH_SIZE];
	char current_working_directory[FILE_PATH_SIZE];
	getcwd(current_working_directory, FILE_PATH_SIZE);
	snprintf(file_path, FILE_PATH_SIZE, "%s/%s/%s", current_working_directory, CONFIG_FOLDER_NAME, DATABASE_TIME_RANGE_CONF_FILENAME);

// First check if the config file exists
	if (!check_file_exist(file_path))
	{
		char errmsg[256];
		snprintf(errmsg, 256, "The %s config file does NOT exist", DATABASE_TIME_RANGE_CONF_FILENAME);
		WRITE_ERROR(errmsg);
		fprintf(stderr, "%s\n", errmsg);
		return RET_FAILURE_NOT_FOUND;
	}

	unsigned int BUF_SIZE = 256;
	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
	if (buf == NULL)
	{
		WRITE_ERROR("Fail to allocate memoery: buf");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}

	unsigned short ret = RET_SUCCESS;
	WRITE_FORMAT_DEBUG("Parse the config file: %s", DATABASE_TIME_RANGE_CONF_FILENAME);
	FILE* fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the config file: %s, due to: %s", DATABASE_TIME_RANGE_CONF_FILENAME, strerror(errno));
		free(buf);
		return RET_FAILURE_SYSTEM_API;
	}
	int source_type_index_count = 0;
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
			}while(strlen(buf) == BUF_SIZE - 1 && buf[BUF_SIZE - 1] == '\0');
		}

		char* finance_database_description = strtok(buf, ": ");
		if (strcmp(finance_database_description, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index_count]) != 0)
		{
			char errmsg[256];
			snprintf(errmsg, 256, "The source type[%s] is NOT identical to %s in %", finance_database_description, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index_count], DATABASE_TIME_RANGE_CONF_FILENAME);
			throw runtime_error(string(errmsg));
		}
		source_type_index_count++;
		char* time_range_str =  strtok(NULL, ": ");
//		fprintf(stderr, "%s: %s\n", finance_database_description, time_range_str);

//		if (time_range_cfg == NULL)
//		{
//			char start_time_str[16];
//			char end_time_str[16];
//			sscanf(buf, "%s %s", start_time_str, end_time_str);
//			time_range_cfg = new TimeRangeCfg(start_time_str, end_time_str);
//			WRITE_FORMAT_DEBUG("Find the time range [%s %s] in %s", start_time_str, end_time_str, WORKDAY_CANLENDAR_CONF_FILENAME);
//			if (time_range_cfg == NULL)
//			{
//				WRITE_ERROR("Fail to allocate the memory: time_range_cfg");
//				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
//				goto OUT1;
//			}
//		}
//		else
//		{
//			int year;
//			sscanf(buf, "[%d]", &year);
//			PDAY_DEQUE day_deque_ptr = new DAY_DEQUE[12];
//			if (day_deque_ptr == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: day_list_ptr");
//				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
//				goto OUT1;
//			}
//			workday_year_sort_queue.push_back(year);
//
//			strtok(buf, "]");
//			char* year_workday_list_str = strtok(NULL, "]");
//			char* month_workday_list_str =  strtok(year_workday_list_str, ";");
//			list<char*> month_workday_list;
//			while (month_workday_list_str != NULL)
//			{
//				int month;
//				char month_tmp[4];
//				sscanf(month_workday_list_str, "%d:", &month);
//				snprintf(month_tmp, 4, "%d:", month);
//				int month_tmp_len = strlen(month_tmp);
//				char* month_workday_list_str_tmp = new char[strlen(month_workday_list_str) - month_tmp_len + 1];
//				if (month_workday_list_str_tmp == NULL)
//				{
//					WRITE_ERROR("Fail to allocate memory: month_workday_list_str_tmp");
//					ret = RET_FAILURE_INSUFFICIENT_MEMORY;
//					goto OUT1;
//				}
//				strcpy(month_workday_list_str_tmp, &month_workday_list_str[month_tmp_len]);
//				month_workday_list.push_back(month_workday_list_str_tmp);
//				if (month != (int)month_workday_list.size())
//				{
//					WRITE_FORMAT_ERROR("Incorrect month, expected: %d, actual: %d", month, (int)month_workday_list.size());
//					ret = RET_FAILURE_INCORRECT_CONFIG;
//					goto OUT1;
//				}
//				month_workday_list_str =  strtok(NULL, ";");
//			}
//			list<char*>::iterator iter = month_workday_list.begin();
//			int month_count = 0;
//			while (iter != month_workday_list.end())
//			{
//				char* month_workday_list_str = (char*)*iter;
//				char* day_workday_str =  strtok(month_workday_list_str, ",");
//				while (day_workday_str != NULL)
//				{
//					day_deque_ptr[month_count].push_back(atoi(day_workday_str));
//					day_workday_str = strtok(NULL, ",");
//				}
//				delete month_workday_list_str;
//				iter++;
//				month_count++;
//			}
//			workday_map[year] = day_deque_ptr;
//			month_workday_list.clear();
//		}
	}

	fclose(fp);
	fp = NULL;

	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
//	sort(workday_year_sort_queue.begin(), workday_year_sort_queue.end());
//	workday_year_sort_queue_size = workday_year_sort_queue.size();
	return ret;
}

int FinanceAnalyzerDatabaseTimeRange::add_ref()
{
	__sync_fetch_and_add(&ref_cnt, 1);
	return ref_cnt;
}

int FinanceAnalyzerDatabaseTimeRange::release()
{
	__sync_fetch_and_sub(&ref_cnt, 1);
	if (ref_cnt == 0)
	{
		delete this;
		return 0;
	}
	return ref_cnt;
}
