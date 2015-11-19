#include <stdio.h>
#include <stdlib.h>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


#define GET_VALUE(DATA_NAME, index)\
DATA_NAME##_FIELD_DEFINITION[index]

static FinanceAnalyzerMgr finance_analyzer_mgr;

//int data1[] = {1, 2, 3};
//int data2[] = {11, 12, 13, 14};
//int data3[] = {21, 22, 23, 24, 25};

int main()
{
//	int *data_array[] = {data1, data2, data3};
////	int *data_array[] =
////	{
////		{1, 2, 3},
////		{11, 12, 13, 14},
////		{21, 22, 23, 24, 25}
////	};
//	printf("data1: %d\n", data_array[0][1]);
//	printf("data2: %d\n", data_array[1][2]);
//	printf("data3: %d\n", data_array[2][3]);

	unsigned short ret = RET_SUCCESS;

//	printf("%s\n", GET_VALUE(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS, 2));

//	ret = finance_analyzer_mgr.test();
//	if (CHECK_FAILURE(ret))
//	{
//		fprintf(stderr, "Error occurs, due to: %d\n", ret);
//		exit(EXIT_FAILURE);
//	}

	exit(EXIT_SUCCESS);
}

