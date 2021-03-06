#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_statistics.h"
#include "finance_analyzer_math_formula_statistics.h"
#include "finance_analyzer_graph_table_statistics.h"
// #include "finance_analyzer_mgr.h"
// #include "finance_analyzer_algorithm.h"
// #include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_output.h"


using namespace std;

static const int RES_INFO_LENGTH = 1024;
static char res_info[RES_INFO_LENGTH];

FinanceAnalyzerStatistics::FinanceAnalyzerStatistics() :
	// finance_analyzer_sql_reader(NULL),
	finance_analyzer_math_formula_statistics(NULL),
	finance_analyzer_graph_table_statistics(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_WORKDAY_CANLENDAR()
	// IMPLEMENT_DATABASE_TIME_RANGE()
	res_info[0] = '\0';
}

FinanceAnalyzerStatistics::~FinanceAnalyzerStatistics()
{
	if (finance_analyzer_math_formula_statistics != NULL)
	{
		delete finance_analyzer_math_formula_statistics;
		finance_analyzer_math_formula_statistics = NULL;
	}
	if (finance_analyzer_graph_table_statistics != NULL)
	{
		delete finance_analyzer_graph_table_statistics;
		finance_analyzer_graph_table_statistics = NULL;
	}
	// if (finance_analyzer_sql_reader != NULL)
	// {
	// 	delete finance_analyzer_sql_reader;
	// 	finance_analyzer_sql_reader = NULL;
	// }
	// RELEASE_DATABASE_TIME_RANGE()
	RELEASE_WORKDAY_CANLENDAR()
	RELEASE_MSG_DUMPER()
}

// void FinanceAnalyzerStatistics::enable_res_info(bool enable)
// {
// 	need_res_info = enable;
// }

// const char* FinanceAnalyzerStatistics::get_last_res_info()const
// {
// 	return res_info;
// }

