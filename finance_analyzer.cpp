#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_test.h"
//#include "finance_analyzer_output.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

using namespace std;

void show_usage();
void print_errmsg_and_exit(const char* errmsg);
void run_test(const char* test_case_list, bool show_detail);
int parse_show_res_type(const char* show_res_type_string);

//#define DO_TEST_DEBUG

int main(int argc, char** argv)
{
#ifdef DO_TEST_DEBUG
	run_test("0", true);
	exit(0);
#endif

	static const int ERRMSG_SIZE = 256;

	char errmsg[ERRMSG_SIZE];
	int index = 1;
	int offset;
	bool update_daily = false;
	int show_update_daily_res_type = 0x0;
	bool analyze_daily = false;
	int show_analyze_daily_res_type =  0x0; //SHOW_RES_DEFAULT;
	bool output_daily = false;

	for (; index < argc ; index += offset)
	{
		if (strcmp(argv[index], "--help") == 0)
		{
			show_usage();
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[index], "--test") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test' attribute");
			run_test(argv[index + 1], false);
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[index], "--test_verbose") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'test_verbose' attribute");
			run_test(argv[index + 1], true);
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[index], "--update_daily") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'update_daily' attribute");
			update_daily = true;
			show_update_daily_res_type = parse_show_res_type(argv[index + 1]);
			offset = 2;
		}
		else if (strcmp(argv[index], "--analyze_daily") == 0)
		{
			if (index + 1 >= argc)
				print_errmsg_and_exit("No argument found in 'analyze_daily' attribute");
			analyze_daily = true;
			show_analyze_daily_res_type = parse_show_res_type(argv[index + 1]);
			offset = 2;
		}
		else if (strcmp(argv[index], "--output_daily") == 0)
		{
			output_daily = true;
			offset = 1;
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
	unsigned short ret = finance_analyzer_mgr.initialize();
	if (CHECK_FAILURE(ret))
		return ret;
	if (update_daily)
	{
		PRINT("Update daily data......\n");
		ret = finance_analyzer_mgr.update_daily(show_update_daily_res_type);
		if (CHECK_FAILURE(ret))
			snprintf(errmsg, ERRMSG_SIZE, "Fails to update daily, due to: %d, %s", ret, get_ret_description(ret));
	}
	if (analyze_daily)
	{
		PRINT("Analyze daily data......\n");
		ret = finance_analyzer_mgr.analyze_daily(show_analyze_daily_res_type, 1);
		if (CHECK_FAILURE(ret))
			snprintf(errmsg, ERRMSG_SIZE, "Fails to analyze daily, due to: %d, %s", ret, get_ret_description(ret));
	}
	if (output_daily)
	{
		PRINT("Output daily data......\n");
		ret = finance_analyzer_mgr.output_daily(1);
		if (CHECK_FAILURE(ret))
			snprintf(errmsg, ERRMSG_SIZE, "Fails to output daily, due to: %d, %s", ret, get_ret_description(ret));
	}

	exit(EXIT_SUCCESS);
}

void show_usage()
{
	PRINT("====================== Usage ======================\n");
	PRINT("-h|--help\nDescription: The usage\nCaution: Other flags are ignored\n");
	PRINT("-test\nDescription: Run test case\nCaution: Other flags are ignored\n");
	PRINT("-test_verbose\nDescription: Run test case and show detailed steps\nCaution: Other flags are ignored\n");
	PRINT(" Test case list: ");
	for (int i = 0 ; i < TestSize ; i++)
		PRINT("%s[%d] ", TEST_TYPE_DESCRIPTION[i], i);
	PRINT("\n");
	PRINT("  Format: 1,2,4 (Start from 0)\n");
	PRINT("  all: All types\n");
	PRINT("--update_daily\nDescription: Update daily data\n");
	PRINT("--analyze_daily\nDescription: Analyze daily data\n");
	PRINT("--output_daily\nDescription: Output daily data\n");
	PRINT(" Show Result Type list: ");
	for (int i = 0 ; i < SHOW_RES_TYPE_SIZE ; i++)
		PRINT("%s[%d] ", SHOW_RES_TYPE_DESCRIPTION[i], i);
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

void run_test(const char* test_case_list, bool show_detail)
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
	printf("\n***Statistics***  Total Test Cases: %d, Pass: %d\n", cnt, pass_cnt);
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
