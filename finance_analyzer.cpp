#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <stdexcept>
#include "common.h"
#include "testcase_mgr.h"
#include "finance_analyzer_mgr.h"
#include "mgr_factory.h"
#include "interactive_server.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

#define REGISTER_CLASS(n, m) g_mgr_factory.register_class<n>(m)

// Paramters
static FinanceAnalysisMode param_mode = FinanceAnalysis_None;
static bool param_help = false;
static bool param_silent = false;
static const char* param_renew_company_profile_filepath = NULL;
static bool param_renew_company = false;
static const char* param_stock_support_resistance_filepath = NULL;
static const char* param_filter_stock_support_resistance_date = NULL;
static const char* param_filter_stock_support_resistance_volume = NULL;
static const char* param_find_stock_support_resistance = NULL;
static const char* param_find_stock_support_resistance_verbose = NULL;
static char* param_log_severity_name = NULL;
static char* param_syslog_severity_name = NULL;
static char* param_test_case = NULL;
static bool param_show_test_verbose = false;
static bool param_interactive_mode = false;
static bool param_daemonize = false;
static char* param_source = NULL;
static char* param_time_range = NULL;
static char* param_company = NULL;
static bool param_search = false;

static const int ERRMSG_SIZE = 256;
static char errmsg[ERRMSG_SIZE];
static const int MAX_STOCK_SUPPORT_RESISTANCE_AMOUNT = 32;
static FinanceAnalysisMode g_finance_analysis_mode = FinanceAnalysis_None;
static PIFINANCE_ANALYZER_MGR manager = NULL;
static PSEARCH_RULE_SET search_rule_set = NULL;
static bool show_stock_support_resistance_detail = false;
// static ResultSetMap result_set_map;
DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER();

static void signal_handler(int signo);

static void print_errmsg(const char* errmsg);
static void print_errmsg_and_exit(const char* errmsg);
static unsigned short parse_param(int argc, char** argv);
static unsigned short check_param();
static unsigned short setup_param();
static void show_usage_and_exit();
static void renew_company_and_exit(const char* source_company_profile_conf_folderpath);
static void run_test_cases_and_exit();
static void find_stock_support_resistance_and_exit(const char* company_number_close_price_pair_list, const char* price_support_and_resistance_root_folderpath, const char* price_support_resistance_time_filter, const char* price_support_resistance_volume_filter);
static void show_search_result_and_exit();
// static int parse_show_res_type(const char* show_res_type_string);
// static const char* get_statistics_method_description(StatisticsMethod statistics_method);
// static void daemonize();
static unsigned short init_interactive_server();

static void signal_handler(int signo)
{
	switch(signo)
	{
		case SIGTERM:
		{
			PRINT("SIGTERM Caught, the Finance Analyzer process[%d] is going to die......\n", getpid());
		}
		break;
		case SIGINT:
		{
			PRINT("SIGINT Caught, the Finance Analyzer process[%d] is going to die......\n", getpid());
		}
		break;
		default:
		{
			snprintf(errmsg, ERRMSG_SIZE,"UnExpected Signal[%d] Caught !!!", signo);
			print_errmsg_and_exit(errmsg);
		}
		break;
	}
	sleep(1);
	exit(EXIT_SUCCESS);
}

