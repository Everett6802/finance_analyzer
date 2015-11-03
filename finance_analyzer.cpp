#include <stdio.h>
#include <stdlib.h>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;

int main()
{
	unsigned short ret = RET_SUCCESS;

	ret = finance_analyzer_mgr.test();
	if (CHECK_FAILURE(ret))
	{
		fprintf(stderr, "Error occurs, due to: %d\n", ret);
		exit(EXIT_FAILURE);
	}
//	for (int i = 0 ; i < FINANCE_DATA_NAME_LIST_LEN ; i++)
//		printf("%s\n", FINANCE_DATA_DESCRIPTION_LIST[i]);
	exit(EXIT_SUCCESS);
}

