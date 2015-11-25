#include <assert.h>
#include <stdexcept>
#include <string>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_sql_reader.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerMgr::FinanceAnalyzerMgr() :
	finance_analyzer_sql_reader(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
	if (finance_analyzer_sql_reader == NULL)
		throw bad_alloc();
}

FinanceAnalyzerMgr::~FinanceAnalyzerMgr()
{
	if (finance_analyzer_sql_reader != NULL)
	{
		delete finance_analyzer_sql_reader;
		finance_analyzer_sql_reader = NULL;
	}
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerMgr::test()
{
	PTIME_RANGE_CFG time_range_cfg = new TimeRangeCfg(2015, 8, 5, 2015, 11, 11);
	PQUERY_SET query_set = new QuerySet();
	query_set->add_query(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, -1);
	query_set->add_query(FinanceSource_FutureTop10DealersAndLegalPersons, 2);
	query_set->add_query(FinanceSource_FutureTop10DealersAndLegalPersons, 3);
	PRESULT_SET result_set = new ResultSet();

	unsigned short ret = query(time_range_cfg, query_set, result_set);
	if (CHECK_FAILURE(ret))
		return ret;

	ret = result_set->show_data();
	if (CHECK_FAILURE(ret))
		return ret;

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMgr::query(const PTIME_RANGE_CFG time_range_cfg, const PQUERY_SET query_set, PRESULT_SET result_set)const
{
	assert(time_range_cfg != NULL && query_set != NULL && result_set != NULL);
	if (time_range_cfg->get_start_time() == NULL || time_range_cfg->get_end_time() == NULL)
	{
		WRITE_ERROR("The start/end time in time_range_cfg should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if (time_range_cfg->is_month_type())
	{
		WRITE_ERROR("The time format of time_range_cfg should be Day type");
		return RET_FAILURE_INVALID_ARGUMENT;
	}

	unsigned short ret = RET_SUCCESS;
//	for (QuerySet::iterator iter = query_set.begin() ; iter < query_set.end() ; iter++)
	for (int source_index = 0 ; source_index < FinanceSourceSize ; source_index++)
	{
		const DEQUE_INT& query_field = (*query_set)[source_index];
		if (query_field.empty())
			continue;
// Add to the result set
		ret = result_set->add_set(source_index, query_field);
		if (CHECK_FAILURE(ret))
			return ret;
// Connect to the database
		ret = finance_analyzer_sql_reader->try_connect_mysql(FINANCE_DATABASE_NAME_LIST[source_index]);
		if (CHECK_FAILURE(ret))
			return ret;
// Generate the field command
		string field_cmd = string("");
		FinanceAnalyzerSqlReader::get_sql_field_command(query_field, field_cmd);
// Query the data in each table
		int start_year = time_range_cfg->get_start_time()->get_year();
		int end_year = time_range_cfg->get_end_time()->get_year();
		for (int year = start_year ; year <= end_year ; year++)
		{
			char table_name[16];
			snprintf(table_name, 16, "%s%d", MYSQL_TABLE_NAME_BASE, year);
			PTIME_RANGE_CFG time_cfg = NULL;
			if (year == start_year || year == end_year)
			{
				if (start_year == end_year)
					time_cfg = new TimeRangeCfg(time_range_cfg->get_start_time()->to_string(), time_range_cfg->get_end_time()->to_string());
				else if (year == start_year)
					time_cfg = new TimeRangeCfg(time_range_cfg->get_start_time()->to_string(), NULL);
				else
					time_cfg = new TimeRangeCfg(NULL, time_range_cfg->get_end_time()->to_string());
				if (time_cfg == NULL)
				{
					WRITE_ERROR("Fail to allocate memory: time_cfg");
					return RET_FAILURE_INSUFFICIENT_MEMORY;
				}
			}
			ret = finance_analyzer_sql_reader->select_data(source_index, string(table_name), field_cmd, (const PDEQUE_INT)&query_field, time_cfg, result_set);
			if (CHECK_FAILURE(ret))
				return ret;
		}

// Disconnect from the database
		ret = finance_analyzer_sql_reader->disconnect_mysql();
		if (CHECK_FAILURE(ret))
			return ret;
		result_set->switch_to_check_date_mode();
	}
// Check the result
	ret = result_set->check_data();

	return ret;
}