void show_usage_and_exit()
{
	PRINT("====================== Usage ======================\n");
// Finance Mode
	PRINT("--market_mode --stock_mode\n Description: Switch the market/stock mode\n Caution: Read parameters from %s when NOT set\n", MARKET_STOCK_SWITCH_CONF_FILENAME);
	PRINT("-h|--help\n Description: The usage\n Caution: Other flags are ignored\n");
	PRINT("--silent\n Description: Disable print log on console\n");
// Log/Syslog Severity
	PRINT("--log_severity\n Description: Set Log Severity\n");
	PRINT("--syslog_severity\n Description: Set Syslog Severity\n");
	int severity_size;
	const char** severity_name_list = MsgDumperWrapper::get_severity_name_list(severity_size);
	PRINT(" Severity List: ");
	for (int i = 0 ; i < severity_size ; i++)
		PRINT("%s ", severity_name_list[i]);
	PRINT("\n");
// Test cases
	PRINT("--test_case\nDescription: Run test case\nCaution: Exit when tests are done\n");
	PRINT("--show_test_verbose\nDescription: Show detailed steps while running test case\nCaution: Exit when tests are done\n");
	PRINT(" Test case list:\n");
	for (int i = 0 ; i < TestTypeSize ; i++)
		PRINT("  %s: %d\n", TEST_TYPE_DESCRIPTION[i], i);
	PRINT("  Format: Test Case Index/Index Range *Ex: 1;2-4;6\n");
// Interactive mode
	PRINT("-i|--interactive\n Description: Run the program in the interactive mode\n Caution: All flags except --daemonize are ignored\n");
// Method
	PRINT("-m|--method\nDescription: List of method\nDefault: all methods\n");
	int method_index_start, method_index_end;
	get_method_index_range(method_index_start, method_index_end, g_finance_analysis_mode);
	for (int i = method_index_start ; i < method_index_end ; i++)
		PRINT("  %s: %d\n", FINANCE_METHOD_DESCRIPTION_LIST[i], i);
	PRINT("  Format 1: All methods/fields (ex. all)\n");	
	PRINT("  Format 2: Method index/index range (ex. 1,2-4,6)\n");
	PRINT("  Format 3: Method index/index range with field index/index range  (ex. 1(1-2;4),2-4(2-4;5),5,6(1;3;5-7))\n");
// Time range
	PRINT("-t|--time_range\nDescription: Time range\nDefault: full time range\n");
	PRINT("  Format 1: Start time: (ex. 2015-01-01)\n");
	PRINT("  Format 2: Start time,End time: (ex. 2015-01-01,2015-09-04)\n");
	PRINT("  Format 3: ,End time: (ex. ,2015-09-04)\n");
// Company
	if (g_finance_analysis_mode == FinanceAnalysis_Stock)
	{
		PRINT("-c|--company\nDescription: List of company code number\nDefault: All company code nubmers\n");
		PRINT("  Format 1: Company code number (ex. 2347)\n");
		PRINT("  Format 2: Company code number range (ex. 2100-2200)\n");
		PRINT("  Format 3: Company group number (ex. [Gg]12)\n");
		PRINT("  Format 4: Company group number range (ex. [Gg]12-15)\n");
		PRINT("  Format 5: Company code number/number range/group/group range hybrid (ex. 2347,g3-5,G12,2362,g2,1500-1510)\n");
		PRINT("--renew_company\nDescription: Renew the table of the company profile\nCaution: Exit after renewing the company profile\n");
		PRINT("--renew_company_profile_filepath\nDescription: The company profile filepath for renewing the table of the company profile\nDefault: %s\n", DEFAULT_SOURCE_COMPANY_PROFILE_CONF_FOLDERPATH);
		PRINT("--stock_support_resistance_filepath\nDescription: Set the file path for finding the stock support and resistance of a specific company\nDefault: %s\n", DEFAULT_STOCK_SUPPORT_RESISTANCE_ROOT_FOLDERPATH);
		PRINT("--enable_stock_support_resistance_verbose\nDescription: Find the stock support and resistance of a specific company in detail\n");
		PRINT("  Format 1: Company code number:Stock close price Pair(ex. 1560:77.8)\n");
		PRINT("  Format 2: Company code number:Stock close price Pair List(ex. 1560:77.8,1589:81.9,1215:67)\nCaution: Max up to %d stock entry once\n", MAX_STOCK_SUPPORT_RESISTANCE_AMOUNT);
		PRINT("--filter_stock_support_resistance_date\nDescription: Filter the data which is eariler than a specific date\n");
		PRINT("  Format 1: Date(ex. 170801)");
		PRINT("--filter_stock_support_resistance_volume\nDescription: Filter the data whose volume is smaller than a specific value\n");
		PRINT("  Format 1: Value(ex. 5000)");
		PRINT("--find_stock_support_resistance\nDescription: Find the stock support and resistance of a specific company\n");
	}
// Search
	if (g_finance_analysis_mode == FinanceAnalysis_Market)
	{
		PRINT("--search\n Description: Search the database under the rule of source type and time range\n");
	}
	else if (g_finance_analysis_mode == FinanceAnalysis_Stock)
	{
		PRINT("--search\n Description: Search the database under the rule of source type, time range and company number\n");
	}
	PRINT("===================================================\n");
	exit(EXIT_SUCCESS);
}

void print_errmsg(const char* errmsg)
{
	assert(errmsg != NULL && "errmsg != NULL");
	WRITE_ERROR(errmsg);
	FPRINT(stderr, "%s\n", errmsg);
}

void print_errmsg_and_exit(const char* errmsg)
{
	print_errmsg(errmsg);
	exit(EXIT_FAILURE);
}

