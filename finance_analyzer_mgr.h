#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include "finance_analyzer_common.h"


class FinanceAnalyzerDataCollectorBase;
class FinanceAnalyzerDataStatisticsBase;
class FinanceAnalyzerMarketDataCollector;
class FinanceAnalyzerMarketDataStatistics;
class FinanceAnalyzerStockDataCollector;
class FinanceAnalyzerStockDataStatistics;

///////////////////////////////////////////////////////////////////////////////////
// The Manager Interface
class IFinanceAnalyzerMgr
{
private:
public:
	static IFinanceAnalyzerMgr* create_instance();

	virtual ~IFinanceAnalyzerMgr(){}
	virtual unsigned short initialize()=0;
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)=0;
};
typedef IFinanceAnalyzerMgr* PIFINANCE_ANALYZER_MGR;
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Manager Base Class
class FinanceAnalyzerMgrBase : public IFinanceAnalyzerMgr
{
protected:
	enum ConfigFieldType{CONFIG_FIELD_UNKNOWN, CONFIG_FIELD_EMAIL_ADDRESS, CONFIG_FIELD_SIZE};
	DECLARE_MSG_DUMPER()

	STRING_LIST email_address_list;
	FinanceAnalyzerDataCollectorBase* data_collector;
	// FinanceAnalyzerDataStatisticsBase* data_statistics;

	unsigned short parse_config();

public:
	FinanceAnalyzerMgrBase();
	virtual ~FinanceAnalyzerMgrBase();

	virtual unsigned short initialize();

#ifdef DO_DEBUG
	unsigned short test();
#endif
};
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Manager Class
class FinanceAnalyzerMarketMgr : public FinanceAnalyzerMgrBase
{
private:
	FinanceAnalyzerMarketDataCollector* market_data_collector;
	// FinanceAnalyzerMarketDataStatistics* market_data_statistics;

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerMarketMgr();
	virtual ~FinanceAnalyzerMarketMgr();

	virtual unsigned short initialize();
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

};
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Manager Class
class FinanceAnalyzerStockMgr : public FinanceAnalyzerMgrBase
{
private:
	FinanceAnalyzerStockDataCollector* stock_data_collector;
	// FinanceAnalyzerStockDataStatistics* stock_data_statistics;

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerStockMgr();
	virtual ~FinanceAnalyzerStockMgr();

	virtual unsigned short initialize();
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);
};
///////////////////////////////////////////////////////////////////////////////////

#endif
