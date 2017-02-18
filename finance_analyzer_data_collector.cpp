#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_data_collector.h"
#include "finance_analyzer_sql_reader.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()


///////////////////////////////////////////////////////////////////////////////////
// The Data Collector Base Class
FinanceAnalyzerDataCollectorBase::FinanceAnalyzerDataCollectorBase() :
	query_set(NULL),
	sql_reader(NULL),
	result_set_map(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	sql_reader = new FinanceAnalyzerSqlReader();
	if (sql_reader == NULL)
		throw bad_alloc();
}

FinanceAnalyzerDataCollectorBase::~FinanceAnalyzerDataCollectorBase()
{
	cleanup_query_set();
	cleanup_result_set_map();
	if (sql_reader != NULL)
	{
		delete sql_reader;
		sql_reader = NULL;
	}
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerDataCollectorBase::init_query_set_from_finance_mode(FinanceAnalysisMode mode)
{
	cleanup_query_set();
	if (mode == FinanceAnalysis_Market)
		query_set = new MarketQuerySet();
	else if (mode == FinanceAnalysis_Stock) 
		query_set = new StockQuerySet();
	else
	{
		static const int ERRMSG_SIZE = 64;
		static char errmsg[ERRMSG_SIZE];
		snprintf(errmsg, ERRMSG_SIZE, "Unknown finance analysis mode: %d", mode);
		throw invalid_argument(errmsg);
	}
	if (query_set == NULL)
		throw bad_alloc();
	return RET_SUCCESS;
}

void FinanceAnalyzerDataCollectorBase::cleanup_query_set()
{
	if (query_set != NULL)
	{
		delete query_set;
		query_set = NULL;
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
FinanceAnalyzerMarketDataCollector::FinanceAnalyzerMarketDataCollector() :
	market_query_set(NULL)
{
}

FinanceAnalyzerMarketDataCollector::~FinanceAnalyzerMarketDataCollector()
{
	market_query_set = NULL;
}

unsigned short FinanceAnalyzerMarketDataCollector::init_query_set()
{
	init_query_set_from_finance_mode(FinanceAnalysis_Market);
	market_query_set = (PMARKET_QUERY_SET)query_set;
	return RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Data Collector Class
FinanceAnalyzerStockDataCollector::FinanceAnalyzerStockDataCollector() :
	stock_query_set(NULL)
{
}

FinanceAnalyzerStockDataCollector::~FinanceAnalyzerStockDataCollector()
{
	stock_query_set = NULL;
}

unsigned short FinanceAnalyzerStockDataCollector::init_query_set()
{
	init_query_set_from_finance_mode(FinanceAnalysis_Stock);
	stock_query_set = (PSTOCK_QUERY_SET)query_set;
	return RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
