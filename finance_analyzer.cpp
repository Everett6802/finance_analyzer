#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include "finance_analyzer_common.h"
#include "finance_analyzer_test.h"
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_mgr_factory.h"
#include "finance_analyzer_interactive_server.h"

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
static char* param_log_level = NULL;
static char* param_syslog_level = NULL;
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
static FinanceAnalysisMode g_finance_analysis_mode = FinanceAnalysis_None;
static PIFINANCE_ANALYZER_MGR manager = NULL;
static PSEARCH_RULE_SET search_rule_set = NULL;
// static ResultSetMap result_set_map;
DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER();
DECLARE_MSG_DUMPER_PARAM();

static void show_usage_and_exit();
static void print_errmsg(const char* errmsg);
static void print_errmsg_and_exit(const char* errmsg);
static unsigned short parse_param(int argc, char** argv);
static unsigned short check_param();
static unsigned short setup_param();
static void run_test_cases_and_exit();
static void show_search_result_and_exit();
static int parse_show_res_type(const char* show_res_type_string);
static const char* get_statistics_method_description(StatisticsMethod statistics_method);
// static void daemonize();
static unsigned short init_interactive_server();

void show_usage_and_exit()
{
	PRINT("====================== Usage ======================\n");
// Finance Mode
	PRINT("--market_mode --stock_mode\n Description: Switch the market/stock mode\n Caution: Read parameters from %s when NOT set\n", MARKET_STOCK_SWITCH_CONF_FILENAME);
	PRINT("-h|--help\n Description: The usage\n Caution: Other flags are ignored\n");
	PRINT("--silent\n Description: Disable print log on console\n");
// Log/Syslog Severity
	PRINT("--log_level\n Set Log Severity Level\n");
	PRINT("--syslog_level\n Set Sysog Severity Level\n");
	PRINT(" Severity level list:\n");
	for (int i = 0 ; i < SEVERITY_NAME_SIZE ; i++)
		PRINT("  %s: %d\n", SEVERITY_NAME[i], i);
// Test cases
	PRINT("--test_case\nDescription: Run test case\nCaution: Exit when tests are done\n");
	PRINT("--show_test_verbose\nDescription: Show detailed steps while running test case\nCaution: Exit when tests are done\n");
	PRINT(" Test case list:\n");
	for (int i = 0 ; i < TestTypeSize ; i++)
		PRINT("  %s: %d\n", TEST_TYPE_DESCRIPTION[i], i);
	PRINT("  Format: Test Case Index/Index Range *Ex: 1;2-4;6\n");
// Interactive mode
	PRINT("-i|--interactive\n Description: Run the program in the interactive mode\n Caution: All flags except --daemonize are ignored\n");
// Source type
	PRINT("-s|--source\nDescription: List of source type\nDefault: all source types\n");
	int source_type_start_index, source_type_end_index;
	get_source_type_index_range(source_type_start_index, source_type_end_index, g_finance_analysis_mode);
	for (int i = source_type_start_index ; i < source_type_end_index ; i++)
		PRINT("  %s: %d\n", FINANCE_DATABASE_DESCRIPTION_LIST[i], i);
	PRINT("  Format 1: All source types/fields (ex. all)\n");	
	PRINT("  Format 2: Source type index/index range (ex. 1,2-4,6)\n");
	PRINT("  Format 3: Source type index/index range with field index/index range  (ex. 1(1-2;4),2-4(2-4;5),5,6(1;3;5-7))\n");
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
		else if (strcmp(argv[index], "--silent") == 0)
		{
			// SHOW_CONSOLE = false;
			param_silent = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--log_level") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'log_level' parameter");
			param_log_level = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--syslog_level") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'syslog_level' parameter");
			param_syslog_level = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--test_case") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test_case' parameter");
			param_test_case = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--test_show_test_verbose") == 0)
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
	if (param_daemonize)
	{
		if (!param_interactive_mode)
		{
			param_daemonize = false;
			PRINT("WARNING: Daemonization must run in the interactive mode\n");
		}
	}
	if (param_log_level != NULL)
	{
		unsigned short log_level = atoi(param_log_level);
		if (log_level < 0 || log_level >= SEVERITY_NAME_SIZE)
		{
			snprintf(error_msg, ERROR_MSG_SIZE, "Unknown log level: %s", param_log_level);
			print_errmsg_and_exit(error_msg);
		}
	}
	if (param_syslog_level != NULL)
	{
		unsigned short syslog_level = atoi(param_syslog_level);
		if (syslog_level < 0 || syslog_level >= SEVERITY_NAME_SIZE)
		{
			snprintf(error_msg, ERROR_MSG_SIZE, "Unknown syslog level: %s", param_syslog_level);
			print_errmsg_and_exit(error_msg);
		}
	}
	if (param_company != NULL)
	{
		if (param_mode != FinanceAnalysis_Stock)
		{
			param_company = NULL;
			PRINT("WARNING: the Company argument is ignored in the Finance Stock mode\n");
		}
	}
	return RET_SUCCESS;
}

unsigned short setup_param()
{
	if (param_log_level != NULL)
		STATIC_SET_LOG_SEVERITY(atoi(param_log_level));
	if (param_syslog_level != NULL)
		STATIC_SET_SYSLOG_SEVERITY(atoi(param_syslog_level));
	unsigned short ret = RET_SUCCESS;
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
	return RET_SUCCESS;
}

