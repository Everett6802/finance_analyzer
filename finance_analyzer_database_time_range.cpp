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
	ref_cnt(0),
	max_time_range_cfg(NULL)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerDatabaseTimeRange::~FinanceAnalyzerDatabaseTimeRange()
{
	int database_time_range_deque_size = database_time_range_deque.size();
	for (int i = 0 ; i < database_time_range_deque_size ; i++)
		delete database_time_range_deque[i];
	database_time_range_deque.clear();

	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerDatabaseTimeRange::initialize()
{
	// static const int FILE_PATH_SIZE = 256;
	// static char file_path[FILE_PATH_SIZE];
	// char current_working_directory[FILE_PATH_SIZE];
	// getcwd(current_working_directory, FILE_PATH_SIZE);
	// snprintf(file_path, FILE_PATH_SIZE, "%s/%s/%s", current_working_directory, CONFIG_FOLDER_NAME, DATABASE_TIME_RANGE_CONF_FILENAME);
	CREATE_PROJECT_FILEPATH(file_path, CONFIG_FOLDER_NAME, DATABASE_TIME_RANGE_CONF_FILENAME)

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

	WRITE_FORMAT_DEBUG("Parse the config file: %s", DATABASE_TIME_RANGE_CONF_FILENAME);
	FILE* fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the config file: %s, due to: %s", DATABASE_TIME_RANGE_CONF_FILENAME, strerror(errno));
		free(buf);
		return RET_FAILURE_SYSTEM_API;
	}

	unsigned short ret = RET_SUCCESS;
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
// Check if the source type in the config file is in order
		char* finance_database_description = strtok(buf, ": ");
		if (strcmp(finance_database_description, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index_count]) != 0)
		{
			char errmsg[256];
			snprintf(errmsg, 256, "The source type[%s] is NOT identical to %s in %s", finance_database_description, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index_count], DATABASE_TIME_RANGE_CONF_FILENAME);
			throw runtime_error(string(errmsg));
		}
		source_type_index_count++;
		char* time_range_str =  strtok(NULL, " ");
//		fprintf(stderr, "%s: %s\n", finance_database_description, time_range_str);
// Find the start/end time string
		char* start_time_str = strtok(time_range_str, ":");
		char* end_time_str = strtok(NULL, ":");
//		fprintf(stderr, "%s: %s:%s\n", finance_database_description, start_time_str, end_time_str);
		PTIME_RANGE_CFG time_range_cfg = new TimeRangeCfg(start_time_str, end_time_str);
		if (time_range_cfg == NULL)
		{
			WRITE_ERROR("Fail to allocate the memory: time_range_cfg");
			ret = RET_FAILURE_INSUFFICIENT_MEMORY;
			break;
		}
		database_time_range_deque.push_back(time_range_cfg);
// Find the oldest start date and latest end date
		if (max_time_range_cfg == NULL)
		{
			max_time_range_cfg = new TimeRangeCfg(start_time_str, end_time_str);
			if (max_time_range_cfg == NULL)
			{
				WRITE_ERROR("Fail to allocate the memory: max_time_range_cfg");
				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
				break;
			}
		}
		else
		{
// Find the oldest start date
			if (*max_time_range_cfg->get_start_time() > start_time_str)
				max_time_range_cfg->reset_time(start_time_str, NULL);
// Find the latest end date
			if (*max_time_range_cfg->get_end_time() < end_time_str)
				max_time_range_cfg->reset_time(NULL, end_time_str);
		}
	}
	WRITE_FORMAT_DEBUG("Max database time range: %s", max_time_range_cfg->to_string());

	fclose(fp);
	fp = NULL;

	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}

	return ret;
}