unsigned short parse_param(int argc, char** argv)
{
	int index = 1;
	int offset;
	for (; index < argc ; index += offset)
	{
        if (strcmp(argv[index], "--market_mode") == 0)
        {
        	if (param_mode != FinanceAnalysis_None)
        	{
        		PRINT("WARNING! The finance mode has already been set to %d\n", param_mode)
        	}
        	else
            	param_mode = FinanceAnalysis_Market;
            offset = 1;
        }
        else if (strcmp(argv[index], "--stock_mode") == 0)
        {
        	if (param_mode != FinanceAnalysis_None)
        	{
        		PRINT("WARNING! The finance mode has already been set to %d\n", param_mode)
        	}
        	else
            	param_mode = FinanceAnalysis_Stock;
            offset = 1;  
        }
		else if ((strcmp(argv[index], "--help") == 0) || (strcmp(argv[index], "-h") == 0))
		{
			param_help = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--renew_company_profile_filepath") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'renew_company_profile_filepath' parameter");
			param_renew_company_profile_filepath = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--renew_company") == 0)
		{
			param_renew_company = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--stock_support_resistance_filepath") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'stock_support_resistance_filepath' parameter");
			param_stock_support_resistance_filepath = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--filter_stock_support_resistance_date") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'filter_stock_support_resistance_date' parameter");
			param_filter_stock_support_resistance_date = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--filter_stock_support_resistance_volume") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'filter_stock_support_resistance_volume' parameter");
			param_filter_stock_support_resistance_volume = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--find_stock_support_resistance") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'find_stock_support_resistance' parameter");
			param_find_stock_support_resistance = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--find_stock_support_resistance_verbose") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'enable_stock_support_resistance_verbose' parameter");
			param_find_stock_support_resistance_verbose = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--silent") == 0)
		{
			// SHOW_CONSOLE = false;
			param_silent = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--log_severity") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'log_severity' parameter");
			param_log_severity_name = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--syslog_severity") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'syslog_severity' parameter");
			param_syslog_severity_name = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--test_case") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test_case' parameter");
			param_test_case = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--show_test_verbose") == 0)
		{
			param_show_test_verbose = true;
			offset = 1;
		}
		else if ((strcmp(argv[index], "--interactive_mode") == 0) || (strcmp(argv[index], "-i") == 0))
		{
			param_interactive_mode = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--daemonize") == 0)
		{
			param_daemonize = true;
			offset = 1;
		}
		else if ((strcmp(argv[index], "--source") == 0) || (strcmp(argv[index], "-s") == 0))
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'source' parameter");
			param_source = argv[index + 1];
			offset = 2;
		}
		else if ((strcmp(argv[index], "--time_range") == 0) || (strcmp(argv[index], "-t") == 0))
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'time_range' parameter");
			param_time_range = argv[index + 1];
			offset = 2;
		}
		else if ((strcmp(argv[index], "--company") == 0) || (strcmp(argv[index], "-c") == 0))
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'company' parameter");
			param_company = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--search") == 0)
		{
			param_search = true;
			offset = 1;
		}

		// else if (strcmp(argv[index], "--calculate_statistics") == 0)
		// {
		// 	if (statistics_method == StatisticsMethod_None)
		// 	{
		// 		if (index + 1 >= argc)
		// 			print_errmsg_and_exit("No argument found in 'calculate_statistics' parameter");
		// 		int method_number = atoi(argv[index + 1]) + StatisticsFormula_Start;
		// 		if (!IS_FORMULA_STATISTICS_METHOD(method_number))
		// 		{
		// 			snprintf(errmsg, ERRMSG_SIZE, "Unknown formula statistics method: %d", method_number);
		// 			print_errmsg_and_exit(errmsg);				
		// 		}
		// 		statistics_method = (StatisticsMethod)(atoi(argv[index + 1]) + StatisticsFormula_Start);
		// 	}
		// 	else
		// 	{
		// 		printf("The %s Statistics Method has already been selected. Ignore this...", get_statistics_method_description(statistics_method));
		// 	}
		// 	offset = 2;
		// }
		// else if (strcmp(argv[index], "--fillout_statistics") == 0)
		// {
		// 	if (statistics_method == StatisticsMethod_None)
		// 	{
		// 		if (index + 1 >= argc)
		// 			print_errmsg_and_exit("No argument found in 'fillout_statistics' parameter");
		// 		int method_number = atoi(argv[index + 1]) + StatisticsTable_Start;
		// 		if (!IS_TABLE_STATISTICS_METHOD(method_number))
		// 		{
		// 			snprintf(errmsg, ERRMSG_SIZE, "Unknown table statistics method: %d", method_number);
		// 			print_errmsg_and_exit(errmsg);					
		// 		}
		// 		statistics_method = (StatisticsMethod)(atoi(argv[index + 1]) + StatisticsFormula_Start);
		// 	}
		// 	else
		// 	{
		// 		printf("The %s Statistics Method has already been selected. Ignore this...", get_statistics_method_description(statistics_method));
		// 	}
		// 	offset = 2;
		// }
		// else if (strcmp(argv[index], "--plot_statistics") == 0)
		// {
		// 	if (statistics_method == StatisticsMethod_None)
		// 	{
		// 		if (index + 1 >= argc)
		// 			print_errmsg_and_exit("No argument found in 'plot_statistics' parameter");
		// 		int method_number = atoi(argv[index + 1]) + StatisticsGraph_Start;
		// 		if (!IS_GRAPH_STATISTICS_METHOD(method_number))
		// 		{
		// 			snprintf(errmsg, ERRMSG_SIZE, "Unknown formula statistics method: %d", method_number);
		// 			print_errmsg_and_exit(errmsg);			
		// 		}
		// 		statistics_method = (StatisticsMethod)(atoi(argv[index + 1]) + StatisticsGraph_Start);
		// 	}
		// 	else
		// 	{
		// 		printf("The %s Statistics Method has already been selected. Ignore this...", get_statistics_method_description(statistics_method));
		// 	}
		// 	offset = 2;
		// }
		// else if (strcmp(argv[index], "--statistics_time_range") == 0)
		// {
		// 	if (index + 1 >= argc)
		// 		print_errmsg_and_exit("No argument found in 'statistics_time_range' parameter");
		// 	char* statistics_time_range_tmp = new char[strlen(argv[index + 1]) + 1];
		// 	assert(statistics_time_range_tmp != NULL && "Fail to allocate memory:statistics_time_range_tmp");
		// 	memset(statistics_time_range_tmp, 0x0, sizeof(char) * strlen(argv[index + 1]) + 1);
		// 	memcpy(statistics_time_range_tmp, argv[index + 1], sizeof(char) * strlen(argv[index + 1]));
		// 	char* time_str_1 = strtok(statistics_time_range_tmp, ",");
		// 	char* time_str_2 = strtok(NULL, ",");
		// 	if (time_str_1 == NULL)
		// 	{
		// 		snprintf(errmsg, ERRMSG_SIZE, "Incorrect statistics time range format: %s", argv[index + 1]);
		// 		print_errmsg_and_exit(errmsg);
		// 	}
		// 	else
		// 	{
		// 		try
		// 		{
		// 			if (time_str_2 == NULL)
		// 			{
		// 				if (argv[index + 1][0] == ',')
		// 					sp_statistics_time_range_cfg.set_new(new TimeRangeCfg(NULL, time_str_1));
		// 				else
		// 					sp_statistics_time_range_cfg.set_new(new TimeRangeCfg(time_str_1, NULL));
		// 			}
		// 			else
		// 				sp_statistics_time_range_cfg.set_new(new TimeRangeCfg(time_str_1, time_str_2));
		// 		}
		// 		catch (const std::invalid_argument& e)
		// 		{
		// 			snprintf(errmsg, ERRMSG_SIZE, "Error occur while setting statistics time range, due to: %s", e.what());
		// 			print_errmsg_and_exit(errmsg);
		// 		}
		// 	}
		// 	// printf("statistics time range: %s\n", sp_statistics_time_range_cfg->to_string());
		// 	offset = 2;
		// }
		else
		{
			FPRINT(stderr, "Unknown parameter: %s\n", argv[index]);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}
	return RET_SUCCESS;
}

