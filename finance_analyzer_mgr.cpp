#include <assert.h>
#include <stdexcept>
#include <string>
#include <set>
#include "finance_analyzer_mgr.h"
// #include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_data_collector.h"
#include "finance_analyzer_data_statistics.h"
// #include "finance_analyzer_workday_canlendar.h"
// #include "finance_analyzer_database_time_range.h"
#include "finance_analyzer_output.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

///////////////////////////////////////////////////////////////////////////////////
// The Manager Base Class
FinanceAnalyzerMgrBase::FinanceAnalyzerMgrBase() :
	data_collector(NULL)
	// data_statistics(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	// data_statistics = new FinanceAnalyzerDataStatistics();
	// if (data_statistics == NULL)
	// 	throw bad_alloc();
	// IMPLEMENT_WORKDAY_CANLENDAR()
	// IMPLEMENT_DATABASE_TIME_RANGE()
}

FinanceAnalyzerMgrBase::~FinanceAnalyzerMgrBase()
{
	// if (data_statistics != NULL)
	// {
	// 	delete data_statistics;
	// 	data_statistics = NULL;
	// }
	if (data_collector != NULL)
	{
		delete data_collector;
		data_collector = NULL;
	}
	// RELEASE_DATABASE_TIME_RANGE()
	// RELEASE_WORKDAY_CANLENDAR()
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerMgrBase::parse_config()
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
					WRITE_FORMAT_DEBUG("Parse the parameter in the file: %s", config_field[i]);
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

unsigned short FinanceAnalyzerMgrBase::initialize()
{
	unsigned short ret = RET_SUCCESS;
	ret = parse_config();
	if (CHECK_FAILURE(ret))
		return ret;

	return RET_SUCCESS;
}

#ifdef DO_DEBUG
unsigned short FinanceAnalyzerMgrBase::test()
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
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Manager Class
IFinanceAnalyzerMgr* FinanceAnalyzerMarketMgr::create_instance()
{
	IFinanceAnalyzerMgr* finance_analyzer_mgr = new FinanceAnalyzerMarketMgr();
	return finance_analyzer_mgr;
}

FinanceAnalyzerMarketMgr::FinanceAnalyzerMarketMgr() :
	market_data_collector(NULL)
{
	market_data_collector = new FinanceAnalyzerMarketDataCollector();
	if (market_data_collector == NULL)
		throw bad_alloc();
	data_collector = market_data_collector;
}

FinanceAnalyzerMarketMgr::~FinanceAnalyzerMarketMgr()
{
	market_data_collector = NULL;
}

unsigned short FinanceAnalyzerMarketMgr::initialize()
{
	WRITE_DEBUG("Initialize FinanceAnalyzerMarketMgr.....");
	// fprintf(stderr, "Initialize FinanceAnalyzerMarketMgr.....\n");
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMarketMgr::search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)
{
	WRITE_DEBUG("Initialize FinanceAnalyzerMarketMgr.....");
	// fprintf(stderr, "Initialize FinanceAnalyzerMarketMgr.....\n");
	return RET_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Manager Class
IFinanceAnalyzerMgr* FinanceAnalyzerStockMgr::create_instance()
{
	IFinanceAnalyzerMgr* finance_analyzer_mgr = new FinanceAnalyzerStockMgr();
	return finance_analyzer_mgr;
}

FinanceAnalyzerStockMgr::FinanceAnalyzerStockMgr() :
	stock_data_collector(NULL)
{
	stock_data_collector = new FinanceAnalyzerStockDataCollector();
	if (stock_data_collector == NULL)
		throw bad_alloc();
	data_collector = stock_data_collector;
}


FinanceAnalyzerStockMgr::~FinanceAnalyzerStockMgr()
{
	stock_data_collector = NULL;
}

unsigned short FinanceAnalyzerStockMgr::initialize()
{
	WRITE_DEBUG("Initialize FinanceAnalyzerStockMgr.....");
	// fprintf(stderr, "Initialize FinanceAnalyzerStockMgr.....\n");
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerStockMgr::search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)
{
	WRITE_DEBUG("Initialize FinanceAnalyzerStockMgr.....");
	// fprintf(stderr, "Initialize FinanceAnalyzerMarketMgr.....\n");
	return RET_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////////
