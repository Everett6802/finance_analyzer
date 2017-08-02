#ifndef FINANCE_ANALYZER_STOCK_SUPPORT_RESISTANCE_H
#define FINANCE_ANALYZER_STOCK_SUPPORT_RESISTANCE_H

#include <list>
#include <string>
#include "finance_analyzer_common.h"

enum CandleStickTimeUnit{CandleStick_Week, CandleStick_Day, CandleStick_30Min, CandleStickSize, CandleStick_None};
static const char* CONFIG_TIMEUNIT_STRING_PREFIX = "TimeUnit:";
static const int CONFIG_TIMEUNIT_STRING_PREFIX_LEN = strlen(CONFIG_TIMEUNIT_STRING_PREFIX);
static const char* CONFIG_WEEK_START = "TimeUnit:Week";
static const char* CONFIG_DAY_START = "TimeUnit:Day";
static const char* CONFIG_30MIN_START = "TimeUnit:30Min";

class FinanceAnalyzerStockSupportResistance;
class StockPriceRef;

///////////////////////////////////////////////////////////////////////////////////

#define TIME_STRING_SIZE 16

class StockCandleStick
{
	friend class FinanceAnalyzerStockSupportResistance;
	friend class StockPriceRef;
	static const int CANDLE_STICK_DESCRIPTION_SIZE;
private:
	char time_string[TIME_STRING_SIZE];
	// float open_price;
	float highest_price;
	float lowest_price;
	// float close_price;
	long volume;
	char* candle_stick_description;

	StockCandleStick(
		const char* stock_time_string,
		// float stock_open_price, 
		float stock_highest_price, 
		float stock_lowest_price, 
		// float stock_close_price, 
		long stock_volume
	);
	~StockCandleStick();
	const char* get_candle_stick_description();	
};
typedef StockCandleStick* PSTOCK_CANDLE_STICK;

///////////////////////////////////////////////////////////////////////////////////
class StockPriceRef
{
	friend class FinanceAnalyzerStockSupportResistance;
private:
	float* price;
	PSTOCK_CANDLE_STICK candle_stick;

	StockPriceRef(float* stock_price, PSTOCK_CANDLE_STICK stock_candle_stick);
	~StockPriceRef();
public:
	const float* get_price()const;
};
typedef StockPriceRef* PSTOCK_PRICE_REF;
bool candle_stick_support_compare (const PSTOCK_PRICE_REF a, const PSTOCK_PRICE_REF b);
bool candle_stick_resistance_compare (const PSTOCK_PRICE_REF a, const PSTOCK_PRICE_REF b);

///////////////////////////////////////////////////////////////////////////////////

typedef std::list<PSTOCK_CANDLE_STICK> StockCandleStickList;
typedef std::list<PSTOCK_PRICE_REF> StockPriceRefList;

class FinanceAnalyzerStockSupportResistance
{
// Define candle stick time unit related member variables
	
	static const char* CONFIG_CANDLE_STICK_START[];
	static const int CONFIG_CANDLE_STICK_ENTRY_ELEMENT_COUNT;
	DECLARE_MSG_DUMPER()

private:
	StockCandleStickList candle_stick_list_array[CandleStickSize];
	StockPriceRefList support_price_ref_list_array[CandleStickSize];
	StockPriceRefList resistance_price_ref_list_array[CandleStickSize];

	bool init;
	float close_price;

	unsigned short update_data_from_config(const char* stock_critical_candle_stick_filepath);
	unsigned short find_support_and_resistance(CandleStickTimeUnit candle_stick_time_unit);
	unsigned short get_price_list(const StockPriceRefList& price_ref_list, std::list<float>& stock_price_list)const;
	unsigned short get_price_string(const StockPriceRefList& price_ref_list, std::string& stock_price_string, bool show_detail=false)const;

public:
	FinanceAnalyzerStockSupportResistance();
	~FinanceAnalyzerStockSupportResistance();

	unsigned short initialize(const char* stock_critical_candle_stick_filepath, float stock_close_price);
	unsigned short get_support_price_list(CandleStickTimeUnit candle_stick_time_unit, std::list<float>& stock_support_price_list)const;
	unsigned short get_resistance_price_list(CandleStickTimeUnit candle_stick_time_unit, std::list<float>& stock_resistance_price_list)const;
	unsigned short get_support_price_string(CandleStickTimeUnit candle_stick_time_unit, std::string& stock_support_price_string, bool show_detail=false)const;
	unsigned short get_resistance_price_string(CandleStickTimeUnit candle_stick_time_unit, std::string& stock_resistance_price_string, bool show_detail=false)const;
	unsigned short get_support_resistance_price_full_string(std::string& stock_support_resistance_price_string, bool show_detail=false)const;
};
typedef FinanceAnalyzerStockSupportResistance* PFINANCE_ANALYZER_STOCK_SUPPORT_RESISTANCE;

///////////////////////////////////////////////////////////////////////////////////

#endif
