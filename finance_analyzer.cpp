#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include "finance_analyzer_common.h"
#include "finance_analyzer_test.h"
#include "finance_analyzer_market_mgr.h"
#include "finance_analyzer_stock_mgr.h"
#include "finance_analyzer_mgr_inf.h"
#include "finance_analyzer_mgr_factory.h"
#include "finance_analyzer_interactive_server.h"


using namespace std;

#define REGISTER_CLASS(n, m) g_mgr_factory.register_class<n>(m)

// Paramters
static FinanceAnalysisMode param_mode = FinanceAnalysis_None;
static bool param_help = false;
static bool param_silent = false;
static char* param_test = NULL;
static bool param_test_verbose = false;
static bool param_interactive_mode = false;
static bool param_daemonize = false;

static const int BUF_SIZE = 256;
static char errmsg[BUF_SIZE];
static PIFINANCE_ANALYZER_MGR finance_analyzer_mgr = NULL;
DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER();
DECLARE_MSG_DUMPER_PARAM();

static void show_usage_and_exit();
static void print_errmsg_and_exit(const char* errmsg);
static unsigned short parse_param(int argc, char** argv);
static unsigned short check_param();
static unsigned short setup_param();
static void run_test_cases_and_exit(const char* test_case_list, bool show_detail);
static int parse_show_res_type(const char* show_res_type_string);
static const char* get_statistics_method_description(StatisticsMethod statistics_method);
// static void daemonize();
// static unsigned short init_interactive_server();

void show_usage_and_exit()
{
	PRINT("====================== Usage ======================\n");
	PRINT("--market_mode --stock_mode\n Description: Switch the market/stock mode\n Caution: Read parameters from %s when NOT set\n", MARKET_STOCK_SWITCH_CONF_FILENAME);
	PRINT("-h|--help\n Description: The usage\n Caution: Other flags are ignored\n");
	PRINT("--silent\n Description: Disable print log on console\n");
	PRINT("--test\nDescription: Run test case\nCaution: Other flags are ignored\n");
	PRINT("--test_verbose\nDescription: Run test case and show detailed steps\nCaution: Other flags are ignored\n");
	PRINT(" Test case list: ");
	int source_type_start_index, source_type_end_index;
	get_source_type_index_range(source_type_start_index, source_type_end_index);
	for (int i = source_type_start_index ; i < source_type_end_index ; i++)
		PRINT("%s[%d] ", TEST_TYPE_DESCRIPTION[i], i);
	PRINT("\n");
	PRINT("  Format: 1,2,4 (Start from 0)\n");
	PRINT("  all: All types\n");
	PRINT("-i|--interactive\n Description: Run the program in the interactive mode\n Caution: All flags except --daemonize are ignored\n");
	PRINT("--daemonize\n Description: Daemonize the process\n Caution: Must be in the interactive mode\n");
	// PRINT("--calculate_statistics\nDescription: Calculate the statistics by different analysis method\n");
	// for (int i = 0 ; i < FORMULA_STATSTICS_METHOD_SIZE ; i++)
	// 	PRINT("%s[%d] ", FORMULA_STATSTICS_METHOD_DESCRIPTION[i], i);
	// PRINT("\n");
	// PRINT("--fillout_statistics\nDescription: Fillout the statistics by different analysis method\n");
	// for (int i = 0 ; i < TABLE_STATSTICS_METHOD_SIZE ; i++)
	// 	PRINT("%s[%d] ", TABLE_STATSTICS_METHOD_DESCRIPTION[i], i);
	// PRINT("\n");
	// PRINT("--plot_statistics\nDescription: Plot the statistics by different analysis method\n");
	// for (int i = 0 ; i < GRAPH_STATSTICS_METHOD_SIZE ; i++)
	// 	PRINT("%s[%d] ", GRAPH_STATSTICS_METHOD_DESCRIPTION[i], i);
	// PRINT("\n");
	// PRINT("--statistics_time_range\nDescription: The time range of statistics result\n");
 //    PRINT("  Format 1 (start_time): 2015-01-01\n");
 //    PRINT("  Format 2 (start_time,end_time): 2015-01-01,2015-09-04\n");
 //    PRINT("  Format 3 (,end_time): ,2015-09-04\n");
	// PRINT("\n");
	PRINT("===================================================\n");
	exit(EXIT_SUCCESS);
}