unsigned short check_param()
{
	static const int ERROR_MSG_SIZE = 256;
	static char error_msg[ERROR_MSG_SIZE];

	// if (param_daemonize)
	// {
	// 	if (!param_interactive_mode)
	// 	{
	// 		param_daemonize = false;
	// 		PRINT("WARNING: Daemonization must run in the interactive mode\n");
	// 	}
	// }
	if (param_log_severity_name != NULL)
	{
		try
		{
			MsgDumperWrapper::check_severity_name(param_log_severity_name);
		}
		catch (exception& e)
		{
			snprintf(error_msg, ERROR_MSG_SIZE, "Unknown log severity: %s", param_log_severity_name);
			print_errmsg_and_exit(error_msg);
		}
	}
	if (param_syslog_severity_name != NULL)
	{
		try
		{
			MsgDumperWrapper::check_severity_name(param_syslog_severity_name);
		}
		catch (exception& e)
		{
			snprintf(error_msg, ERROR_MSG_SIZE, "Unknown syslog severity: %s", param_syslog_severity_name);
			print_errmsg_and_exit(error_msg);
		}
	}
	if (param_source != NULL)
	{
		if (!param_interactive_mode)
		{
			param_source = NULL;
			PRINT("WARNING: the Source argument is ignored in the Interactive mode\n");
		}
	}
	if (param_time_range != NULL)
	{
		if (!param_interactive_mode)
		{
			param_time_range = NULL;
			PRINT("WARNING: the Time Range argument is ignored in the Interactive mode\n");
		}
	}
	if (param_mode != FinanceAnalysis_Stock)
	{
		if (param_company != NULL)
		{
			param_company = NULL;
			PRINT("WARNING: the Company argument is ignored in the Finance Stock mode\n");
		}
		if (param_renew_company)
		{
			param_renew_company = false;
			PRINT("WARNING: the Renew Company argument is ignored in the Finance Market mode\n");
		}
		if (param_renew_company_profile_filepath != NULL)
		{
			param_renew_company_profile_filepath = NULL;
			PRINT("WARNING: the Renew Company Profile Filepath argument is ignored in the Finance Market mode\n");
		}
		if (param_stock_support_resistance_filepath != NULL)
		{
			param_stock_support_resistance_filepath = NULL;
			PRINT("WARNING: the Stock Support Resistance Filepath argument is ignored in the Finance Market mode\n");
		}
		if (param_filter_stock_support_resistance_date != NULL)
		{
			param_filter_stock_support_resistance_date = NULL;
			PRINT("WARNING: the Filter Stock Support Resistance Date argument is ignored in the Finance Market mode\n");
		}
		if (param_filter_stock_support_resistance_volume != NULL)
		{
			param_filter_stock_support_resistance_volume = NULL;
			PRINT("WARNING: the Filter Stock Support Resistance Volume argument is ignored in the Finance Market mode\n");
		}	
		if (param_find_stock_support_resistance != NULL)
		{
			param_find_stock_support_resistance = NULL;
			PRINT("WARNING: the Find Stock Support Resistance argument is ignored in the Finance Market mode\n");
		}
		if (param_find_stock_support_resistance_verbose != NULL)
		{
			param_find_stock_support_resistance_verbose = NULL;
			PRINT("WARNING: the Find Stock Support Resistance Verbose argument is ignored in the Finance Market mode\n");
		}
	}
	else
	{
		if (param_company != NULL)
		{
			if (!param_interactive_mode)
			{
				param_company = NULL;
				PRINT("WARNING: the Company argument is ignored in the Interactive mode\n");
			}
		}
		if (param_renew_company)
		{
			if (param_renew_company_profile_filepath == NULL)
				param_renew_company_profile_filepath = DEFAULT_SOURCE_COMPANY_PROFILE_CONF_FOLDERPATH;
		}
		else
		{
			if (param_renew_company_profile_filepath != NULL)
			{
				param_renew_company_profile_filepath = NULL;
				PRINT("WARNING: the Renew Company Profile Filepath argument is ignored when the Renew Company argument is False\n");
			}
		}
		if (param_find_stock_support_resistance_verbose != NULL)
		{
			show_stock_support_resistance_detail = true;
			if (param_find_stock_support_resistance != NULL)
			{
				// param_find_stock_support_resistance = NULL;
				PRINT("WARNING: the Find Stock Support Resistance argument is ignored when the Find Stock Support Resistance Verbose argument is False\n");
			}
			param_find_stock_support_resistance = param_find_stock_support_resistance_verbose;
			param_find_stock_support_resistance_verbose = NULL;
		}
		if (param_find_stock_support_resistance != NULL)
		{
			if (param_stock_support_resistance_filepath == NULL)
				param_stock_support_resistance_filepath = DEFAULT_STOCK_SUPPORT_RESISTANCE_ROOT_FOLDERPATH;
		}
		else
		{
			if (param_stock_support_resistance_filepath != NULL)
			{
				param_stock_support_resistance_filepath = NULL;
				PRINT("WARNING: the Stock Support Resistance Filepath argument is ignored when the Find Stock Support Resistance argument is False\n");
			}
			if (param_filter_stock_support_resistance_date != NULL)
			{
				param_filter_stock_support_resistance_date = NULL;
				PRINT("WARNING: the Filter Stock Support Resistance Time argument is ignored when the Find Stock Support Resistance argument is False\n");
			}
			if (param_filter_stock_support_resistance_volume != NULL)
			{
				param_filter_stock_support_resistance_volume = NULL;
				PRINT("WARNING: the Filter Stock Support Resistance Volume argument is ignored when the Find Stock Support Resistance argument is False\n");
			}
		}
	}

	return RET_SUCCESS;
}

