#ifndef FINANCE_ANALYZER_DATA_COLLECTOR_H
#define FINANCE_ANALYZER_DATA_COLLECTOR_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_sql_reader.h"


///////////////////////////////////////////////////////////////////////////////////
// The Data Collector Base Class
class FinanceAnalyzerDataCollectorBase
{
protected:
	DECLARE_MSG_DUMPER()
	QuerySet* query_set;
	FinanceAnalyzerSqlReader* sql_reader;
	ResultSetMap* result_set_map;

	unsigned short init_query_set_from_finance_mode(FinanceAnalysisMode mode);
	virtual unsigned short init_query_set()=0;
	void cleanup_query_set();
	unsigned short init_result_set_map();
	void cleanup_result_set_map();

public:
	FinanceAnalyzerDataCollectorBase();
	virtual ~FinanceAnalyzerDataCollectorBase();
};
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Data Collector Class
class FinanceAnalyzerMarketDataCollector : public FinanceAnalyzerDataCollectorBase
{
private:
	MarketQuerySet* market_query_set;

	virtual unsigned short init_query_set();

public:
	FinanceAnalyzerMarketDataCollector();
	~FinanceAnalyzerMarketDataCollector();
};
typedef FinanceAnalyzerMarketDataCollector* PFINANCE_ANALYZER_MARKET_DATA_COLLECTOR;
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Data Collector Class
class FinanceAnalyzerStockDataCollector : public FinanceAnalyzerDataCollectorBase
{
private:
	StockQuerySet* stock_query_set;

	virtual unsigned short init_query_set();

public:
	FinanceAnalyzerStockDataCollector();
	~FinanceAnalyzerStockDataCollector();
};
typedef FinanceAnalyzerStockDataCollector* PFINANCE_ANALYZER_STOCK_DATA_COLLECTOR;
///////////////////////////////////////////////////////////////////////////////////

#endif
