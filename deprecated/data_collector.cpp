#include <assert.h>
#include <stdexcept>
#include "data_collector.h"
#include "data_reader.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// The Data Collector Base Class
DataCollectorBase::DataCollectorBase() :
	search_rule_set(NULL),
	result_set_map(NULL)
{
	IMPLEMENT_MSG_DUMPER()
}

DataCollectorBase::~DataCollectorBase()
{
	cleanup_result_set_map();
	cleanup_search_rule_set();
	RELEASE_MSG_DUMPER()
}

// unsigned short DataCollectorBase::init_query_set_from_finance_mode(FinanceAnalysisMode mode)
// {
// 	cleanup_query_set();
// 	if (mode == FinanceAnalysis_Market)
// 		query_set = new MarketQuerySet();
// 	else if (mode == FinanceAnalysis_Stock) 
// 		query_set = new StockQuerySet();
// 	else
// 	{
// 		static const int ERRMSG_SIZE = 64;
// 		static char errmsg[ERRMSG_SIZE];
// 		snprintf(errmsg, ERRMSG_SIZE, "Unknown finance analysis mode: %d", mode);
// 		throw invalid_argument(errmsg);
// 	}
// 	if (query_set == NULL)
// 		throw bad_alloc();
// 	return RET_SUCCESS;
// }

unsigned short DataCollectorBase::init_search_rule_set()
{
	// cleanup_search_rule_set();
	// search_rule_set = new SearchRuleSet();
	// if (search_rule_set == NULL)
	// 	throw bad_alloc();
	return RET_SUCCESS;
}

void DataCollectorBase::cleanup_search_rule_set()
{
	if (search_rule_set != NULL)
	{
		delete search_rule_set;
		search_rule_set = NULL;
	}
}

unsigned short DataCollectorBase::init_result_set_map()
{
	cleanup_result_set_map();
	result_set_map = new ResultSetMap();
	if (result_set_map == NULL)
		throw bad_alloc();
	return RET_SUCCESS;
}
	
void DataCollectorBase::cleanup_result_set_map()
{
	if (result_set_map != NULL)
	{
		delete result_set_map;
		result_set_map = NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Manager Class
MarketDataCollector::MarketDataCollector()
{
}

MarketDataCollector::~MarketDataCollector()
{
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Data Collector Class
StockDataCollector::StockDataCollector()
{
}

StockDataCollector::~StockDataCollector()
{
}

///////////////////////////////////////////////////////////////////////////////////
