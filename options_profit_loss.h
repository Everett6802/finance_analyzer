#ifndef OPTIONS_PROFIT_LOSS_H
#define OPTIONS_PROFIT_LOSS_H

#include "common.h"
#include "options_profit_loss_def.h"


///////////////////////////////////////////////////////////////////////////////////
class StockPriceRef
{
	friend class StockSupportResistance;
private:
	float price;
	PSTOCK_CANDLE_STICK candle_stick;

	StockPriceRef(float stock_price, PSTOCK_CANDLE_STICK stock_candle_stick);
	~StockPriceRef();
public:
	float get_price()const;
};
typedef StockPriceRef* PSTOCK_PRICE_REF;
bool candle_stick_support_compare (const PSTOCK_PRICE_REF a, const PSTOCK_PRICE_REF b);
bool candle_stick_resistance_compare (const PSTOCK_PRICE_REF a, const PSTOCK_PRICE_REF b);

///////////////////////////////////////////////////////////////////////////////////

typedef std::list<PSTOCK_CANDLE_STICK> StockCandleStickList;
typedef std::list<PSTOCK_PRICE_REF> StockPriceRefList;

class StockSupportResistance
{
// Define candle stick time unit related member variables
	static const char* CONFIG_TIMEUNIT_STRING_PREFIX;
	static const int CONFIG_TIMEUNIT_STRING_PREFIX_LEN;
	static const char* CONFIG_WEEK_START;
	static const char* CONFIG_DAY_START;
	static const char* CONFIG_30MIN_START;
	static const char* CONFIG_CANDLE_STICK_START[];
	static const int CONFIG_CANDLE_STICK_ENTRY_ELEMENT_COUNT;
	static const int DEF_PRICE_LIMIT_PERCENTAGE;
	DECLARE_MSG_DUMPER()

	static float get_price_tick_reciprocal(float stock_close_price);
	static float calculate_highest_price_limit(float stock_close_price, float price_limit_ratio);
	static float calculate_lowest_price_limit(float stock_close_price, float price_limit_ratio);
	static unsigned short check_time_string_format(CandleStickTimeUnit time_unit, const char* time_string);
	static unsigned short convert_time_string2timeval(CandleStickTimeUnit time_unit, const char* time_string, timeval& time_val);

private:
	StockCandleStickList candle_stick_list_array[CandleStickSize];
	StockPriceRefList support_price_ref_list_array[CandleStickSize];
	StockPriceRefList resistance_price_ref_list_array[CandleStickSize];

	bool init;
	float close_price;
	float lowest_price_limit;
	float highest_price_limit;
	int limit_percentage;

	unsigned short update_data_from_config(const char* stock_critical_candle_stick_filepath, const char* time_filter_rule=NULL, const char* volume_filter_rule=NULL);
	unsigned short find_support_and_resistance(CandleStickTimeUnit candle_stick_time_unit);
	unsigned short get_price_list(const StockPriceRefList& price_ref_list, std::list<float>& stock_price_list)const;
	unsigned short get_price_string(const StockPriceRefList& price_ref_list, std::string& stock_price_string, bool show_detail=false)const;

public:
	StockSupportResistance();
	~StockSupportResistance();

	unsigned short initialize(const char* stock_critical_candle_stick_filepath, float stock_close_price, const char* time_filter_rule=NULL, const char* volume_filter_rule=NULL);
// Disable the price limit when price_limit_percentage = 0
	unsigned short set_price_limit_percentage(int price_limit_percentage);
	unsigned short get_price_limit_percentage(int& price_limit_percentage)const;
	unsigned short get_support_price_list(CandleStickTimeUnit candle_stick_time_unit, std::list<float>& stock_support_price_list)const;
	unsigned short get_resistance_price_list(CandleStickTimeUnit candle_stick_time_unit, std::list<float>& stock_resistance_price_list)const;
	unsigned short get_support_price_string(CandleStickTimeUnit candle_stick_time_unit, std::string& stock_support_price_string, bool show_detail=false)const;
	unsigned short get_resistance_price_string(CandleStickTimeUnit candle_stick_time_unit, std::string& stock_resistance_price_string, bool show_detail=false)const;
	unsigned short get_support_resistance_price_full_string(std::string& stock_support_resistance_price_string, bool show_detail=false)const;
};
typedef StockSupportResistance* PSTOCK_SUPPORT_RESISTANCE;

///////////////////////////////////////////////////////////////////////////////////

#endif
