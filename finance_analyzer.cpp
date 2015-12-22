#include <stdio.h>
#include <stdlib.h>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


static FinanceAnalyzerMgr finance_analyzer_mgr;


#include <list>
#include <queue>
#include <map>
#include <set>
#include <string>
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"
using namespace std;

int main()
{
//	int* array = new int[100000];
//	printf("%d\n", sizeof(int*));
//	if (array == NULL)
//	{
//		fprintf(stderr, "error occurs\n");
//	}
//	return 0;
//	int year, month, day;
//	static PFINANCE_ANALYZER_WORKDAY_CANLENDAR workday_canlendar = FinanceAnalyzerWorkdayCanlendar::get_instance();
//	printf("%s\n", (workday_canlendar->is_workday(2015, 9, 4) ? "True" : "False"));
//	printf("%s\n", (workday_canlendar->is_workday(2015, 9, 5) ? "True" : "False"));
//	workday_canlendar->get_prev_workday(2015, 9, 4, year, month, day);
//	printf("%04d-%02d-%02d\n", year, month, day);
//	workday_canlendar->get_prev_workday(2000, 1, 1, year, month, day);
//	printf("%04d-%02d-%02d\n", year, month, day);
//	workday_canlendar->get_prev_workday(2015, 1, 1, year, month, day);
//	printf("%04d-%02d-%02d\n", year, month, day);
//	PTIME_CFG time_cfg = NULL;
//	workday_canlendar->get_next_workday(new TimeCfg(2015, 5, 30), &time_cfg);
//	printf("%04d-%02d-%02d\n", time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day());
//	workday_canlendar->get_next_workday(new TimeCfg(2015, 12, 31), &time_cfg);
//	printf("%04d-%02d-%02d\n", time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day());

//	DECLARE_DATABASE_TIME_RANGE()
//	std::set<int> my_set;
//	my_set.insert(1);
//	my_set.insert(2);
//	my_set.insert(1);
//	set<int>::iterator iter = my_set.begin();
//	while (iter != my_set.end())
//	{
//		printf("%d\n", *iter);
//		iter++;
//	}

	unsigned short ret = RET_SUCCESS;
	ret = finance_analyzer_mgr.run_daily();
	if (CHECK_FAILURE(ret))
	{
		fprintf(stderr, "Error occurs, due to: %d, %s\n", ret, get_ret_description(ret));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