unsigned short setup_param()
{
	if (param_log_severity_name != NULL)
		STATIC_SET_LOG_SEVERITY_BY_NAME(param_log_severity_name);
	if (param_syslog_severity_name != NULL)
		STATIC_SET_SYSLOG_SEVERITY_BY_NAME(param_syslog_severity_name);
	unsigned short ret = RET_SUCCESS;
	if (!param_interactive_mode)
	{
		bool need_set_search_rule = false;
		if (param_source != NULL || param_time_range != NULL || param_company != NULL)
			need_set_search_rule = true;
// Initialize the search rule	
		if (need_set_search_rule)
		{
			search_rule_set = new SearchRuleSet(g_finance_analysis_mode);
			if (search_rule_set == NULL)
			{
				print_errmsg("Fail to allocate memory: search_rule_set");
				return RET_FAILURE_INSUFFICIENT_MEMORY;
			}		
		}
// Add the source type into the search rule
		if (param_source != NULL)
		{
			ret = search_rule_set->add_query_rule(param_source);
			if (CHECK_FAILURE(ret))
			{
				snprintf(errmsg, ERRMSG_SIZE, "SearchRuleSet::add_query_rule() fails, due to: %s", get_ret_description(ret));
				print_errmsg(errmsg);
				return ret;
			}
		}
// Add the time range into the search rule
		if (param_time_range != NULL)
		{
			ret = search_rule_set->add_time_rule(param_time_range);
			if (CHECK_FAILURE(ret))
			{
				snprintf(errmsg, ERRMSG_SIZE, "SearchRuleSet::add_time_rule() fails, due to: %s", get_ret_description(ret));
				print_errmsg(errmsg);
				return ret;
			}
		}
// Add the company into the search rule
		if (param_company != NULL)
		{
			ret = search_rule_set->add_company_rule(param_company);
			if (CHECK_FAILURE(ret))
			{
				snprintf(errmsg, ERRMSG_SIZE, "SearchRuleSet::add_company_rule() fails, due to: %s", get_ret_description(ret));
				print_errmsg(errmsg);
				return ret;
			}
		}
		if (need_set_search_rule)
		{
			ret = search_rule_set->add_rule_done();
			if (CHECK_FAILURE(ret))
			{
				snprintf(errmsg, ERRMSG_SIZE, "SearchRuleSet::add_rule_done() fails, due to: %s", get_ret_description(ret));
				print_errmsg(errmsg);
				return ret;
			}
		}
	}
	return RET_SUCCESS;
}

void run_test_cases_and_exit()
{
	assert(param_test_case != NULL && "param_test_case should NOT be NULL");
	TestCaseMgr testcase_mgr;
	testcase_mgr.set_show_detail(param_show_test_verbose);
	int cnt = 0;
	int pass_cnt = 0;
	if (strcmp(param_test_case, "all") == 0)
	{
		for (int i = 0 ; i < TestTypeSize ; i++)
		{
			cnt++;
			if (testcase_mgr.test((TestType)i))
				pass_cnt++;
		}
	}
	else
	{
		int test_case_list_size = strlen(param_test_case) + 1;
		char* test_case_list = new char[test_case_list_size];
		assert(test_case_list != NULL && "Fail to allocate memory: test_case_list");
		memcpy(test_case_list, param_test_case, sizeof(char) * test_case_list_size);
		char* test_case_no_str = strtok(test_case_list, ",");
		while (test_case_no_str != NULL)
		{
			int test_case_no = atoi(test_case_no_str);
			if (test_case_no < 0 || test_case_no >= TestTypeSize)
			{
				char errmsg[64];
				snprintf(errmsg, 64, "Unknown test case no: %d", test_case_no);
				throw invalid_argument(errmsg);
			}
			cnt++;
			if (testcase_mgr.test((TestType)test_case_no))
				pass_cnt++;
			test_case_no_str =  strtok(NULL, ",");
		}
		delete[] test_case_list;
	}
	PRINT("\n***Result***  Total Test Cases: %d, Pass: %d\n", cnt, pass_cnt);
	exit((cnt == pass_cnt) ? EXIT_SUCCESS : EXIT_FAILURE);
}

void show_search_result_and_exit()
{
	assert(search_rule_set != NULL && "search_rule_set is NOT NULL");
	assert(search_rule_set->is_add_rule_done() && "SearchRuleSet::add_done is NOT true");
	ResultSetMap result_set_map;
	unsigned short ret = manager->search(search_rule_set, &result_set_map);
	if (CHECK_SUCCESS(ret))
	{
		PRINT("\n***Result***\n%s\n", result_set_map.to_string().c_str());
	}
	else
	{
		snprintf(errmsg, ERRMSG_SIZE, "IFinanceAnalyzerMgr::search() fails, due to: %s", get_ret_description(ret));
		print_errmsg(errmsg);
	}
	exit(CHECK_SUCCESS(ret) ? EXIT_SUCCESS : EXIT_FAILURE);	
}

