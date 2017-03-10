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
	SearchRuleSet* search_rule_set;
	ResultSetMap* result_set_map;

	// unsigned short init_query_set_from_finance_mode(FinanceAnalysisMode mode);
	unsigned short init_search_rule_set();
	void cleanup_search_rule_set();
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

public:
	FinanceAnalyzerStockDataCollector();
	~FinanceAnalyzerStockDataCollector();
};
typedef FinanceAnalyzerStockDataCollector* PFINANCE_ANALYZER_STOCK_DATA_COLLECTOR;
///////////////////////////////////////////////////////////////////////////////////

#endif
