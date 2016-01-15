#include <assert.h>
#include <stdexcept>
#include <string>
#include <set>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_calculator.h"
#include "finance_analyzer_workday_canlendar.h"
#include "finance_analyzer_database_time_range.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerMgr::FinanceAnalyzerMgr() :
	finance_analyzer_sql_reader(NULL),
	finance_analyzer_calculator(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_WORKDAY_CANLENDAR()
	IMPLEMENT_DATABASE_TIME_RANGE()
//	finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
//	if (finance_analyzer_sql_reader == NULL)
//		throw bad_alloc();
}

FinanceAnalyzerMgr::~FinanceAnalyzerMgr()
{
	if (finance_analyzer_calculator != NULL)
	{
		delete finance_analyzer_calculator;
		finance_analyzer_calculator = NULL;
	}
	if (finance_analyzer_sql_reader != NULL)
	{
		delete finance_analyzer_sql_reader;
		finance_analyzer_sql_reader = NULL;
	}
	RELEASE_DATABASE_TIME_RANGE()
	RELEASE_WORKDAY_CANLENDAR()
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerMgr::parse_config()
{
// Open the config file
	char filepath[32];
	snprintf(filepath, 32, "%s/%s", CONFIG_FOLDER_NAME, FINANCE_ANALYZER_CONF_FILENAME);
	FILE* fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the file: %s, due to: %s", filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Parse the config file
	static const int BUF_SIZE = 256;
	char buf[BUF_SIZE];
	ConfigFieldType config_field_type = CONFIG_FIELD_UNKNOWN;
	while (fgets(buf, BUF_SIZE, fp))
	{
		if (buf[0] == '[')
		{
			static const char* config_field[] = {"Unknown", "[email_address]"};
			config_field_type = CONFIG_FIELD_UNKNOWN;
			for (int i = 0 ; i < CONFIG_FIELD_SIZE ; i++)
			{
				if (strncmp(buf, config_field[i], strlen(config_field[i])) == 0)
				{
					WRITE_FORMAT_DEBUG("Parse the parameter in the filed: %s", config_field[i]);
					config_field_type = (ConfigFieldType)i;
					break;
				}
			}
			if (config_field_type == CONFIG_FIELD_UNKNOWN)
			{
				WRITE_FORMAT_ERROR("Unknown field title in config[%s]: %s", filepath, buf);
				return RET_FAILURE_INCORRECT_CONFIG;
			}
		}
		else
		{
			int buf_len = strlen(buf);
			if (buf_len != 0)
			{
				if (buf[buf_len - 1] == '\n')
					buf[buf_len - 1] = '\0';
			}
			if (strlen(buf) == 0)
				continue;
			switch(config_field_type)
			{
			case CONFIG_FIELD_EMAIL_ADDRESS:
			{
				string address(buf);
				email_address_list.push_back(address);
			}
			break;
			default:
			{
				WRITE_FORMAT_ERROR("Unsupported field type: %d", config_field_type);
				return RET_FAILURE_INCORRECT_CONFIG;
			}
			break;
			}
		}
	}

// CLose the handle of the config file
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMgr::initialize()
{
	finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
	if (finance_analyzer_sql_reader == NULL)
	{
		WRITE_ERROR("Fail to allocate memory: finance_analyzer_sql_reader");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}

	finance_analyzer_calculator = new FinanceAnalyzerCalculator();
	if (finance_analyzer_calculator == NULL)
	{
		WRITE_ERROR("Fail to allocate memory: finance_analyzer_calculator");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}

	unsigned short ret = RET_SUCCESS;
	ret = parse_config();
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
	if (!query_set->is_add_query_done())
	{
		WRITE_ERROR("The setting of query data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}

// Collect the information that what kind of the data source will be queried
	set<int> source_type_index_set;
	for (int i = 0 ; i < FinanceSourceSize ; i++)
	{
		if (!(*query_set)[i].empty())
			source_type_index_set.insert(i);
	}

	unsigned short ret = RET_SUCCESS;
// Check the boundary of each database
	SmartPointer<TimeRangeCfg> sp_restricted_time_range_cfg(new TimeRangeCfg(*time_range_cfg));
	ret = database_time_range->restrict_time_range(source_type_index_set, sp_restricted_time_range_cfg.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;
	//	printf("The new search time range: %s\n", sp_restricted_time_range_cfg->to_string());

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
		int start_year = sp_restricted_time_range_cfg->get_start_time()->get_year();
		int end_year = sp_restricted_time_range_cfg->get_end_time()->get_year();
// Search for each table year by year
		for (int year = start_year ; year <= end_year ; year++)
		{
			char table_name[16];
			snprintf(table_name, 16, "%s%d", MYSQL_TABLE_NAME_BASE, year);
			PTIME_RANGE_CFG time_range_cfg_in_year = NULL;
			if (year == start_year || year == end_year)
			{
				if (start_year == end_year)
					time_range_cfg_in_year = new TimeRangeCfg(sp_restricted_time_range_cfg->get_start_time()->to_string(), sp_restricted_time_range_cfg->get_end_time()->to_string());
				else if (year == start_year)
					time_range_cfg_in_year = new TimeRangeCfg(sp_restricted_time_range_cfg->get_start_time()->to_string(), NULL);
				else
					time_range_cfg_in_year = new TimeRangeCfg(NULL, sp_restricted_time_range_cfg->get_end_time()->to_string());
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

unsigned short FinanceAnalyzerMgr::correlate(const PTIME_RANGE_CFG time_range_cfg, FinanceSourceType finance_source_type1, int finance_field_no1, FinanceSourceType finance_source_type2, int finance_field_no2, float& correlation)const
{
	assert(time_range_cfg != NULL && "time_range_cfg should NOT be NULL");
	SmartPointer<QuerySet> sp_query_set(new QuerySet());
	SmartPointer<TimeRangeCfg> sp_time_range_cfg(new TimeRangeCfg(*time_range_cfg));
	SmartPointer<ResultSet> sp_result_set(new ResultSet());
	ADD_QUERY((*sp_query_set.get_instance()), finance_source_type1, finance_field_no1);
	ADD_QUERY((*sp_query_set.get_instance()), finance_source_type2, finance_field_no2);
	sp_query_set->add_query_done();

	unsigned short ret = RET_SUCCESS;
// Query the data from MySQL
	ret = query(sp_time_range_cfg.get_instance(), sp_query_set.get_instance(), sp_result_set.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;


	return ret;
}

unsigned short FinanceAnalyzerMgr::run_daily()
{
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
#ifdef DO_DEBUG
	ret = sp_result_set->show_data();
	if (CHECK_FAILURE(ret))
		return ret;
#endif
// Write into file
	SmartPointer<TimeCfg> sp_time_cfg(new TimeCfg(year, month, day));
	ret = show_daily(sp_time_cfg, sp_result_set.get_instance());
	if (CHECK_FAILURE(ret))
		return ret;

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMgr::show_daily(const SmartPointer<TimeCfg>& sp_time_cfg, const PRESULT_SET result_set, int show_result_type)const
{
//	assert(filepath != NULL && "filepath should NOT be NULL");
	assert(result_set != NULL && "result_set should NOT be NULL");

	static const int BUF_SIZE = 512;
	static char buf[BUF_SIZE];

	unsigned short ret = RET_SUCCESS;
// Check the folder of keeping track of the result exist
	ret = create_folder_in_project_if_not_exist(RESULT_FOLDER_NAME);
	if (CHECK_FAILURE(ret))
		return ret;

	string buf_string = "";
// Assemble the data
	snprintf(buf, BUF_SIZE, "日期: %04d-%02d-%02d\n", sp_time_cfg->get_year(), sp_time_cfg->get_month(), sp_time_cfg->get_day());
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "發行量加權股價指數: %.2f, 漲跌: %.2f, 成交金額(億): %.2f, 變化(億): %.2f\n\n",
		result_set->get_float_array_element(FinanceSource_StockExchangeAndVolume, 4, 1),
		result_set->get_float_array_element(FinanceSource_StockExchangeAndVolume, 5, 1),
		result_set->get_long_array_element(FinanceSource_StockExchangeAndVolume, 2, 1) / 100000000.0,
		(result_set->get_long_array_element(FinanceSource_StockExchangeAndVolume, 2, 1) - result_set->get_long_array_element(FinanceSource_StockExchangeAndVolume, 2, 0)) / 100000000.0
		);
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "三大法人買賣超(億)\n外資及陸資: %.2f, 變化: %.2f\n投信: %.2f, 變化: %.2f\n自營商: %.2f, 變化: %.2f\n\n",
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, 1) / 100000000.0,
		(result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, 1) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 12, 0))  / 100000000.0,
		result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, 1) / 100000000.0,
		(result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, 1) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 9, 0)) / 100000000.0,
		(result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, 1) + result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, 1)) / 100000000.0,
		(result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, 1) + result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, 1) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 3, 0) - result_set->get_long_array_element(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, 6, 0)) / 100000000.0
		);
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "融資餘額(億): %.2f, 變化: %.2f\n\n",
			result_set->get_long_array_element(FinanceSource_StockMarginTradingAndShortSelling, 15, 1) / 100000.0,
			(result_set->get_long_array_element(FinanceSource_StockMarginTradingAndShortSelling, 15, 1) - result_set->get_long_array_element(FinanceSource_StockMarginTradingAndShortSelling, 14, 1)) / 100000.0
		);
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "三大法人期權留倉淨額\n外資: %d, 變化: %d\n投信: %d, 變化: %d\n自營商: %d, 變化: %d\n\n",
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, 1),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, 1) - result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 18, 0),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, 1),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, 1) - result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 12, 0),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, 1),
		result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, 1) - result_set->get_int_array_element(FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest, 6, 0)
		);
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "未平倉Put/Call Ratio: %.2f, 變化: %.2f\n\n",
		result_set->get_float_array_element(FinanceSource_OptionPutCallRatio, 6, 1),
		result_set->get_float_array_element(FinanceSource_OptionPutCallRatio, 6, 1) - result_set->get_float_array_element(FinanceSource_OptionPutCallRatio, 6, 0)
		);
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "選擇權買賣權留倉口數\n外資\n Buy Call: %d, 變化: %d\n Buy Put: %d, 變化: %d\n Sell Call: %d, 變化: %d\n Sell Put: %d, 變化: %d\n自營商\n Buy Call: %d, 變化: %d\n Buy Put: %d, 變化: %d\n Sell Call: %d, 變化: %d\n Sell Put: %d, 變化: %d\n\n",
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
	buf_string += string(buf);
	snprintf(buf, BUF_SIZE, "十大交易人及特法留倉淨口數\n近月: %d, 變化: %d\n全月: %d, 變化: %d\n\n",
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 3, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 7, 1),
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 3, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 7, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 3, 0) + result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 7, 0),
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 12, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 16, 1),
		result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 12, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 16, 1) - result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 12, 0) + result_set->get_int_array_element(FinanceSource_FutureTop10DealersAndLegalPersons, 16, 0)
		);
	buf_string += string(buf);

