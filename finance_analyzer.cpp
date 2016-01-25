#include <stdio.h>
#include <stdlib.h>
#include "msg_dumper_wrapper.h"
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
//#include "finance_analyzer_algorithm.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

using namespace std;

void show_usage();

int main(int argc, char** argv)
{
//	finance_analyzer_mgr.analyze_daily();
//	FinanceIntDataArray finance_data_array1, finance_data_array2;
//	finance_data_array1.add(1);
//	finance_data_array1.add(2);
//	finance_data_array1.add(3);
//	finance_data_array1.add(4);
//	finance_data_array1.add(5);
//	finance_data_array2.add(-1);
//	finance_data_array2.add(-2);
//	finance_data_array2.add(-3);
//	finance_data_array2.add(-4);
//	finance_data_array2.add(-5);
//	for (int i = 0 ; i < finance_data_array1.get_size() ; i++)
//		printf("%d ", finance_data_array1[i]);
//	printf("\n");
//	for (int i = 0 ; i < finance_data_array2.get_size() ; i++)
//		printf("%d ", finance_data_array2[i]);
//	printf("\n");
//	finance_data_array1 += finance_data_array2;
//	for (int i = 0 ; i < finance_data_array1.get_size() ; i++)
//		printf("%d ", finance_data_array1[i]);
//	printf("\n");
//	FinanceIntDataArray finance_data_array3;
//	finance_data_array3 = finance_data_array1 + finance_data_array2;
//	for (int i = 0 ; i < finance_data_array3.get_size() ; i++)
//		printf("%d ", finance_data_array3[i]);
//	printf("\n");
//	FinanceIntDataArray finance_data_array4;
//	finance_data_array3.get_sub_array(finance_data_array4, 1, 5);
//	for (int i = 0 ; i < finance_data_array4.get_size() ; i++)
//		printf("%d ", finance_data_array4[i]);
//	printf("\n");
//	FinanceIntDataArray finance_data_array5;
//	finance_data_array3.get_diff_array(finance_data_array5, 1, 5);
//	for (int i = 0 ; i < finance_data_array5.get_size() ; i++)
//		printf("%d ", finance_data_array5[i]);
//	printf("\n");

	FinanceIntDataArray finance_data_array6;
	finance_data_array6.add(1);
	finance_data_array6.add(2);
//	finance_data_array6.add(3);
//	finance_data_array6.add(4);
//	finance_data_array6.add(5);
//	finance_data_array6.add(6);
//	finance_data_array6.add(7);
//	finance_data_array6.add(8);
	for (int i = 0 ; i < finance_data_array6.get_size() ; i++)
		printf("%d ", finance_data_array6[i]);
	printf("\n");
	FinanceFloatDataArray finance_data_array7;
	finance_data_array6.get_avg_array(finance_data_array7, 3, 0);
	for (int i = 0 ; i < finance_data_array7.get_size() ; i++)
		printf("%.2f ", finance_data_array7[i]);
	printf("\n");

	exit(EXIT_SUCCESS);

//
//	printf("Average1: %f\n", average<int>(finance_data_array1));
//	printf("Average2: %f\n", average<int>(finance_data_array2));
//	printf("StandardDeviation1: %f\n", standard_deviation<int>(finance_data_array1));
//	printf("StandardDeviation2: %f\n", standard_deviation<int>(finance_data_array2));
//	printf("Covariance: %f\n", covariance<int>(finance_data_array1, finance_data_array2));
//	printf("Correlation: %f\n", correlation<int>(finance_data_array1, finance_data_array2));
//	exit(EXIT_SUCCESS);

	unsigned short ret = finance_analyzer_mgr.initialize();
	if (CHECK_FAILURE(ret))
		return ret;

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
	printf("--run_daily\nDescription: Run daily data\nCaution: Other flags are ignored\n");
	printf("--analyze_daily\nDescription: Analyze daily data\nCaution: Other flags are ignored\n");
	printf("--show_console\nDescription: Print the runtime info on STDOUT/STDERR\n");
	printf("===================================================\n");
}
