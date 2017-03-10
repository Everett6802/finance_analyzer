#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_data_collector.h"
#include "finance_analyzer_sql_reader.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()


///////////////////////////////////////////////////////////////////////////////////
// The Data Collector Base Class
FinanceAnalyzerDataCollectorBase::FinanceAnalyzerDataCollectorBase() :
	search_rule_set(NULL),
	result_set_map(NULL)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerDataCollectorBase::~FinanceAnalyzerDataCollectorBase()
{
	cleanup_result_set_map();
	cleanup_search_rule_set();
	RELEASE_MSG_DUMPER()
}

// unsigned short FinanceAnalyzerDataCollectorBase::init_query_set_from_finance_mode(FinanceAnalysisMode mode)
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

unsigned short FinanceAnalyzerDataCollectorBase::init_search_rule_set()
{
	cleanup_search_rule_set();
	search_rule_set = new SearchRuleSet();
	if (search_rule_set == NULL)
		throw bad_alloc();
	return RET_SUCCESS;
}

void FinanceAnalyzerDataCollectorBase::cleanup_search_rule_set()
{
	if (search_rule_set != NULL)
	{
		delete search_rule_set;
		search_rule_set = NULL;
	}
}

unsigned short FinanceAnalyzerDataCollectorBase::init_result_set_map()
{
	cleanup_result_set_map();
	result_set_map = new ResultSetMap();
	if (result_set_map == NULL)
		throw bad_alloc();
	return RET_SUCCESS;
}
	
void FinanceAnalyzerDataCollectorBase::cleanup_result_set_map()
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
FinanceAnalyzerMarketDataCollector::FinanceAnalyzerMarketDataCollector()
{
}

FinanceAnalyzerMarketDataCollector::~FinanceAnalyzerMarketDataCollector()
{
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Data Collector Class
FinanceAnalyzerStockDataCollector::FinanceAnalyzerStockDataCollector()
{
}

FinanceAnalyzerStockDataCollector::~FinanceAnalyzerStockDataCollector()
{
}

///////////////////////////////////////////////////////////////////////////////////