void renew_company_and_exit(const char* source_company_profile_conf_folderpath)
{
	// static const int BUF_SIZE = 256;
	// static char buf[BUF_SIZE];
	unsigned short ret = RET_SUCCESS;
	assert(source_company_profile_conf_folderpath != NULL && "source_company_profile_conf_folderpath should NOT be NULL");
	string timestamp_src;
	string timestamp_dst;
	ret = get_config_file_timestamp(timestamp_src, COMPANY_PROFILE_CONF_FILENAME, source_company_profile_conf_folderpath);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fails to get time stamp from srouce file[%s], due to: %s", COMPANY_PROFILE_CONF_FILENAME, get_ret_description(ret));
		print_errmsg_and_exit(errmsg);
	}
	ret = get_config_file_timestamp(timestamp_dst, COMPANY_PROFILE_CONF_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		if (!FAILURE_IS_NOT_FOUND(ret))
		{
			snprintf(errmsg, ERRMSG_SIZE, "Fails to get time stamp from destination file[%s], due to: %s", COMPANY_PROFILE_CONF_FILENAME, get_ret_description(ret));
			print_errmsg_and_exit(errmsg);
		}
	}
	bool check_equal = (timestamp_src == timestamp_dst ? true : false);
	if (check_equal)
	{
		PRINT("The time stamp is equal, NO NEED to renew......\n");
	}
	else
	{
		PRINT("Renew company profile: %s -> %s\n", timestamp_dst.c_str(), timestamp_src.c_str());
// Copy the company profile from the finance_scrapy_python project
		ret = copy_config_file(COMPANY_PROFILE_CONF_FILENAME, source_company_profile_conf_folderpath);
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, ERRMSG_SIZE, "Fails to copy the config file[%s] from: %s, due to: %s", COMPANY_PROFILE_CONF_FILENAME, source_company_profile_conf_folderpath, get_ret_description(ret));
			print_errmsg_and_exit(errmsg);
		}
	}
	exit(EXIT_SUCCESS);
}

void find_stock_support_resistance_and_exit(const char* company_number_close_price_pair_list, const char* price_support_and_resistance_root_folderpath, const char* price_support_resistance_time_filter, const char* price_support_resistance_volume_filter)
{
	unsigned short ret = RET_SUCCESS;
	assert(company_number_close_price_pair_list != NULL && "company_number_close_price_pair_list should NOT be NULL");
	assert(price_support_and_resistance_root_folderpath != NULL && "price_support_and_resistance_root_folderpath should NOT be NULL");
	int data_buf_size = strlen(company_number_close_price_pair_list) + 1;
	char* data_buf = new char[data_buf_size];
	if (data_buf == NULL)
		throw bad_alloc();
	memcpy(data_buf, company_number_close_price_pair_list, sizeof(char) * (data_buf_size + 1));

	char *data_buf_tmp = data_buf;
	int stock_support_resistance_count = 0;
	char* stock_support_resistance_entry_array[MAX_STOCK_SUPPORT_RESISTANCE_AMOUNT];
// Parse the stock entry
	char *data_buf_entry = NULL;
	while ((data_buf_entry = strtok(data_buf_tmp, ",")) != NULL)
	{
		if (stock_support_resistance_count == MAX_STOCK_SUPPORT_RESISTANCE_AMOUNT)
		{
			PRINT("The stock entry[%s] is IGNORED......\n", data_buf_entry);
		}
		else
		{
			stock_support_resistance_entry_array[stock_support_resistance_count++] = data_buf_entry;
		}
		if (data_buf_tmp != NULL)
			data_buf_tmp = NULL;
	}
// Calculate the support and resistance for each entry
	string price_support_and_resistance_result;
	for (int i = 0 ; i < stock_support_resistance_count ; i++)
	{
		char* stock_support_resistance_entry = stock_support_resistance_entry_array[i];
		// printf ("item: %s\n", stock_support_resistance_entry);
// Parse the data 
		char* comma_pos = strchr(stock_support_resistance_entry, ':');
		if (comma_pos == NULL)
		{
			snprintf(errmsg, ERRMSG_SIZE, "Incorrect argument for finding stock support and resistance: %s", stock_support_resistance_entry);
			print_errmsg_and_exit(errmsg);
		}
		int comma_pos_index = comma_pos - stock_support_resistance_entry;
		const int BUF_SIZE = 8;
		char company_number_buf[BUF_SIZE];
		char stock_close_price_buf[BUF_SIZE];
		memset(company_number_buf, 0x0, sizeof(char) * BUF_SIZE);
		memset(stock_close_price_buf, 0x0, sizeof(char) * BUF_SIZE);
		memcpy(company_number_buf, stock_support_resistance_entry, sizeof(char) * comma_pos_index);
		memcpy(stock_close_price_buf, &stock_support_resistance_entry[comma_pos_index + 1], sizeof(char) * (strlen(stock_support_resistance_entry) - (comma_pos_index + 1)));
		float stock_close_price = atof(stock_close_price_buf);
// Analyze data from each stock
		price_support_and_resistance_result += (string("==================") + string(company_number_buf) + string("==================\nClose Price: ") + string(stock_close_price_buf) + string("\n"));
		string price_support_resistance_string;
		ret = manager->get_stock_support_resistance_string(string(company_number_buf), stock_close_price, price_support_resistance_string, price_support_and_resistance_root_folderpath, show_stock_support_resistance_detail, price_support_resistance_time_filter, price_support_resistance_volume_filter);
		if (CHECK_FAILURE(ret))
		{
			if (FAILURE_IS_NOT_FOUND(ret))
			{
				price_support_and_resistance_result += "No Data\n\n";
				PRINT("WARNING: The %s support resistance file does NOT exist in %s\n", company_number_buf, price_support_and_resistance_root_folderpath);
			}
			else
			{
				snprintf(errmsg, ERRMSG_SIZE, "Fail to find the support and resistance of the company: %s, due to: %s", company_number_buf, get_ret_description(ret));
				print_errmsg_and_exit(errmsg);
			}
		}
		else
			price_support_and_resistance_result += (price_support_resistance_string + string("\n")) ;
	}
	if (data_buf != NULL)
	{
		delete[] data_buf;
		data_buf = NULL;
	}
	PRINT("%s", price_support_and_resistance_result.c_str());
	exit(EXIT_SUCCESS);
}