unsigned short FinanceAnalyzerDatabaseTimeRange::get_max_database_time_range(SmartPointer<TimeRangeCfg>& sp_max_database_time_range_cfg)
{
	PTIME_RANGE_CFG max_database_time_range_cfg = new TimeRangeCfg(*max_time_range_cfg);
	if (max_database_time_range_cfg == NULL)
	{
		WRITE_ERROR("Fail to allocate the memory: max_database_time_range_cfg");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	sp_max_database_time_range_cfg.set_new(max_database_time_range_cfg);
	return RET_SUCCESS;
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

unsigned short FinanceAnalyzerDatabaseTimeRange::restrict_time_range(const set<int>& source_type_index_set, PTIME_RANGE_CFG time_range_cfg)
{
	assert (!source_type_index_set.empty() && "source_type_index_set should NOT be empty");
	if (time_range_cfg == NULL)
	{
		WRITE_ERROR("time_range_cfg should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
// Search for the max start time and min end time to make sure the MySQL data is NOT out of range
	set<int>::iterator iter = source_type_index_set.begin();
	int max_start_time_source_type_index = -1;
	int max_start_time_int_value;
	int min_end_time_source_type_index = -1;
	int min_end_time_int_value;
// Find the max start time and min end time in the current selection
	while (iter != source_type_index_set.end())
	{
		int time_int_value = TimeCfg::get_int_value(database_time_range_deque[*iter]->get_start_time());
		if (max_start_time_source_type_index == -1 || time_int_value > max_start_time_int_value)
		{
			max_start_time_source_type_index = *iter;
			max_start_time_int_value = time_int_value;
		}
		if (min_end_time_source_type_index == -1 || time_int_value < min_end_time_int_value)
		{
			min_end_time_source_type_index = *iter;
			min_end_time_int_value = time_int_value;
		}
		iter++;
	}
	WRITE_FORMAT_DEBUG("The available search time range:%s %s", database_time_range_deque[max_start_time_source_type_index]->get_start_time()->to_string(), database_time_range_deque[min_end_time_source_type_index]->get_end_time()->to_string());
// Check if the start/end time is out of the Mysql time range
	if (time_range_cfg->get_end_time() != NULL)
	{
		if (*time_range_cfg->get_end_time() < *database_time_range_deque[max_start_time_source_type_index]->get_start_time())
		{
			WRITE_FORMAT_ERROR("The end time[%s] is earlier than the MySQL start time[%s]", time_range_cfg->get_end_time()->to_string(), database_time_range_deque[max_start_time_source_type_index]->get_start_time()->to_string());
			return RET_FAILURE_OUT_OF_RANGE;
		}
	}
	if (time_range_cfg->get_start_time() != NULL)
	{
		if (*time_range_cfg->get_start_time() > *database_time_range_deque[max_start_time_source_type_index]->get_end_time())
		{
			WRITE_FORMAT_ERROR("The start time[%s] is later than the MySQL end time[%s]", time_range_cfg->get_start_time()->to_string(), database_time_range_deque[max_start_time_source_type_index]->get_end_time()->to_string());
			return RET_FAILURE_OUT_OF_RANGE;
		}
	}
// Check the start time boundary
	if (*database_time_range_deque[max_start_time_source_type_index]->get_start_time() > *time_range_cfg->get_start_time())
	{
		WRITE_FORMAT_WARN("Start search time out of range, restrict from %s to %s", time_range_cfg->get_start_time()->to_string(), database_time_range_deque[max_start_time_source_type_index]->get_start_time()->to_string());
// Caution: Don't modify the start time in this way below:
		 // *time_range_cfg->get_start_time() = *database_time_range_deque[max_start_time_source_type_index]->get_start_time();
		time_range_cfg->reset_start_time(database_time_range_deque[max_start_time_source_type_index]->get_start_time()->to_string());
	}
// Check the end time boundary
	if (*database_time_range_deque[min_end_time_source_type_index]->get_end_time() < *time_range_cfg->get_end_time())
	{
		WRITE_FORMAT_WARN("End search time out of range, restrict from %s to %s", time_range_cfg->get_end_time()->to_string(), database_time_range_deque[max_start_time_source_type_index]->get_end_time()->to_string());
// Caution: Don't modify the end time in this way below:
		// *time_range_cfg->get_end_time() = *database_time_range_deque[min_end_time_source_type_index]->get_end_time();
		time_range_cfg->reset_end_time(database_time_range_deque[min_end_time_source_type_index]->get_end_time()->to_string());
	}

	return RET_SUCCESS;
}
