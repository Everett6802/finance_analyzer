#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H

#include <string>
#include "common.h"
#include "data_source/data_source.h"


// class DataCollectorBase;
class DataCalculatorBase;
// class MarketDataCollector;
class MarketDataCalculator;
// class StockDataCollector;
class StockDataCalculator;

///////////////////////////////////////////////////////////////////////////////////
// The Manager Interface
class IFinanceAnalyzerMgr
{
private:
public:
	static IFinanceAnalyzerMgr* create_instance();

	virtual ~IFinanceAnalyzerMgr(){}
	virtual unsigned short initialize()=0;
	virtual void set_data_type(FinanceDataType data_type)=0;
	virtual FinanceDataType get_data_type()const=0;
	virtual void set_continue_when_non_exist(bool continue_when_non_exist)=0;
	virtual bool is_continue_when_non_exist()const=0;
	virtual unsigned short set_csv_root_folderpath(const char* root_folderpath)=0;
	virtual const char* get_csv_root_folderpath()const=0;
	virtual unsigned short set_shm_root_folderpath(const char* root_folderpath)=0;
	virtual const char* get_shm_root_folderpath()const=0;
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)=0;
// Only exploited in the Market mode
// Only exploited in the Stock mode
	virtual unsigned short get_stock_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& price_support_resistance_string, const char* stock_price_support_resistance_folderpath=NULL, bool show_detail=false, const char* stock_price_support_resistance_time_filter=NULL, const char* price_support_resistance_volume_filter=NULL)=0;
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
	DECLARE_DATA_READER()

	STRING_LIST email_address_list;
	FinanceDataType finance_data_type;
	bool continue_when_non_exist;
	char* csv_root_folderpath;
	char* shm_root_folderpath;
	// DataCollectorBase* data_collector;
	DataCalculatorBase* data_calculator;

	unsigned short parse_config();

public:
	FinanceAnalyzerMgrBase();
	virtual ~FinanceAnalyzerMgrBase();

	virtual unsigned short initialize();
	virtual void set_data_type(FinanceDataType data_type);
	virtual FinanceDataType get_data_type()const;
	virtual void set_continue_when_non_exist(bool continue_when_non_exist);
	virtual bool is_continue_when_non_exist()const;
	virtual unsigned short set_csv_root_folderpath(const char* root_folderpath);
	virtual const char* get_csv_root_folderpath()const;
	virtual unsigned short set_shm_root_folderpath(const char* root_folderpath);
	virtual const char* get_shm_root_folderpath()const;
	virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

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
	// MarketDataCollector* market_data_collector;
	MarketDataCalculator* market_data_calculator;

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerMarketMgr();
	virtual ~FinanceAnalyzerMarketMgr();

	virtual unsigned short initialize();
	// virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

	virtual unsigned short get_stock_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& price_support_resistance_string, const char* stock_price_support_resistance_folderpath=NULL, bool show_detail=false, const char* stock_price_support_resistance_time_filter=NULL, const char* price_support_resistance_volume_filter=NULL);
};
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Manager Class
class FinanceAnalyzerStockMgr : public FinanceAnalyzerMgrBase
{
private:
	// StockDataCollector* stock_data_collector;
	StockDataCalculator* stock_data_calculator;

	std::string stock_price_support_resistance_root_folderpath;

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerStockMgr();
	virtual ~FinanceAnalyzerStockMgr();

	virtual unsigned short initialize();
	// virtual unsigned short search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

	virtual unsigned short get_stock_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& price_support_resistance_string, const char* stock_price_support_resistance_folderpath=NULL, bool show_detail=false, const char* stock_price_support_resistance_time_filter=NULL, const char* price_support_resistance_volume_filter=NULL);
};
///////////////////////////////////////////////////////////////////////////////////

#endif
