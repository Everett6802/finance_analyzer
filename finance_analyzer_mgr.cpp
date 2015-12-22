#include <assert.h>
#include <stdexcept>
#include <string>
#include <set>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"


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
	if (!query_set->is_add_query_done())
	{
		WRITE_ERROR("The setting of query data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
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
		FinanceAnalyzerSqlReader::get_sql_field_command(source_index, query_field, field_cmd);
// Query the data in each table
		int start_year = time_range_cfg->get_start_time()->get_year();
		int end_year = time_range_cfg->get_end_time()->get_year();
// Search for each table year by year
		for (int year = start_year ; year <= end_year ; year++)
		{
			char table_name[16];
			snprintf(table_name, 16, "%s%d", MYSQL_TABLE_NAME_BASE, year);
			PTIME_RANGE_CFG time_range_cfg_in_year = NULL;
			if (year == start_year || year == end_year)
			{
				if (start_year == end_year)
					time_range_cfg_in_year = new TimeRangeCfg(time_range_cfg->get_start_time()->to_string(), time_range_cfg->get_end_time()->to_string());
				else if (year == start_year)
					time_range_cfg_in_year = new TimeRangeCfg(time_range_cfg->get_start_time()->to_string(), NULL);
				else
					time_range_cfg_in_year = new TimeRangeCfg(NULL, time_range_cfg->get_end_time()->to_string());
				if (time_range_cfg_in_year == NULL)
				{
					WRITE_ERROR("Fail to allocate memory: time_range_cfg_in_year");
					return RET_FAILURE_INSUFFICIENT_MEMORY;
				}
			}
			ret = finance_analyzer_sql_reader->select_data(source_index, string(table_name), field_cmd, (const PDEQUE_INT)&query_field, time_range_cfg_in_year, result_set);
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

unsigned short FinanceAnalyzerMgr::run_daily()
{
	DECLARE_WORKDAY_CANLENDAR();
	DECLARE_DATABASE_TIME_RANGE();

	unsigned short ret = RET_SUCCESS;
//	int year = 2016, month = 9, day = 4;
	int year, month, day;
	ret = workday_canlendar->get_last_workday(year, month, day);
	if (CHECK_FAILURE(ret))
		return ret;
	WRITE_FORMAT_DEBUG("The workday: %04d-%02d-%02d", year, month, day);
	int prev_year, prev_month, prev_day;
	ret = workday_canlendar->get_prev_workday(year, month, day, prev_year, prev_month, prev_day);
	if (CHECK_FAILURE(ret))
		return ret;
	WRITE_FORMAT_DEBUG("The previous workday: %04d-%02d-%02d", prev_year, prev_month, prev_day);

/*
* 臺股指數及成交量
	成交金額(2), 發行量加權股價指數(4), 漲跌點數(5)
* 三大法人現貨買賣超
	自營商(自行買賣)_買賣差額(3), 自營商(避險)_買賣差額(6), 投信_買賣差額(9), 外資及陸資_買賣差額(12)
* 現貨融資融券餘額
	融資金額(仟元)_前日餘額(14), 融資金額(仟元)_今日餘額(15)
* 三大法人期貨和選擇權留倉淨額
	自營商_多空淨額_契約金額(6), 投信_多空淨額_契約金額(12), 外資_多空淨額_契約金額(18)
* 三大法人選擇權賣權買權比
	買賣權未平倉量比率%(6)
* 三大法人選擇權買賣權留倉淨額
	買權_自營商_買方_口數(1), 買權_自營商_賣方_口數(3), 買權_外資_買方_口數(13), 買權_外資_賣方_口數(15), 賣權_自營商_買方_口數(19), 賣權_自營商_賣方_口數(21), 賣權_外資_買方_口數(31), 賣權_外資_賣方_口數(33)
* 十大交易人及特定法人期貨資訊
	臺股期貨_到期月份_買方_前十大交易人合計_部位數(3), 臺股期貨_到期月份_賣方_前十大交易人合計_部位數(7), 臺股期貨_所有契約_買方_前十大交易人合計_部位數(12), 臺股期貨_所有契約_賣方_前十大交易人合計_部位數(16)
 */
//	PQUERY_SET query_set = new QuerySet();
//	PSINGLE_TIME_RANGE_CFG time_range_cfg = new SingleTimeRangeCfg(year, month, day);
//	PTIME_RANGE_CFG time_range_cfg = new TimeRangeCfg(prev_year, prev_month, prev_day, year, month, day);
//	PRESULT_SET result_set = new ResultSet();
	SmartPointer<QuerySet> sp_query_set(new QuerySet());
	SmartPointer<TimeRangeCfg> sp_time_range_cfg(new TimeRangeCfg(prev_year, prev_month, prev_day, year, month, day));
	SmartPointer<ResultSet> sp_result_set(new ResultSet());
	set<int> source_type_index_set;

//	ADD_QUERY_EX((*query_set), FinanceSource_StockExchangeAndVolume, 2, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockExchangeAndVolume, 2, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockExchangeAndVolume, 4, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockExchangeAndVolume, 5, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockMarginTradingAndShortSelling, 14, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_StockMarginTradingAndShortSelling, 15, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionPutCallRatio, 6, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 1, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 3, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 13, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 15, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 19, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 21, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 31, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 33, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureTop10DealersAndLegalPersons, 3, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureTop10DealersAndLegalPersons, 7, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureTop10DealersAndLegalPersons, 12, source_type_index_set);
	ADD_QUERY_EX((*sp_query_set.get_instance()), FinanceSource_FutureTop10DealersAndLegalPersons, 16, source_type_index_set);
	sp_query_set->add_query_done();

// Check the boundary of each database
//	source_type_index_set.insert(FinanceSource_StockExchangeAndVolume);
//	source_type_index_set.insert(FinanceSource_StockTop3LegalPersonsNetBuyOrSell);
//	source_type_index_set.insert(FinanceSource_StockMarginTradingAndShortSelling);
//	source_type_index_set.insert(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest);
//	source_type_index_set.insert(FinanceSource_OptionPutCallRatio);
//	source_type_index_set.insert(FinanceSource_FutureTop10DealersAndLegalPersons);
	ret = database_time_range->restrict_time_range(source_type_index_set, sp_time_range_cfg.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;
//	printf("The new search time range: %s\n", time_range_cfg->to_string());

// Query the data from MySQL
	ret = query(sp_time_range_cfg.get_instance(), sp_query_set.get_instance(), sp_result_set.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;
// Show the result
//	ret = result_set->show_data();
//	if (CHECK_FAILURE(ret))
//		goto OUT;
// Write into file
	char filepath[32];
	snprintf(filepath, 32, DAILY_FINANCE_FILENAME_FORMAT, year, month, day);
	SmartPointer<TimeCfg> sp_time_cfg(new TimeCfg(year, month, day));
	ret = write_daily(sp_time_cfg, filepath, sp_result_set.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;

//OUT:
//	RELEASE_OBJ(result_set);
//	RELEASE_OBJ(query_set);

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMgr::write_daily(const SmartPointer<TimeCfg>& sp_time_cfg, const char* filepath, const PRESULT_SET result_set)const
{
	assert(filepath != NULL && "filepath should NOT be NULL");
	assert(result_set != NULL && "result_set should NOT be NULL");
// Open the file for writing the daily financial data
	FILE* fp = fopen(filepath, "w");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open file: %s, due to: %s", filepath, strerror(errno));
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
// Write the data into file
	fprintf(fp, "日期: %04d-%02d-%02d\n", sp_time_cfg->get_year(), sp_time_cfg->get_month(), sp_time_cfg->get_day());
	fprintf(fp, "發行量加權股價指數: %.2f, 漲跌: %.2f, 成交金額(億): %.2f, 變化(億): %.2f\n\n",
		result_set->get_float_array_element(FinanceSource_StockExchangeAndVolume, 4, 1),
		result_set->get_float_array_element(FinanceSource_StockExchangeAndVolume, 5, 1),
		result_set->get_long_array_element(FinanceSource_StockExchangeAndVolume, 2, 1) / 100000000.0,
		(result_set->get_long_array_element(FinanceSource_StockExchangeAndVolume, 2, 1) - result_set->get_long_array_element(FinanceSource_StockExchangeAndVolume, 2, 0)) / 100000000.0
		);
	fprintf(fp, "三大法人買賣超\n外資及陸資: %ld, 變化: %ld\n投信: %ld, 變化: %ld\n自營商: %ld, 變化: %ld\n\n",
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, 1),
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, 1) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, 0),
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, 1),
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, 1) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, 0),
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, 1) + result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, 1),
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, 1) + result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, 1) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, 0) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, 0)
		);
	fprintf(fp, "融資餘額(仟元): %ld, 變化: %ld\n\n",
			result_set->get_long_array_element(FinanceSource_StockMarginTradingAndShortSelling, 15, 1),
			result_set->get_long_array_element(FinanceSource_StockMarginTradingAndShortSelling, 15, 1) - result_set->get_long_array_element(FinanceSource_StockMarginTradingAndShortSelling, 14, 1)
		);
	fprintf(fp, "三大法人期權留倉淨額\n外資: %d, 變化: %d\n投信: %d, 變化: %d\n自營商: %d, 變化: %d\n\n",
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, 1),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, 1) - result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, 0),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, 1),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, 1) - result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, 0),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, 1),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, 1) - result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, 0)
		);
	fprintf(fp, "未平倉Put/Call Ratio: %.2f, 變化: %.2f\n\n",
		result_set->get_float_array_element(FinanceSource_OptionPutCallRatio, 6, 1),
		result_set->get_float_array_element(FinanceSource_OptionPutCallRatio, 6, 1) - result_set->get_float_array_element(FinanceSource_OptionPutCallRatio, 6, 0)
		);
	fprintf(fp, "選擇權買賣權留倉口數\n外資 BuyCall: %d, 變化: %d, BuyPut: %d, 變化: %d, SellCall: %d, 變化: %d, SellPut: %d, 變化: %d\n自營商 BuyCall: %d, 變化: %d, BuyPut: %d, 變化: %d, SellCall: %d, 變化: %d, SellPut: %d, 變化: %d\n\n",
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 13, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 13, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 13, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 15, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 15, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 15, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 31, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 31, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 31, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 33, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 33, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 33, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 1, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 1, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 1, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 3, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 3, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 3, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 19, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 19, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 19, 0),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 21, 1),
		result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 21, 1) - result_set->get_int_array_element(FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 21, 0)
		);
	fprintf(fp, "十大交易人及特法留倉淨口數\n近月: %d, 變化: %d\n全月: %d, 變化: %d\n\n",
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 3, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 7, 1),
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 3, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 7, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 3, 0) + result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 7, 0),
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 12, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 16, 1),
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 12, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 16, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 12, 0) + result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 16, 0)
		);
// Close the file
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;
}
#ifdef DO_DEBUG
unsigned short FinanceAnalyzerMgr::test()
{
	PTIME_RANGE_CFG time_range_cfg = new TimeRangeCfg(2015, 8, 5, 2015, 11, 11);
	PQUERY_SET query_set = new QuerySet();
//	query_set->add_query(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, -1);
//	query_set->add_query(FinanceSource_FutureTop10DealersAndLegalPersons, 2);
//	query_set->add_query(FinanceSource_FutureTop10DealersAndLegalPersons, 3);
	ADD_QUERY((*query_set), FinanceSource_StockTop3LegalPersonsNetBuyOrSell, -1);
	ADD_QUERY((*query_set), FinanceSource_FutureTop10DealersAndLegalPersons, 2);
	ADD_QUERY((*query_set), FinanceSource_FutureTop10DealersAndLegalPersons, 3);
	query_set->add_query_done();
	PRESULT_SET result_set = new ResultSet();

	unsigned short ret = query(time_range_cfg, query_set, result_set);
	if (CHECK_FAILURE(ret))
		return ret;

	ret = result_set->show_data();
	if (CHECK_FAILURE(ret))
		return ret;

	return RET_SUCCESS;
}
#endif
