#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_test.h"
//#include "finance_analyzer_output.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

using namespace std;

void show_usage();
void print_errmsg_and_exit(const char* errmsg);
bool run_test(const char* test_case_list, bool show_detail);
int parse_show_res_type(const char* show_res_type_string);
const char* get_statistics_method_description(StatisticsMethod statistics_method);


#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
// #include "finance_analyzer_company_profile.h"
#include <string>
#include <algorithm>

using namespace std;

class my_class
{
public:
	char* number;
	int number_int;
	my_class(char* num):number(num)
	{
		number_int = atoi(number);
		fprintf(stderr, "number: %d\n", number_int);
	}
	const char* to_string()const{return number;}	
};

bool compare_company_number(const my_class* number1, const my_class* number2)
{
	return (strcmp(number1->number, number2->number) < 0 ? true : false);
}

bool compare_company_number_int(const my_class* number1, const my_class* number2)
{
	fprintf(stderr, "number1: %d, number2: %d\n", number1->number_int, number2->number_int);
	if (number1->number_int < number2->number_int)
	{
		return false;
	}
	else 
	{
		return true;
	}
}

int main(int argc, char** argv)
{
	FinanceCharDataPtrArray array;
	char *my_string_array[] = {"Fuck", "Shit", "Damn"};
	for (int i = 0 ; i < 3 ; i++)
		array.add(my_string_array[i], strlen(my_string_array[i]) + 1);
	cout << array << endl;
	// for (int i = 0 ; i < 3 ; i++)
	// 	cout << array[i] << endl;
	exit(0);

	cout << (is_market_mode() ? "Market" : "Stock") << endl;
	// cout << strcmp("2362", "2361") << endl;
	DECLARE_COMPANY_PROFILE()
	IMPLEMENT_COMPANY_PROFILE()
	// string company_code_number("2362");
	// string res_listing_date = company_profile->lookup_company_listing_date(company_code_number);
	// string res_group_name = company_profile->lookup_company_group_name(company_code_number);
	// string res_group_number = company_profile->lookup_company_group_number(company_code_number);
	// cout << res_listing_date << endl;
	// cout << res_group_name << endl;
	// cout << res_group_number << endl;
	// company_profile->generate_company_profile_sorted_deque();
	// company_profile->generate_company_group_profile_sorted_deque();

	// FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->begin();
	// while (iter != company_profile->end())
	// {
	// 	const PROFILE_ELEMENT_DEQUE& profile_element_deque = *iter;
	// 	cout << profile_element_deque[0] << endl;
	// 	++iter;
	// }

	// int company_group_size = company_profile->get_company_group_size();
	// for (int i = 0 ; i < company_group_size ; i++)
	// {
	// 	cout << "****************** " << i << " : " << company_profile->get_company_group_description(i) << " ******************" << endl;
	// 	FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->group_begin(i);
	// 	while (iter != company_profile->group_end(i))
	// 	{
	// 		const PROFILE_ELEMENT_DEQUE& profile_element_deque = *iter;
	// 		cout << profile_element_deque[0] << endl;
	// 		++iter;
	// 	}
	// }
	// FinanceAnalyzerCompanyProfile::show_traverse_result(company_profile, false);
	company_profile->get_company_number_list_in_group(1);
	company_profile->get_company_number_list_in_group(2);
	// company_profile->get_company_number_list_in_group(1);
	RELEASE_COMPANY_PROFILE()

	getchar();
	exit(EXIT_SUCCESS);
	static const int BUF_SIZE = 256;
	static char errmsg[BUF_SIZE];
    // wchar_t myString2[16] = { 0x55E8, 0x3001, 0x4F60, 0x597D};

    // setlocale(LC_ALL, "");
    // wprintf(L"嗨、你好\n");
    // wprintf(L"嗨好\n");
    // printf("1111\n");
    // printf("%s\n", FINANCE_DATABASE_DESCRIPTION_LIST[0]);
    // return 0; 

	// static const int ERRMSG_SIZE = 256;

	// char errmsg[ERRMSG_SIZE];
	int index = 1;
	int offset;
	unsigned short ret = RET_SUCCESS;
	StatisticsMethod statistics_method = StatisticsMethod_None;
	SmartPointer<TimeRangeCfg> sp_statistics_time_range_cfg;
	// bool update_daily = false;
	// int show_update_daily_res_type = 0x0;
	// bool analyze_daily = false;
	// int show_analyze_daily_res_type =  0x0; //SHOW_RES_DEFAULT;
	// bool output_daily = false;

	int exit_code = EXIT_SUCCESS;
	for (; index < argc ; index += offset)
	{
		if (strcmp(argv[index], "--help") == 0)
		{
			show_usage();
			goto OUT;
		}
		else if (strcmp(argv[index], "--test") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test' attribute");
			exit_code = run_test(argv[index + 1], false) ? EXIT_SUCCESS : EXIT_FAILURE;
			goto OUT;
		}
		else if (strcmp(argv[index], "--test_verbose") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test_verbose' attribute");
			exit_code = run_test(argv[index + 1], true) ? EXIT_SUCCESS : EXIT_FAILURE;
			goto OUT;
		}
		else if (strcmp(argv[index], "--calculate_statistics") == 0)
		{
			if (statistics_method == StatisticsMethod_None)
			{
				if (index + 1 >= argc)
					print_errmsg_and_exit("No argument found in 'calculate_statistics' attribute");
				int method_number = atoi(argv[index + 1]) + StatisticsFormula_Start;
				if (!IS_FORMULA_STATISTICS_METHOD(method_number))
				{
					snprintf(errmsg, BUF_SIZE, "Unknown formula statistics method: %d", method_number);
					print_errmsg_and_exit(errmsg);				
				}
				statistics_method = (StatisticsMethod)(atoi(argv[index + 1]) + StatisticsFormula_Start);
			}
			else
			{
				printf("The %s Statistics Method has already been selected. Ignore this...", get_statistics_method_description(statistics_method));
			}
			offset = 2;
		}
		else if (strcmp(argv[index], "--fillout_statistics") == 0)
		{
			if (statistics_method == StatisticsMethod_None)
			{
				if (index + 1 >= argc)
					print_errmsg_and_exit("No argument found in 'fillout_statistics' attribute");
				int method_number = atoi(argv[index + 1]) + StatisticsTable_Start;
				if (!IS_TABLE_STATISTICS_METHOD(method_number))
				{
					snprintf(errmsg, BUF_SIZE, "Unknown table statistics method: %d", method_number);
					print_errmsg_and_exit(errmsg);					
				}
				statistics_method = (StatisticsMethod)(atoi(argv[index + 1]) + StatisticsFormula_Start);
			}
			else
			{
				printf("The %s Statistics Method has already been selected. Ignore this...", get_statistics_method_description(statistics_method));
			}
			offset = 2;
		}
		else if (strcmp(argv[index], "--plot_statistics") == 0)
		{
			if (statistics_method == StatisticsMethod_None)
			{
				if (index + 1 >= argc)
					print_errmsg_and_exit("No argument found in 'plot_statistics' attribute");
				int method_number = atoi(argv[index + 1]) + StatisticsGraph_Start;
				if (!IS_GRAPH_STATISTICS_METHOD(method_number))
				{
					snprintf(errmsg, BUF_SIZE, "Unknown formula statistics method: %d", method_number);
					print_errmsg_and_exit(errmsg);			
				}
				statistics_method = (StatisticsMethod)(atoi(argv[index + 1]) + StatisticsGraph_Start);
			}
			else
			{
				printf("The %s Statistics Method has already been selected. Ignore this...", get_statistics_method_description(statistics_method));
			}
			offset = 2;
		}
		else if (strcmp(argv[index], "--statistics_time_range") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'statistics_time_range' attribute");
			char* statistics_time_range_tmp = new char[strlen(argv[index + 1]) + 1];
			assert(statistics_time_range_tmp != NULL && "Fail to allocate memory:statistics_time_range_tmp");
			memset(statistics_time_range_tmp, 0x0, sizeof(char) * strlen(argv[index + 1]) + 1);
			memcpy(statistics_time_range_tmp, argv[index + 1], sizeof(char) * strlen(argv[index + 1]));
			char* time_str_1 = strtok(statistics_time_range_tmp, ",");
			char* time_str_2 = strtok(NULL, ",");
			if (time_str_1 == NULL)
			{
				snprintf(errmsg, BUF_SIZE, "Incorrect statistics time range format: %s", argv[index + 1]);
				print_errmsg_and_exit(errmsg);
			}
			else
			{
				try
				{
					if (time_str_2 == NULL)
					{
						if (argv[index + 1][0] == ',')
							sp_statistics_time_range_cfg.set_new(new TimeRangeCfg(NULL, time_str_1));
						else
							sp_statistics_time_range_cfg.set_new(new TimeRangeCfg(time_str_1, NULL));
					}
					else
						sp_statistics_time_range_cfg.set_new(new TimeRangeCfg(time_str_1, time_str_2));
				}
				catch (const std::invalid_argument& e)
				{
					snprintf(errmsg, BUF_SIZE, "Error occur while setting statistics time range, due to: %s", e.what());
					print_errmsg_and_exit(errmsg);
				}
			}
			// printf("statistics time range: %s\n", sp_statistics_time_range_cfg->to_string());
			offset = 2;
		}
		else if (strcmp(argv[index], "--disable_console") == 0)
		{
			SHOW_CONSOLE = false;
			offset = 1;
		}
		else
		{
			fprintf(stderr, "Unknown param: %s\n", argv[index]);
			exit(EXIT_FAILURE);
		}
	}

// Start to run
	ret = finance_analyzer_mgr.initialize();
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, BUF_SIZE, "Fail to initialize, due to: %s", get_ret_description(ret));
		goto OUT;
	}

	if (statistics_method != StatisticsMethod_None)
	{
		ret = finance_analyzer_mgr.get_statistics(statistics_method, sp_statistics_time_range_cfg);
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, BUF_SIZE, "Fail to get statistics, due to: %s", get_ret_description(ret));
			goto OUT;
		}	
	}

	exit(EXIT_SUCCESS);