// const char* get_statistics_method_description(StatisticsMethod statistics_method)
// {
// 	if (IS_FORMULA_STATISTICS_METHOD(statistics_method))
// 		return "Statistics by Formula";
// 	else if (IS_TABLE_STATISTICS_METHOD(statistics_method))
// 		return "Statistics by Table";
// 	else if (IS_GRAPH_STATISTICS_METHOD(statistics_method))
// 		return "Statistics by Graph";
// 	assert ("Unknown Statistics Method");
// 	return NULL;
// }

// int parse_show_res_type(const char* show_res_type_string)
// {
// 	static const int SHOW_RES_TYPE[] = {SHOW_RES_STDOUT, SHOW_RES_EMAIL, SHOW_RES_FILE, SHOW_RES_SYSLOG, SHOW_RES_DEFAULT, SHOW_RES_ALL};
// 	assert(show_res_type_string != NULL && "show_res_type_string should NOT be NULL");
// 	int show_res_type_string_len = strlen(show_res_type_string);
// 	for (int i = 0 ; i < SHOW_RES_TYPE_SIZE ; i++)
// 	{
// 		if (strncmp(show_res_type_string, SHOW_RES_TYPE_DESCRIPTION[i], show_res_type_string_len) == 0)
// 			return SHOW_RES_TYPE[i];
// 	}
// 	char errmsg[256];
// 	snprintf(errmsg, 256, "Unknown show result type: %s", show_res_type_string);
// 	print_errmsg_and_exit(errmsg);
// 	return 0;
// }

// void daemonize() 
// { 
// // Step 1: Fork off the parent process
//  	pid_t pid = fork();
//   	if (pid < 0) exit(EXIT_FAILURE);
// 	if (pid != 0) exit (EXIT_SUCCESS);
// // Step 2: Create a unique session ID
//    	pid = setsid();
// 	if (pid < -1) exit(EXIT_FAILURE);
// // Step 3: Change the working directory
// 	chdir ("/"); 
// // Step 4: Close the standard file descriptors
// 	int fd = open ("/dev/null", O_RDWR, 0);
// 	if (fd != -1) 
// 	{
// 		dup2 (fd, STDIN_FILENO);
// 		dup2 (fd, STDOUT_FILENO);
// 		dup2 (fd, STDERR_FILENO);
// 		if (fd > 2) close (fd);
// 	}
// // Step 5: Change the file mode mask
// 	umask (0027);
// }

unsigned short init_interactive_server()
{
	// unsigned short ret = RET_SUCCESS;
	int server_fd, client_fd;
	sockaddr_in server_sock, client_sock;
	socklen_t client_sock_len;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		WRITE_FORMAT_ERROR("socket() fails, error code: %d", errno);
		return RET_FAILURE_SYSTEM_API;
	}
	int val = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));
	bzero((char*)&server_sock, sizeof(server_sock));
	server_sock.sin_family = AF_INET;
	server_sock.sin_addr.s_addr = INADDR_ANY;
	server_sock.sin_port = htons(INTERACTIVE_SERVER_PORT);
	if(bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0)
	{
		WRITE_FORMAT_ERROR("bind() fails, error code: %d", errno);
		return RET_FAILURE_SYSTEM_API;
	}
	if (listen(server_fd, INTERACTIVE_SERVER_BACKLOG) < 0)
	{
		WRITE_FORMAT_ERROR("listen() fails, error code: %d", errno);
		return RET_FAILURE_SYSTEM_API;
	}
	WRITE_DEBUG("Finance Analysis Server Ready, Wait for connection......");
	while (true)
	{
		client_sock_len = sizeof(client_sock);
		client_fd = accept(server_fd, (struct sockaddr *)&client_sock, &client_sock_len);
		WRITE_DEBUG("Connection request from......");
		if (client_fd == -1)
		{
			WRITE_FORMAT_ERROR("listen() fails, error code: %d", errno);
			return RET_FAILURE_SYSTEM_API;
		}
		write(client_fd, INTERACTIVE_PROMPT, strlen(INTERACTIVE_PROMPT));
	}

	return RET_SUCCESS;
}

#include <list>
#include <string>
using namespace std;

int main(int argc, char** argv)
{
	unsigned int line_count = 0;
	unsigned short ret_test = get_file_line_count(line_count, "/var/tmp/finance_sample/market/option_top3_legal_persons_buy_and_sell_option_open_interest.csv");
	printf("Line Count: %d\n", line_count);
	PTIME_RANGE_PARAM time_range_param = new TimeRangeParam("2016-01-07", "2016-01-11");
	list<string> line_list;
	read_file_lines_ex(line_list, "/var/tmp/finance_sample/market/option_top3_legal_persons_buy_and_sell_option_open_interest.csv", "r", time_range_param);
	list<string>::iterator iter = line_list.begin();
	while (iter != line_list.end())
	{
		string line = (string)*iter;
		printf("%s\n", line.c_str());
		iter++;
	}
	exit(0);
// Register the signals so that the process can exit gracefully
	struct sigaction sa;
	memset(&sa, 0x0, sizeof(sa));
	sa.sa_flags = 0;
	sa.sa_handler = &signal_handler;
	if (sigaction(SIGTERM, &sa, NULL) == -1)
		print_errmsg_and_exit("Fail to register the signal: SIGTERM");
	if (sigaction(SIGINT, &sa, NULL) == -1)
		print_errmsg_and_exit("Fail to register the signal: SIGINT");

// Register the manager class to manager factory
	MgrFactory g_mgr_factory;
	REGISTER_CLASS(FinanceAnalyzerMarketMgr, FinanceAnalysis_Market);
	REGISTER_CLASS(FinanceAnalyzerStockMgr, FinanceAnalysis_Stock);

	unsigned short ret = RET_SUCCESS;
// Parse the parameters.....
	ret = parse_param(argc, argv);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to parse parameters, due to: %s", get_ret_description(ret));
		goto FAIL;
	}
	if (param_silent)
		SHOW_CONSOLE = false;

