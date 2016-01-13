#include <stdio.h>
#include <stdlib.h>
#include "msg_dumper_wrapper.h"
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_algorithm.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

using namespace std;

void show_usage();

int main(int argc, char** argv)
{
	FinanceIntDataArray finance_data_array1, finance_data_array2;
	finance_data_array1.add(1);
	finance_data_array1.add(2);
	finance_data_array1.add(3);
	finance_data_array1.add(4);
	finance_data_array1.add(5);
	finance_data_array2.add(-1);
	finance_data_array2.add(-2);
	finance_data_array2.add(-3);
	finance_data_array2.add(-4);
	finance_data_array2.add(-5);

	printf("Average1: %f\n", average<int>(finance_data_array1));
	printf("Average2: %f\n", average<int>(finance_data_array2));
	printf("StandardDeviation1: %f\n", standard_deviation<int>(finance_data_array1));
	printf("StandardDeviation2: %f\n", standard_deviation<int>(finance_data_array2));
	printf("Covariance: %f\n", covariance<int>(finance_data_array1, finance_data_array2));
	printf("Correlation: %f\n", correlation<int>(finance_data_array1, finance_data_array2));
	exit(EXIT_SUCCESS);

	unsigned short ret = finance_analyzer_mgr.initialize();
	if (CHECK_FAILURE(ret))
		return ret;

	int index = 1;
	int offset;
	for (; index < argc ; index += offset)
	{
		if (strcmp(argv[index], "--help") == 0)
		{
			show_usage();
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[index], "--run_daily") == 0)
		{
			unsigned short ret = RET_SUCCESS;
			ret = finance_analyzer_mgr.run_daily();
			if (CHECK_FAILURE(ret))
			{
				fprintf(stderr, "Error occurs, due to: %d, %s\n", ret, get_ret_description(ret));
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}
		else
		{
			fprintf(stderr, "Unknown param: %s\n", argv[index]);
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

void show_usage()
{
	printf("====================== Usage ======================\n");
	printf("-h|--help\nDescription: The usage\nCaution: Other flags are ignored\n");
	printf("--run_daily\nDescription: Run daily data\nCaution: Other flags are ignored\n");
	printf("===================================================\n");
}