// SHow result on the screen
	if (show_result_type & SHOW_RESULT_STDOUT)
	{
// Write the data into STDOUT
		ret =  direct_string_to_output_stream(buf_string.c_str());
		if (CHECK_FAILURE(ret))
			return ret;
	}
// Write the data into file
	if (show_result_type & SHOW_RESULT_FILE)
	{
		char filename[32];
		snprintf(filename, 32, DAILY_FINANCE_FILENAME_FORMAT, sp_time_cfg->get_year(), sp_time_cfg->get_month(), sp_time_cfg->get_day());
		char filepath[32];
		snprintf(filepath, 32, "%s/%s", RESULT_FOLDER_NAME, filename);
		WRITE_FORMAT_DEBUG("Write daily data to file[%s]", filepath);
		ret = direct_string_to_output_stream(buf_string.c_str(), filepath);
		if (CHECK_FAILURE(ret))
			return ret;
		printf("Check the result in file: %s\n", filepath);
	}
// Send the result by email
	if (show_result_type & SHOW_RESULT_EMAIL)
	{
		char title[32];
		snprintf(title, 32, DAILY_FINANCE_EMAIL_TITLE_FORMAT, sp_time_cfg->get_year(), sp_time_cfg->get_month(), sp_time_cfg->get_day());
		for (list<string>::const_iterator iter = email_address_list.begin() ; iter != email_address_list.end() ; iter++)
		{
			string email_address = (string)*iter;
			WRITE_FORMAT_DEBUG("Write daily data by email[%s] to %s", title, email_address.c_str());
			ret = send_email(title, email_address.c_str(), buf_string.c_str());
			if (CHECK_FAILURE(ret))
				return ret;
		}
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