void run_test_cases_and_exit()
{
	assert(param_test_case != NULL && "param_test_case should NOT be NULL");
	FinanceAnalyzerTest finance_analyzer_test;
	finance_analyzer_test.set_show_detail(param_show_test_verbose);
	int cnt = 0;
	int pass_cnt = 0;
	if (strcmp(param_test_case, "all") == 0)
	{
		for (int i = 0 ; i < TestSize ; i++)
		{
			cnt++;
			if (finance_analyzer_test.test((TestType)i))
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
			if (test_case_no < 0 || test_case_no >= TestSize)
			{
				char errmsg[64];
				snprintf(errmsg, 64, "Unknown test case no: %d", test_case_no);
				throw invalid_argument(errmsg);
			}
			cnt++;
			if (finance_analyzer_test.test((TestType)test_case_no))
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

const char* get_statistics_method_description(StatisticsMethod statistics_method)
{
	if (IS_FORMULA_STATISTICS_METHOD(statistics_method))
		return "Statistics by Formula";
	else if (IS_TABLE_STATISTICS_METHOD(statistics_method))
		return "Statistics by Table";
	else if (IS_GRAPH_STATISTICS_METHOD(statistics_method))
		return "Statistics by Graph";
	assert ("Unknown Statistics Method");
	return NULL;
}

int parse_show_res_type(const char* show_res_type_string)
{
	static const int SHOW_RES_TYPE[] = {SHOW_RES_STDOUT, SHOW_RES_EMAIL, SHOW_RES_FILE, SHOW_RES_SYSLOG, SHOW_RES_DEFAULT, SHOW_RES_ALL};
	assert(show_res_type_string != NULL && "show_res_type_string should NOT be NULL");
	int show_res_type_string_len = strlen(show_res_type_string);
	for (int i = 0 ; i < SHOW_RES_TYPE_SIZE ; i++)
	{
		if (strncmp(show_res_type_string, SHOW_RES_TYPE_DESCRIPTION[i], show_res_type_string_len) == 0)
			return SHOW_RES_TYPE[i];
	}
	char errmsg[256];
	snprintf(errmsg, 256, "Unknown show result type: %s", show_res_type_string);
	print_errmsg_and_exit(errmsg);
	return 0;
}

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


int main(int argc, char** argv)
{
	// STATIC_SET_LOG_SEVERITY_CONFIG(3);
	// STATIC_SET_SYSLOG_SEVERITY_CONFIG(3);
	// STATIC_WRITE_DEBUG("Fuck DEBUG");
	// STATIC_WRITE_INFO("Fuck INFO");
	// STATIC_WRITE_WARN("Fuck WARN");
	// STATIC_WRITE_ERROR("Fuck ERROR");

	// static const int FILE_PATH_SIZE = 256;
	// char current_working_directory[FILE_PATH_SIZE];
	// getcwd(current_working_directory, FILE_PATH_SIZE);
	// char conf_filepath[FILE_PATH_SIZE];
	// snprintf(conf_filepath, FILE_PATH_SIZE, "%s/%s", current_working_directory, "conf/dumper_param.conf");
	// FILE* fp_set = popen("sed -i 's/^Log=[A-Z].*/Log=DEBUG/g' ./conf/dumper_param.conf", "w");
	// pclose(fp_set);

	// FILE* fp_get = popen("cat conf/dumper_param.conf | grep 'Log='", "r");
	// static const int LINE_SIZE = 64;
	// char line[LINE_SIZE], level[LINE_SIZE];
	// fgets(line, LINE_SIZE, fp_get);
	// printf("Line: %s\n", line);
	// sscanf(line, "Log=%s", level);
	// printf("Level: %s\n", level);
	// pclose(fp_get);

	// printf("Fuck1\n");
 //    daemonize();
 //    init_interactive_server();
 //    printf("Fuck\n");
 //    exit(EXIT_SUCCESS);

	// OutputMailStreamCfg output_mail_stream_cfg;
	// output_mail_stream_cfg.address_list.push_back(string("Everett6802@hotmail.com"));
	// output_mail_stream_cfg.title = string("Decorator Test");
	// POUTPUT_STREAM oms = new OutputMailStream(&output_mail_stream_cfg);

	// OutputFileStreamCfg output_file_stream_cfg;
	// output_file_stream_cfg.file_path = string("/home/super/test.log");
	// output_file_stream_cfg.file_attribute = string("a+");
	// POUTPUT_STREAM ofs = new OutputFileStream(&output_file_stream_cfg, oms);

	// POUTPUT_STREAM omos = new OutputMonitorStream(ofs);

	// omos->output("Fuck You !\nGo to Hell !!!\n");
	// delete omos;
	// exit(EXIT_SUCCESS);

// Register the manager class to manager factory
	FinanceAnalyzerMgrFactory g_mgr_factory;
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
	    // IS_FINANCE_MARKET_MODE = (g_finance_analysis_mode == FinanceAnalysis_Market ? true : false);
	    // IS_FINANCE_STOCK_MODE = (g_finance_analysis_mode == FinanceAnalysis_Stock ? true : false);
	    if (param_help)
	    	show_usage_and_exit();
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
		if (param_search)
			show_search_result_and_exit();
// Let's do something
		// STATIC_WRITE_DEBUG("Fuck DEBUG");
		// STATIC_WRITE_INFO("Fuck INFO");
		// STATIC_WRITE_WARN("Fuck WARN");
		// STATIC_WRITE_ERROR("Fuck ERROR");
    }
    sleep(2);

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
    // printf("%s\n", FINANCE_DATABASE_DESCRIPTION_LIST[0]);
    // return 0; 
}