OUT:
	print_errmsg_and_exit(errmsg);
}

void show_usage()
{
	PRINT("====================== Usage ======================\n");
	PRINT("-h|--help\nDescription: The usage\nCaution: Other flags are ignored\n");
	PRINT("--test\nDescription: Run test case\nCaution: Other flags are ignored\n");
	PRINT("--test_verbose\nDescription: Run test case and show detailed steps\nCaution: Other flags are ignored\n");
	PRINT(" Test case list: ");
	for (int i = 0 ; i < TestSize ; i++)
		PRINT("%s[%d] ", TEST_TYPE_DESCRIPTION[i], i);
	PRINT("\n");
	PRINT("  Format: 1,2,4 (Start from 0)\n");
	PRINT("  all: All types\n");
	PRINT("--calculate_statistics\nDescription: Calculate the statistics by different analysis method\n");
	for (int i = 0 ; i < FORMULA_STATSTICS_METHOD_SIZE ; i++)
		PRINT("%s[%d] ", FORMULA_STATSTICS_METHOD_DESCRIPTION[i], i);
	PRINT("\n");
	PRINT("--fillout_statistics\nDescription: Fillout the statistics by different analysis method\n");
	for (int i = 0 ; i < TABLE_STATSTICS_METHOD_SIZE ; i++)
		PRINT("%s[%d] ", TABLE_STATSTICS_METHOD_DESCRIPTION[i], i);
	PRINT("\n");
	PRINT("--plot_statistics\nDescription: Plot the statistics by different analysis method\n");
	for (int i = 0 ; i < GRAPH_STATSTICS_METHOD_SIZE ; i++)
		PRINT("%s[%d] ", GRAPH_STATSTICS_METHOD_DESCRIPTION[i], i);
	PRINT("\n");
	PRINT("--statistics_time_range\nDescription: The time range of statistics result\n");
    PRINT("  Format 1 (start_time): 2015-01-01\n");
    PRINT("  Format 2 (start_time,end_time): 2015-01-01,2015-09-04\n");
    PRINT("  Format 3 (,end_time): ,2015-09-04\n");
	PRINT("\n");
	PRINT("--disable_console\nDescription: Disable printing the runtime info on STDOUT/STDERR\n");
	PRINT("===================================================\n");
}

void print_errmsg_and_exit(const char* errmsg)
{
	assert(errmsg != NULL && "errmsg != NULL");
	fprintf(stderr, "%s\n", errmsg);
	exit(EXIT_FAILURE);
}

bool run_test(const char* test_case_list, bool show_detail)
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
	printf("\n***Result***  Total Test Cases: %d, Pass: %d\n", cnt, pass_cnt);
	return ((cnt == pass_cnt) ? true : false);
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
