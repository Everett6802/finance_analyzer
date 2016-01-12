#include <stdio.h>
#include <stdlib.h>
#include "msg_dumper_wrapper.h"
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

using namespace std;

void show_usage();

int main(int argc, char** argv)
{
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
