#include <stdio.h>
#include <stdlib.h>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

int main()
{
//	char* value_str = "4901376433";
//	long value = atol(value_str);
//	printf("value_str: %s, value: %ld\n", value_str, value);
	unsigned short ret = RET_SUCCESS;
	ret = finance_analyzer_mgr.test();
	if (CHECK_FAILURE(ret))
	{
		fprintf(stderr, "Error occurs, due to: %d\n", ret);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

