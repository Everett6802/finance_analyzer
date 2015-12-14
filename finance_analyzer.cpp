#include <stdio.h>
#include <stdlib.h>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;


#include <list>
#include <queue>
#include <map>
#include <string>
#include "finance_analyzer_workday_canlendar.h"
using namespace std;

int main()
{
	static PFINANCE_ANALYZER_WORKDAY_CANLENDAR workday_canlendar = FinanceAnalyzerWorkdayCanlendar::get_instance();
	printf("%s\n", (workday_canlendar->is_workday(1999, 9, 4) ? "True" : "False"));
	printf("%s\n", (workday_canlendar->is_workday(2015, 9, 5) ? "True" : "False"));

//	typedef list<int> DAY_LIST;
//	typedef DAY_LIST* PDAY_LIST;
//	typedef map<int, PDAY_LIST> YEAR_MAP;
//
//	YEAR_MAP non_workday_map;
//	char str[] = "[2000]1:1,2,3,8,9,16,22,23,30;2:2,3,4,5,6,7,8,12,13,20,26,27,28;3:5,11,12,18,19,25,26;4:2,3,4,9,16,22,23,30;5:1,7,13,14,21,27,28;6:4,6,10,11,18,24,25;7:1,2,8,9,16,22,23,30;8:6,12,13,20,23,26,27;9:3,9,10,12,17,23,24;10:1,8,10,14,15,22,28,29;11:5,11,12,19,25,26;12:3,9,10,17,23,24,25,31";
//	char start_time_str[16];
//	char end_time_str[16];
//	int year;
//	sscanf(str, "[%d]", &year);
////	printf("start time: %s, end time: %s\n", start_time_str, end_time_str);
//	printf("[%04d]\n", year);
//	PDAY_LIST day_list_ptr = new DAY_LIST[12];
//	if (day_list_ptr == NULL)
//	{
//		fprintf(stderr, "Fail to allocate memory: day_list_ptr\n");
//		return 0;
//	}
//
//	char* tmp = strtok(str, "]");
//	char* year_no_workday_list_str = strtok(NULL, "]");
//	char* month_no_workday_list_str =  strtok(year_no_workday_list_str, ";");
//	list<char*> month_no_workday_list;
//	while (month_no_workday_list_str != NULL)
//	{
////		printf("month_no_workday_list_str: %s\n", month_no_workday_list_str);
//		int month;
//		char month_tmp[4];
//		sscanf(month_no_workday_list_str, "%d:", &month);
////	//	printf("start time: %s, end time: %s\n", start_time_str, end_time_str);
//		snprintf(month_tmp, 4, "%d:", month);
//		int month_tmp_len = strlen(month_tmp);
//		char* month_no_workday_list_str_tmp = new char[strlen(month_no_workday_list_str) - month_tmp_len + 1];
//		if (month_no_workday_list_str_tmp == NULL)
//		{
//			fprintf(stderr, "Fail to allocate memory: month_no_workday_list_str_tmp\n");
//			return 0;
//		}
//		strcpy(month_no_workday_list_str_tmp, &month_no_workday_list_str[month_tmp_len]);
//		month_no_workday_list.push_back(month_no_workday_list_str_tmp);
//		if (month != month_no_workday_list.size())
//		{
//			fprintf(stderr, "Incorrect month, expected: %d, actual: %d\n", month, month_no_workday_list.size());
//			return 0;
//		}
//		month_no_workday_list_str =  strtok(NULL, ";");
//	}
//	list<char*>::iterator iter = month_no_workday_list.begin();
//	int month_count = 0;
//	while (iter != month_no_workday_list.end())
//	{
//		char* month_no_workday_list_str = (char*)*iter;
//		char* day_no_workday_str =  strtok(month_no_workday_list_str, ",");
//		while (day_no_workday_str != NULL)
//		{
//			printf("%s ", day_no_workday_str);
//			day_list_ptr[month_count].push_back(atoi(day_no_workday_str));
//			day_no_workday_str = strtok(NULL, ",");
//		}
//		delete month_no_workday_list_str;
//		printf("\n");
//		iter++;
//		month_count++;
//	}
//	non_workday_map[year] = day_list_ptr;
//	month_no_workday_list.clear();
//
//	YEAR_MAP::iterator iter = non_workday_map.begin();
//	while(iter != non_workday_map.end())
//	{
//		PDAY_LIST day_list_ptr = (PDAY_LIST)iter->second;
//		delete day_list_ptr;
//	}
//	non_workday_map.clear();
//
//	exit(EXIT_SUCCESS);

//	char* value_str = "4901376433";
//	long value = atol(value_str);
//	printf("value_str: %s, value: %ld\n", value_str, value);
	unsigned short ret = RET_SUCCESS;
	ret = finance_analyzer_mgr.run_daily();
	if (CHECK_FAILURE(ret))
	{
		fprintf(stderr, "Error occurs, due to: %d, %s\n", ret, get_ret_description(ret));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