// Check the parameters
    check_param();
    if (param_daemonize)
    {
		// daemonize();
		// sleep(100000);
// Caution: When the program run as a daemon, the STDIN/STDOUT/STDERR is redirected to /dev/null
// The functions related to STDIN/STDOUT/STDERR will have errors.		
// getchar(); Can't run this function when the program run a daemon
    }
    if (param_interactive_mode)
    {
    	// daemonize();
    	// ret = init_interactive_server();
// Setup the parameters
		ret = setup_param();
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, ERRMSG_SIZE, "Fail to setup parameters, due to: %s", get_ret_description(ret));
			goto FAIL;
		}
    	DECLARE_AND_IMPLEMENT_STATIC_INTERACTIVE_SERVER()
    }
    else
    {
// Determine the finance mode
	    if (param_mode == FinanceAnalysis_None)
	    {
	    	g_finance_analysis_mode = get_finance_analysis_mode_from_file();
	    }
	    else if (param_mode == FinanceAnalysis_Market)
	    {
	    	g_finance_analysis_mode = FinanceAnalysis_Market;
	    }
	    else if (param_mode == FinanceAnalysis_Stock)
	    {
	    	g_finance_analysis_mode = FinanceAnalysis_Stock;
	    }
	    else
	        throw runtime_error(string("Unknown mode !!!"));

	    if (param_help)
	    	show_usage_and_exit();
	    if (param_renew_company)
	    	renew_company_and_exit(param_renew_company_profile_filepath);
// Run the test cases
	    if (param_test_case != NULL)
	    {
	    	if (g_finance_analysis_mode != FinanceAnalysis_Market)
	    	{
		    	snprintf(errmsg, ERRMSG_SIZE, "%s", "Error!!! Can only run the cases in Market Mode");
				goto FAIL;
			}
	    	run_test_cases_and_exit();
	    }
// Create the instance of the manager class due to different mode
		manager = g_mgr_factory.get_instance(g_finance_analysis_mode);
// Initialize the manager class
		ret = manager->initialize();
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, ERRMSG_SIZE, "Fail to initialize manager class, due to: %s", get_ret_description(ret));
			goto FAIL;
		}
// Setup the parameters
		ret = setup_param();
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, ERRMSG_SIZE, "Fail to setup parameters, due to: %s", get_ret_description(ret));
			goto FAIL;
		}
		if (param_find_stock_support_resistance != NULL)
			find_stock_support_resistance_and_exit(param_find_stock_support_resistance, param_stock_support_resistance_filepath, param_filter_stock_support_resistance_date, param_filter_stock_support_resistance_volume);
		if (param_search)
			show_search_result_and_exit();
    }

	RELEASE_MSG_DUMPER();
	exit(EXIT_SUCCESS);
FAIL:
	RELEASE_MSG_DUMPER();
	print_errmsg_and_exit(errmsg);
}

void test()
{
		// FinanceCharDataPtrArray array;
	// char *my_string_array[] = {"Fuck", "Shit", "Damn"};
	// for (int i = 0 ; i < 3 ; i++)
	// 	array.add(my_string_array[i], strlen(my_string_array[i]) + 1);
	// cout << array << endl;
	// // for (int i = 0 ; i < 3 ; i++)
	// // 	cout << array[i] << endl;
	// exit(0);

	// cout << (is_market_mode() ? "Market" : "Stock") << endl;
	// // cout << strcmp("2362", "2361") << endl;
	// DECLARE_COMPANY_PROFILE()
	// IMPLEMENT_COMPANY_PROFILE()
	// // string company_code_number("2362");
	// // string res_listing_date = company_profile->lookup_company_listing_date(company_code_number);
	// // string res_group_name = company_profile->lookup_company_group_name(company_code_number);
	// // string res_group_number = company_profile->lookup_company_group_number(company_code_number);
	// // cout << res_listing_date << endl;
	// // cout << res_group_name << endl;
	// // cout << res_group_number << endl;
	// // company_profile->generate_company_profile_sorted_deque();
	// // company_profile->generate_company_group_profile_sorted_deque();

	// // FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->begin();
	// // while (iter != company_profile->end())
	// // {
	// // 	const PROFILE_ELEMENT_DEQUE& profile_element_deque = *iter;
	// // 	cout << profile_element_deque[0] << endl;
	// // 	++iter;
	// // }

	// // int company_group_size = company_profile->get_company_group_size();
	// // for (int i = 0 ; i < company_group_size ; i++)
	// // {
	// // 	cout << "****************** " << i << " : " << company_profile->get_company_group_description(i) << " ******************" << endl;
	// // 	FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->group_begin(i);
	// // 	while (iter != company_profile->group_end(i))
	// // 	{
	// // 		const PROFILE_ELEMENT_DEQUE& profile_element_deque = *iter;
	// // 		cout << profile_element_deque[0] << endl;
	// // 		++iter;
	// // 	}
	// // }
	// // FinanceAnalyzerCompanyProfile::show_traverse_result(company_profile, false);
	// company_profile->get_company_number_list_in_group(1);
	// company_profile->get_company_number_list_in_group(2);
	// // company_profile->get_company_number_list_in_group(1);
	// RELEASE_COMPANY_PROFILE()

	// getchar();
	// exit(EXIT_SUCCESS);

    // wchar_t myString2[16] = { 0x55E8, 0x3001, 0x4F60, 0x597D};

    // setlocale(LC_ALL, "");
    // wprintf(L"嗨、你好\n");
    // wprintf(L"嗨好\n");
    // printf("1111\n");
    // printf("%s\n", FINANCE_METHOD_DESCRIPTION_LIST[0]);
    // return 0; 
}
