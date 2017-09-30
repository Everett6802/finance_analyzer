#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include "common.h"
#include "data_reader.h"


///////////////////////////////////////////////////////////////////////////////////
// The Data Collector Base Class
class DataCollectorBase
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
	DataCollectorBase();
	virtual ~DataCollectorBase();
};
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Data Collector Class
class MarketDataCollector : public DataCollectorBase
{
private:

public:
	MarketDataCollector();
	~MarketDataCollector();
};
typedef MarketDataCollector* PMARKET_DATA_COLLECTOR;
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Data Collector Class
class StockDataCollector : public DataCollectorBase
{
private:

public:
	StockDataCollector();
	~StockDataCollector();
};
typedef StockDataCollector* PSTOCK_DATA_COLLECTOR;
///////////////////////////////////////////////////////////////////////////////////

#endif