unsigned short FinanceAnalyzerStatistics::initialize(STRING_LIST& src_email_address_list)
{
	// finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
	// if (finance_analyzer_sql_reader == NULL)
	// {
	// 	WRITE_ERROR("Fail to allocate memory: finance_analyzer_sql_reader");
	// 	return RET_FAILURE_INSUFFICIENT_MEMORY; 
	// }
	finance_analyzer_math_formula_statistics = new FinanceAnalyzerMathFormulaStatistics(this);
	if (finance_analyzer_math_formula_statistics == NULL)
	{
		WRITE_ERROR("Fail to allocate memory: finance_analyzer_math_formula_statistics");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	finance_analyzer_graph_table_statistics = new FinanceAnalyzerGraphTableStatistics(this);
	if (finance_analyzer_graph_table_statistics == NULL)
	{
		WRITE_ERROR("Fail to allocate memory: finance_analyzer_graph_table_statistics");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	unsigned short ret = RET_SUCCESS;
	ret = copy_email_adress_list(email_address_list);
	if (CHECK_FAILURE(ret))
		return ret;
	return ret;
}

unsigned short FinanceAnalyzerStatistics::copy_email_adress_list(STRING_LIST& src_email_address_list)
{
	for (STRING_LIST::const_iterator iter = src_email_address_list.begin() ; iter != src_email_address_list.end() ; iter++)
	{
		string email_address = (string)*iter;
		email_address_list.push_back(email_address);
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerStatistics::show_result(string& result_str, int show_result_type, const char* data_description)const
{
	// assert(time_cfg != NULL && "time_cfg should NOT be NULL");
	unsigned short ret = RET_SUCCESS;

// Show result on the screen
	if (show_result_type & SHOW_RES_STDOUT)
	{
		if (!SHOW_CONSOLE)
		{
			WRITE_WARN("Disabled; No data will be shown on STDOUT/STDERR");
		}
		else
		{
// Write the data into STDOUT
			if (data_description != NULL)
			{
				string data_description_str = string(data_description) + string("\n");
				ret = direct_string_to_output_stream(data_description_str.c_str());
				if (CHECK_FAILURE(ret))
					return ret;
			}
			ret = direct_string_to_output_stream(result_str.c_str());
			if (CHECK_FAILURE(ret))
				return ret;
		}
	}
// Send the result by email
	if (show_result_type & SHOW_RES_EMAIL)
	{
		if (data_description == NULL)
		{
			static const char* DEF_EMAIL_TITLE = "Finance Statistics";
			data_description = DEF_EMAIL_TITLE;
		}
		for (STRING_LIST::const_iterator iter = email_address_list.begin() ; iter != email_address_list.end() ; iter++)
		{
			string email_address = (string)*iter;
			WRITE_FORMAT_DEBUG("Write daily data by email[%s] to %s", data_description, email_address.c_str());
			ret = send_email(data_description, email_address.c_str(), result_str.c_str());
			if (CHECK_FAILURE(ret))
				return ret;
		}
	}
// Write the data into file
	if (show_result_type & SHOW_RES_FILE)
	{
// Check the folder of keeping track of the result exist
		static bool check_result_folder_exist = false;
		if (!check_result_folder_exist)
		{
			ret = create_folder_in_project_if_not_exist(RESULT_FOLDER_NAME);
			if (CHECK_FAILURE(ret))
				return ret;
			check_result_folder_exist = true;
		}
		// char filename[32];
		// snprintf(filename, 32, DAILY_FINANCE_FILENAME_FORMAT, time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day());
		if (data_description == NULL)
		{
			static const char* DEF_FILENAME = "finance_statistics";
			data_description = DEF_FILENAME;
		}
		static char filepath[256];
		snprintf(filepath, 256, "%s/%s", RESULT_FOLDER_NAME, data_description);
		WRITE_FORMAT_DEBUG("Write daily data to file[%s]", filepath);
		ret = direct_string_to_output_stream(result_str.c_str(), filepath);
		if (CHECK_FAILURE(ret))
			return ret;
		PRINT("Check the result in file: %s\n", filepath);
	}
// Show result on syslog
	if (show_result_type & SHOW_RES_SYSLOG)
	{
// Write the data into syslog
		if (data_description != NULL)
			WRITE_FORMAT_INFO("*** %s ***\n%s", data_description, result_str.c_str());
		else
			WRITE_INFO(result_str.c_str());
	}

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerStatistics::get_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)
{
	unsigned short ret = RET_SUCCESS;
	SmartPointer<TimeRangeCfg> sp_modified_time_range_cfg;
	if (sp_time_range_cfg.get_const_instance() != NULL)
	{
		sp_modified_time_range_cfg.set_new((const PTIME_RANGE_CFG)sp_time_range_cfg.get_const_instance());
		WRITE_FORMAT_DEBUG("Check if it's required to check the statistics time range[%s]", sp_modified_time_range_cfg.get_instance()->to_string());
		if (sp_modified_time_range_cfg->get_start_time() != NULL)
		{
			if (!workday_canlendar->is_workday(sp_modified_time_range_cfg->get_start_time()))
			{
				SmartPointer<TimeCfg> sp_nearest_start_time_cfg;
				ret = workday_canlendar->get_next_workday(sp_modified_time_range_cfg->get_start_time(), sp_nearest_start_time_cfg);
				if (CHECK_FAILURE(ret))
				{
					WRITE_FORMAT_ERROR("Fail to get the nearest start workday from the start time: %s, due to: %s", sp_modified_time_range_cfg->get_start_time()->to_string(), get_ret_description(ret));
					return ret;
				}
				WRITE_FORMAT_DEBUG("The nearest start workday: %s", sp_nearest_start_time_cfg->to_string());
				sp_modified_time_range_cfg->reset_start_time(sp_nearest_start_time_cfg.get_instance());		
			}
		}
		else
		{
			SmartPointer<TimeCfg> sp_first_time_cfg;
			ret = workday_canlendar->get_first_workday(sp_first_time_cfg);
			if (CHECK_FAILURE(ret))
			{
				WRITE_FORMAT_ERROR("Fail to get the first workday, due to: %s", get_ret_description(ret));
				return ret;
			}
			WRITE_FORMAT_DEBUG("Set the end to first workday: %s", sp_first_time_cfg->to_string());
			sp_modified_time_range_cfg->reset_start_time(sp_first_time_cfg.get_instance());
		}
		if (sp_modified_time_range_cfg->get_end_time() != NULL)
		{
			if (!workday_canlendar->is_workday(sp_modified_time_range_cfg->get_end_time()))
			{
				SmartPointer<TimeCfg> sp_nearest_end_time_cfg;
				ret = workday_canlendar->get_prev_workday(sp_modified_time_range_cfg->get_end_time(), sp_nearest_end_time_cfg);
				if (CHECK_FAILURE(ret))
				{
					WRITE_FORMAT_ERROR("Fail to get the nearest end workday from the end time: %s, due to: %s", sp_modified_time_range_cfg->get_end_time()->to_string(), get_ret_description(ret));
					return ret;
				}
				WRITE_FORMAT_DEBUG("The nearest end workday: %s", sp_nearest_end_time_cfg->to_string());
				sp_modified_time_range_cfg->reset_end_time(sp_nearest_end_time_cfg.get_instance());
			}
		}
		else
		{
			SmartPointer<TimeCfg> sp_last_time_cfg;
			ret = workday_canlendar->get_last_workday(sp_last_time_cfg);
			if (CHECK_FAILURE(ret))
			{
				WRITE_FORMAT_ERROR("Fail to get the latest workday, due to: %s", get_ret_description(ret));
				return ret;
			}
			WRITE_FORMAT_DEBUG("Set the end to latest workday: %s", sp_last_time_cfg->to_string());
			sp_modified_time_range_cfg->reset_end_time(sp_last_time_cfg.get_instance());
		}
	}

	if (IS_FORMULA_STATISTICS_METHOD(statistics_method))
	{
		ret = finance_analyzer_math_formula_statistics->calculate_statistics(statistics_method, sp_modified_time_range_cfg);
	}
	else if (IS_TABLE_STATISTICS_METHOD(statistics_method))
	{
		ret = finance_analyzer_graph_table_statistics->fillout_statistics(statistics_method, sp_modified_time_range_cfg);
	}
	else if (IS_GRAPH_STATISTICS_METHOD(statistics_method))
	{
		ret = finance_analyzer_graph_table_statistics->plot_statistics(statistics_method, sp_modified_time_range_cfg);
	}
	else
	{
		WRITE_FORMAT_ERROR("Unknown Statistics Method: %d", statistics_method);
		ret = RET_FAILURE_INVALID_ARGUMENT;
	}
	return ret;
}