void print_errmsg_and_exit(const char* errmsg)
{
	assert(errmsg != NULL && "errmsg != NULL");
	FPRINT(stderr, "%s\n", errmsg);
	WRITE_ERROR(errmsg);
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
		else if (strcmp(argv[index], "--test") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test' parameter");
			param_test = argv[index + 1];
			offset = 2;
		}
		else if (strcmp(argv[index], "--test_verbose") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test_verbose' parameter");
			param_test = argv[index + 1];
			param_test_verbose = true;
			offset = 2;
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
		// else if (strcmp(argv[index], "--calculate_statistics") == 0)
		// {
		// 	if (statistics_method == StatisticsMethod_None)
		// 	{
		// 		if (index + 1 >= argc)
		// 			print_errmsg_and_exit("No argument found in 'calculate_statistics' parameter");
		// 		int method_number = atoi(argv[index + 1]) + StatisticsFormula_Start;
		// 		if (!IS_FORMULA_STATISTICS_METHOD(method_number))
		// 		{
		// 			snprintf(errmsg, BUF_SIZE, "Unknown formula statistics method: %d", method_number);
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
		// 			snprintf(errmsg, BUF_SIZE, "Unknown table statistics method: %d", method_number);
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
		// 			snprintf(errmsg, BUF_SIZE, "Unknown formula statistics method: %d", method_number);
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
		// 		snprintf(errmsg, BUF_SIZE, "Incorrect statistics time range format: %s", argv[index + 1]);
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
		// 			snprintf(errmsg, BUF_SIZE, "Error occur while setting statistics time range, due to: %s", e.what());
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
	if (param_daemonize)
	{
		if (!param_interactive_mode)
		{
			param_daemonize = false;
			PRINT("WARNING: Daemonization must run in the interactive mode\n");
		}
	}
	return RET_SUCCESS;
}

unsigned short setup_param()
{
	return RET_SUCCESS;
}

void run_test_cases_and_exit(const char* test_case_list, bool show_detail)
{
	assert(test_case_list != NULL && "test_case_list should NOT be NULL");

	FinanceAnalyzerTest finance_analyzer_test;
	finance_analyzer_test.set_show_detail(show_detail);
	int cnt = 0;
	int pass_cnt = 0;
	if (strcmp(test_case_list, "all") == 0)
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
		char* test_case_list_tmp = new char[strlen(test_case_list) + 1];
		assert(test_case_list_tmp != NULL && "Fail to allocate memory: test_case_list_tmp");
		memset(test_case_list_tmp, 0x0, sizeof(char) * strlen(test_case_list) + 1);
		memcpy(test_case_list_tmp, test_case_list, sizeof(char) * strlen(test_case_list));
		char* test_case_no_str = strtok(test_case_list_tmp, ",");
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
		delete[] test_case_list_tmp;
	}
	PRINT("\n***Result***  Total Test Cases: %d, Pass: %d\n", cnt, pass_cnt);
	exit((cnt == pass_cnt) ? EXIT_SUCCESS : EXIT_FAILURE);
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

// unsigned short init_interactive_server()
// {
// 	// unsigned short ret = RET_SUCCESS;
// 	int server_fd, client_fd;
// 	sockaddr_in server_sock, client_sock;
// 	socklen_t client_sock_len;
// 	server_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd == -1)
// 	{
// 		WRITE_FORMAT_ERROR("socket() fails, error code: %d", errno);
// 		return RET_FAILURE_SYSTEM_API;
// 	}
// 	bzero((char*)&server_sock, sizeof(server_sock));
// 	server_sock.sin_family = AF_INET;
// 	server_sock.sin_addr.s_addr = INADDR_ANY;
// 	server_sock.sin_port = htons(INTERACTIVE_SERVER_PORT);
// 	if(bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0)
// 	{
// 		WRITE_FORMAT_ERROR("bind() fails, error code: %d", errno);
// 		return RET_FAILURE_SYSTEM_API;
// 	}
// 	if (listen(server_fd, INTERACTIVE_SERVER_BACKLOG) < 0)
// 	{
// 		WRITE_FORMAT_ERROR("listen() fails, error code: %d", errno);
// 		return RET_FAILURE_SYSTEM_API;
// 	}
// 	WRITE_DEBUG("Finance Analysis Server Ready, Wait for connection......");
// 	while (true)
// 	{
// 		client_sock_len = sizeof(client_sock);
// 		client_fd = accept(server_fd, (struct sockaddr *)&client_sock, &client_sock_len);
// 		WRITE_DEBUG("Connection request from......");
// 		if (client_fd == -1)
// 		{
// 			WRITE_FORMAT_ERROR("listen() fails, error code: %d", errno);
// 			return RET_FAILURE_SYSTEM_API;
// 		}
// 		write(client_fd, INTERACTIVE_PROMPT, strlen(INTERACTIVE_PROMPT));
// 	}

// 	return RET_SUCCESS;
// }

int main(int argc, char** argv)
{
// Register the manager class to manager factory
	FinanceAnalyzerMgrFactory g_mgr_factory;
	REGISTER_CLASS(FinanceAnalyzerMarketMgr, FinanceAnalysis_Market);
	REGISTER_CLASS(FinanceAnalyzerStockMgr, FinanceAnalysis_Stock);

	unsigned short ret = RET_SUCCESS;

// Parse the parameters.....
	ret = parse_param(argc, argv);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, BUF_SIZE, "Fail to parse parameters, due to: %s", get_ret_description(ret));
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
// The functions related to STDIN/STDOUT/STDERR will hav erros.		
// getchar(); Can't run this function when the program run a daemon
    }
    if (param_interactive_mode)
    {
    	// daemonize();
    	// ret = init_interactive_server();
    	DECLARE_AND_IMPLEMENT_STATIC_INTERACTIVE_SERVER()
    	// ret = interactive_server->wait_for_connection();
    	// if (CHECK_FAILURE(ret))
    	// {
    	// 	snprintf(errmsg, BUF_SIZE, "Fail to initialize the interactive server, error code: %d", ret);
     //   		goto FAIL;
     //   	}
    }
    else
    {
// Determine the finance mode
	    if (param_mode == FinanceAnalysis_None)
	    {
	    	finance_analysis_mode = get_finance_analysis_mode();
	        IS_FINANCE_MARKET_MODE = is_market_mode();
	        IS_FINANCE_STOCK_MODE = is_stock_mode();
	    }
	    else if (param_mode == FinanceAnalysis_Market)
	    {
	    	finance_analysis_mode = FinanceAnalysis_Market;
	        IS_FINANCE_MARKET_MODE = true;
	        IS_FINANCE_STOCK_MODE = false;
	    }
	    else if (param_mode == FinanceAnalysis_Stock)
	    {
	    	finance_analysis_mode = FinanceAnalysis_Stock;
	        IS_FINANCE_MARKET_MODE = false;
	        IS_FINANCE_STOCK_MODE = true;
	    }
	    else
	        throw runtime_error(string("Unknown mode !!!"));

	    if (param_help)
	    	show_usage_and_exit();

// Run the test cases
	    if (param_test != NULL)
	    {
	    	if (!IS_FINANCE_MARKET_MODE)
	    	{
		    	snprintf(errmsg, BUF_SIZE, "%s", "Error!!! Can only run the cases in Market Mode");
				goto FAIL;
			}
	    	run_test_cases_and_exit(param_test, param_test_verbose);
	    }
// Create the instance of the manager class due to different mode
		finance_analyzer_mgr = g_mgr_factory.get_instance(finance_analysis_mode);
// Initialize the manager class
		ret = finance_analyzer_mgr->initialize();
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, BUF_SIZE, "Fail to initialize manager class, due to: %s", get_ret_description(ret));
			goto FAIL;
		}

// Setup the parameters
		ret = setup_param();
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, BUF_SIZE, "Fail to setup parameters, due to: %s", get_ret_description(ret));
			goto FAIL;
		}
    }
    WRITE_DEBUG("Fuck3");

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