#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include <string>
#include "finance_analyzer_common.h"


class FinanceAnalyzerDataCollectorBase;
class FinanceAnalyzerDataCalculatorBase;
class FinanceAnalyzerMarketDataCollector;
class FinanceAnalyzerMarketDataCalculator;
class FinanceAnalyzerStockDataCollector;
class FinanceAnalyzerStockDataCalculator;

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
// Only exploited in the Market mode
// Only exploited in the Stock mode
	virtual unsigned short get_stock_price_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& price_support_resistance_string, const char* stock_price_support_resistance_folderpath=NULL, bool show_detail=false, const char* stock_price_support_resistance_time_filter=NULL, price_support_resistance_volume_filter=NULL)=0;
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
	FinanceAnalyzerDataCalculatorBase* data_calculator;

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
	FinanceAnalyzerMarketDataCalculator* market_data_calculator;

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerMarketMgr();
	virtual ~FinanceAnalyzerMarketMgr();

	virtual unsigned short initialize();
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

	virtual unsigned short get_stock_price_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& price_support_resistance_string, const char* stock_price_support_resistance_folderpath=NULL, bool show_detail=false, const char* stock_price_support_resistance_time_filter=NULL, price_support_resistance_volume_filter=NULL);
};
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Manager Class
class FinanceAnalyzerStockMgr : public FinanceAnalyzerMgrBase
{
private:
	FinanceAnalyzerStockDataCollector* stock_data_collector;
	FinanceAnalyzerStockDataCalculator* stock_data_calculator;

	std::string stock_price_support_resistance_root_folderpath;

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerStockMgr();
	virtual ~FinanceAnalyzerStockMgr();

	virtual unsigned short initialize();
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

	virtual unsigned short get_stock_price_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& price_support_resistance_string, const char* stock_price_support_resistance_folderpath=NULL, bool show_detail=false, const char* stock_price_support_resistance_time_filter=NULL, price_support_resistance_volume_filter=NULL);
};
///////////////////////////////////////////////////////////////////////////////////

#endif
