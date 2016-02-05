#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_test.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

using namespace std;

void show_usage();
void run_test(const char* test_case_list, bool show_detail);

int main(int argc, char** argv)
{
	int index = 1;
	int offset;
	bool run_daily = false;
	bool analyze_daily = true;

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
			{
				fprintf(stderr, "Select the test case first\n");
			}
			run_test(argv[index + 1], false);
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[index], "--test_verbose") == 0)
		{
			if (index + 1 >= argc)
			{
				fprintf(stderr, "Select the test case first\n");
			}
			run_test(argv[index + 1], true);
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[index], "--run_daily") == 0)
		{
			run_daily = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--analyze_daily") == 0)
		{
			analyze_daily = true;
			offset = 1;
		}
		else if (strcmp(argv[index], "--show_console") == 0)
		{
			SHOW_CONSOLE = true;
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
	if (run_daily)
	{
		printf("Run daily data......\n");
		ret = finance_analyzer_mgr.run_daily();
		if (CHECK_FAILURE(ret))
		{
			fprintf(stderr, "Fails to run daily, due to: %d, %s\n", ret, get_ret_description(ret));
			goto FAIL;
		}
	}
	if (analyze_daily)
	{
		printf("Analyze daily data......\n");
		ret = finance_analyzer_mgr.analyze_daily();
		if (CHECK_FAILURE(ret))
		{
			fprintf(stderr, "Fails to analyze daily, due to: %d, %s\n", ret, get_ret_description(ret));
			goto FAIL;
		}
	}

	exit(EXIT_SUCCESS);
FAIL:
	exit(EXIT_FAILURE);
}

void show_usage()
{
	printf("====================== Usage ======================\n");
	printf("-h|--help\nDescription: The usage\nCaution: Other flags are ignored\n");
	printf("-test\nDescription: Run test case\nCaution: Other flags are ignored\n");
	printf("-test_verbose\nDescription: Run test case and show detailed steps\nCaution: Other flags are ignored\n");
	printf(" Test case list: ");
	for (int i = 0 ; i < TestSize ; i++)
		printf("%s[%d] ", TEST_TYPE_DESCRIPTION[i], i);
	printf("\n");
	printf("  Format: 1,2,4 (Start from 0)\n");
	printf("  all: All types\n");
	printf("--run_daily\nDescription: Run daily data\nCaution: Other flags are ignored\n");
	printf("--analyze_daily\nDescription: Analyze daily data\nCaution: Other flags are ignored\n");
	printf("--show_console\nDescription: Print the runtime info on STDOUT/STDERR\n");
	printf("===================================================\n");
}

void run_test(const char* test_case_list, bool show_detail)
{
	assert(test_case_list != NULL && "test_case_list should NOT be NULL");

	FinanceAnalyzerTest finance_analyzer_test;
	finance_analyzer_test.set_show_detail(show_detail);
	if (strcmp(test_case_list, "all") == 0)
	{
		for (int i = 0 ; i < TestSize ; i++)
		{
			finance_analyzer_test.test((TestType)i);
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
			finance_analyzer_test.test((TestType)test_case_no);
			test_case_no_str =  strtok(NULL, ",");
		}
		delete[] test_case_list_tmp;
	}
}
