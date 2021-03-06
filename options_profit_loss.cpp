#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <stdexcept>
#include <string>
#include "options_profit_loss.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////////

const int StockCandleStick::CANDLE_STICK_DESCRIPTION_SIZE = 64;

StockCandleStick::StockCandleStick(
	const char* stock_time_string, 
	// float stock_open_price, 
	float stock_lowest_price, 
	float stock_highest_price, 
	// float stock_close_price, 
	long stock_volume
	) :
	// open_price(stock_open_price), 
	lowest_price(stock_lowest_price), 
	highest_price(stock_highest_price), 
	// close_price(stock_close_price), 
	volume(stock_volume),
	candle_stick_description(NULL)
{
	assert(stock_time_string != NULL && "stock_time_string should NOT be NULL");
	memcpy(time_string, stock_time_string, strlen(stock_time_string) + 1);
}

StockCandleStick::~StockCandleStick()
{
	if (candle_stick_description != NULL)
	{
		delete[] candle_stick_description;
		candle_stick_description = NULL;
	}
}

const char* StockCandleStick::to_string()
{
	if (candle_stick_description == NULL)
	{
		candle_stick_description = new char[CANDLE_STICK_DESCRIPTION_SIZE];
		if (candle_stick_description == NULL)
			throw bad_alloc();
		// snprintf(candle_stick_description, CANDLE_STICK_DESCRIPTION_SIZE, "%s O:%.2f L:%.2f H:%.2f C:%.2f V:%ld", time_string, open_price, lowest_price, highest_price, close_price, volume);
		snprintf(candle_stick_description, CANDLE_STICK_DESCRIPTION_SIZE, "%s L:%.2f H:%.2f V:%ld", time_string, lowest_price, highest_price, volume);
	}
	return candle_stick_description;
}

///////////////////////////////////////////////////////////////////////////////////


StockPriceRef::StockPriceRef(float stock_price, PSTOCK_CANDLE_STICK stock_candle_stick) :
	price(stock_price),
	candle_stick(stock_candle_stick)
{

}

StockPriceRef::~StockPriceRef()
{
	candle_stick = NULL;
	// price = NULL;
}

float StockPriceRef::get_price()const
{
	// assert(price != NULL && "price should NOT be NULL");
	return price;
}

bool candle_stick_support_compare (const PSTOCK_PRICE_REF a, const PSTOCK_PRICE_REF b)
{
	return a->get_price() > b->get_price();
}

bool candle_stick_resistance_compare (const PSTOCK_PRICE_REF a, const PSTOCK_PRICE_REF b)
{
	return a->get_price() < b->get_price();
}

///////////////////////////////////////////////////////////////////////////////////

const char* StockSupportResistance::CONFIG_TIMEUNIT_STRING_PREFIX = "TimeUnit:";
const int StockSupportResistance::CONFIG_TIMEUNIT_STRING_PREFIX_LEN = strlen(CONFIG_TIMEUNIT_STRING_PREFIX);
const char* StockSupportResistance::CONFIG_WEEK_START = "TimeUnit:Week";
const char* StockSupportResistance::CONFIG_DAY_START = "TimeUnit:Day";
const char* StockSupportResistance::CONFIG_30MIN_START = "TimeUnit:30Min";
const char* StockSupportResistance::CONFIG_CANDLE_STICK_START[] = {CONFIG_WEEK_START, CONFIG_DAY_START, CONFIG_30MIN_START};
const int StockSupportResistance::CONFIG_CANDLE_STICK_ENTRY_ELEMENT_COUNT = 4;
const int StockSupportResistance::DEF_PRICE_LIMIT_PERCENTAGE = 10;

float StockSupportResistance::get_price_tick_reciprocal(float stock_close_price)
{
/*
price     unit
0.01~10   0.01 
10~50     0.05 
50~100    0.1 
100~500   0.5 
500~1000  1 
1000+     5
*/
	if (stock_close_price < 10.0)
		return 100.0;
	else if (stock_close_price >= 10.0 && stock_close_price < 50.0)
		return 20.0;
	else if (stock_close_price >= 50.0 && stock_close_price < 100.0)
		return 10.0;
	else if (stock_close_price >= 100.0 && stock_close_price < 500.0)
		return 2.0;
	else if (stock_close_price >= 100.0 && stock_close_price < 500.0)
		return 1.0;
	else if (stock_close_price >= 1000.0)
		return 0.5;
	else
	{
		static const int ERRMSG_BUF_SIZE = 32;
		static char errmsg[ERRMSG_BUF_SIZE];
		snprintf(errmsg, ERRMSG_BUF_SIZE, "The tick of the price [%.2f] is NOT defined", stock_close_price);
		throw invalid_argument(string(errmsg));		
	}
}

float StockSupportResistance::calculate_highest_price_limit(float stock_close_price, float price_limit_ratio)
{
	float highest_price_limit_tmp = stock_close_price * (1 + price_limit_ratio);
	float price_tick_reciprocal = get_price_tick_reciprocal(stock_close_price);
	return floor(highest_price_limit_tmp * price_tick_reciprocal) / price_tick_reciprocal;
}

float StockSupportResistance::calculate_lowest_price_limit(float stock_close_price, float price_limit_ratio)
{
	float lowest_price_limit_tmp = stock_close_price * (1 - price_limit_ratio); 
	float price_tick_reciprocal = get_price_tick_reciprocal(stock_close_price);
	return ceil(lowest_price_limit_tmp * price_tick_reciprocal) / price_tick_reciprocal;
}

unsigned short StockSupportResistance::check_time_string_format(CandleStickTimeUnit time_unit, const char* time_string)
{
	assert(time_string != NULL && "time_string should NOT be NULL");
	static unsigned int TIME_STRING_FORMAT_LEN_WEEK = 7;
	static unsigned int TIME_STRING_FORMAT_LEN_DAY = 6;
	static unsigned int TIME_STRING_FORMAT_LEN_30MIN = 10;
	unsigned short ret = RET_SUCCESS;
	switch (time_unit)
	{
		case CandleStick_Week:
		{
			if (strlen(time_string) != TIME_STRING_FORMAT_LEN_WEEK || time_string[0] != 'w')
				return RET_FAILURE_INVALID_ARGUMENT;
			ret = check_string_is_digit(&time_string[1], TIME_STRING_FORMAT_LEN_WEEK - 1);
			if (CHECK_FAILURE(ret))
				return ret;
		}
		break;
		case CandleStick_Day:
		{
			if (strlen(time_string) != TIME_STRING_FORMAT_LEN_DAY)
				return RET_FAILURE_INVALID_ARGUMENT;
			ret = check_string_is_digit(time_string, TIME_STRING_FORMAT_LEN_DAY);
			if (CHECK_FAILURE(ret))
				return ret;
		}
		break;
		case CandleStick_30Min:
		{
			if (strlen(time_string) != TIME_STRING_FORMAT_LEN_30MIN)
				return RET_FAILURE_INVALID_ARGUMENT;
			ret = check_string_is_digit(time_string, TIME_STRING_FORMAT_LEN_30MIN);
			if (CHECK_FAILURE(ret))
				return ret;
		}
		break;
		default:
			throw invalid_argument(string("Unknown Candle Stick time unit"));
			break;
	}
	return RET_SUCCESS;	
}

unsigned short StockSupportResistance::convert_time_string2timeval(CandleStickTimeUnit time_unit, const char* time_string, timeval& time_val)
{
	assert(time_string != NULL && "time_string should NOT be NULL");
	static const char* TIME_STRING_FORMAT_WEEK = "w%y%m%d";
	static const char* TIME_STRING_FORMAT_DAY = "%y%m%d";
	static const char* TIME_STRING_FORMAT_30MIN = "%y%m%d%H%M";
	static const char* const TIME_STRING_FORMAT_ARRAY[] = {TIME_STRING_FORMAT_WEEK, TIME_STRING_FORMAT_DAY, TIME_STRING_FORMAT_30MIN};
	unsigned short ret = check_time_string_format(time_unit, time_string);
	if (CHECK_FAILURE(ret))
		return ret;
	const char* time_string_format = TIME_STRING_FORMAT_ARRAY[time_unit];	
	struct tm tm_tmp;
	strptime(time_string, time_string_format, &tm_tmp);
	// printf("time unit: %d, time string: %s\n", time_unit, time_string);
	// switch (time_unit)
	// {
	// 	case CandleStick_Week:
	// 		printf("w%04d/%02d/%02d\n", tm_tmp.tm_year + 1900, tm_tmp.tm_mon + 1, tm_tmp.tm_mday);
	// 		break;
	// 	case CandleStick_Day:
	// 		printf("%04d/%02d/%02d\n", tm_tmp.tm_year + 1900, tm_tmp.tm_mon + 1, tm_tmp.tm_mday);
	// 		break;
	// 	case CandleStick_30Min:
	// 		printf("%04d/%02d/%02d %02d:%02d\n", tm_tmp.tm_year + 1900, tm_tmp.tm_mon + 1, tm_tmp.tm_mday, tm_tmp.tm_hour, tm_tmp.tm_min);
	// 		break;
	// 	default:
	// 		throw invalid_argument(string("Unknown Candle Stick time unit"));
	// 		break;
	// }
	time_t t_tmp = mktime(&tm_tmp); 
	// printf("time_t: %d\n", t_tmp);
	time_val.tv_sec = t_tmp;
	time_val.tv_usec = 0;
	return RET_SUCCESS;
}

StockSupportResistance::StockSupportResistance() :
	init(false),
	close_price(0.0),
	lowest_price_limit(0.0),
	highest_price_limit(0.0),
	limit_percentage(DEF_PRICE_LIMIT_PERCENTAGE)
{
	IMPLEMENT_MSG_DUMPER()
}

StockSupportResistance::~StockSupportResistance()
{
	for (int i = 0 ; i < CandleStickSize ; i++)
	{
		{
			StockPriceRefList& resistance_price_ref_list = resistance_price_ref_list_array[i];
			StockPriceRefList::iterator iter = resistance_price_ref_list.begin();
			while(iter != resistance_price_ref_list.end())
			{
				PSTOCK_PRICE_REF stock_price_ref = (PSTOCK_PRICE_REF)*iter;
				++iter;
				delete stock_price_ref;
			}
			resistance_price_ref_list.clear();
		}
		{
			StockPriceRefList& support_price_ref_list = support_price_ref_list_array[i];
			StockPriceRefList::iterator iter = support_price_ref_list.begin();
			while(iter != support_price_ref_list.end())
			{
				PSTOCK_PRICE_REF stock_price_ref = (PSTOCK_PRICE_REF)*iter;
				++iter;
				delete stock_price_ref;
			}
			support_price_ref_list.clear();
		}
		{
			StockCandleStickList& candle_stick_list = candle_stick_list_array[i];
			StockCandleStickList::iterator iter = candle_stick_list.begin();
			while(iter != candle_stick_list.end())
			{
				PSTOCK_CANDLE_STICK stock_candle_stick = (PSTOCK_CANDLE_STICK)*iter;
				++iter;
				delete stock_candle_stick;
			}
			candle_stick_list.clear();
		}
	}
	RELEASE_MSG_DUMPER()
}

unsigned short StockSupportResistance::update_data_from_config(const char* stock_critical_candle_stick_filepath, const char* time_filter_rule, const char* volume_filter_rule)
{
	unsigned short ret = RET_SUCCESS;
	WRITE_FORMAT_DEBUG("Read Candle Sticks from config: %s", stock_critical_candle_stick_filepath);
	list<string> line_list;
// Read each line from the config file
	ret = read_file_lines_ex(line_list, stock_critical_candle_stick_filepath, "r");
	if (CHECK_FAILURE(ret))
		return ret;
	CandleStickFilterOperator time_filter_operator = CandleStickFilter_None;
	CandleStickFilterOperator volume_filter_operator = CandleStickFilter_None;
// Setup the time filter if necessary
	if (time_filter_rule != NULL)
	{
		// switch(time_filter_rule[0])
		// {
		// 	case '<':
		// 	{
		// 		time_filter_operator = CandleStickFilter_LessThan;
		// 	}
		// 	break;
		// 	case '>':
		// 	{
		// 		time_filter_operator = CandleStickFilter_GreaterThan;
		// 	}
		// 	break;
		// 	default:
		// 	{
		// 		WRITE_FORMAT_ERROR("Incorrect time filter rules: %s", time_filter_rule);
		// 		return RET_FAILURE_INVALID_ARGUMENT;
		// 	}
		// 	break;
		// }
		time_filter_operator = CandleStickFilter_LessThan;
// CAUTION: time_filter_timeval can't read correct value in this place. I DON'T know why
		// ret = convert_time_string2timeval(CandleStick_Day, time_filter_rule, time_filter_timeval);
		// if (CHECK_FAILURE(ret))
		// 	return ret;
		// time_filter_exist = true;
	}
// Setup the volume filter if necessary
	if (volume_filter_rule != NULL)
	{
		volume_filter_operator = CandleStickFilter_LessThan;
	}
// Start to parse each line......
	CandleStickTimeUnit candle_stick_time_unit = CandleStick_None;
	static const int LINE_BUF_SIZE = 64;
	static char line_buf[LINE_BUF_SIZE];
	bool time_filter_exist = false;
	bool volume_filter_exist = false;
	timeval time_filter_timeval;
	long volume_filter_threshold;
	for(list<string>::iterator iter = line_list.begin() ; iter != line_list.end() ; ++iter)
	{
		string line = *iter;
// Find candle stick time unit
		if (strncmp(line.c_str(), CONFIG_TIMEUNIT_STRING_PREFIX, CONFIG_TIMEUNIT_STRING_PREFIX_LEN) == 0)
		{
			bool match = false;
			for (int i = 0 ; i < CandleStickSize ; i++)
			{
				if (strcmp(line.c_str(), CONFIG_CANDLE_STICK_START[i]) == 0)
				{
					candle_stick_time_unit = (CandleStickTimeUnit)i;
					match = true;
					break;
				}
			}
			if (!match)
			{
				WRITE_FORMAT_ERROR("Unknown CandleStickTimeUnit: %s", line.c_str());
				return RET_FAILURE_INCORRECT_CONFIG;
			}
			continue;
		}
		if (candle_stick_time_unit == CandleStick_None)
		{
			WRITE_ERROR("The candle_stick_time_unit variable should NOT be CandleStick_None");
			return RET_FAILURE_INCORRECT_CONFIG;
		}
// Parse each element in every candle stick entry
		memcpy(line_buf, line.c_str(), line.size() + 1);
		char* line_string = line_buf;
		char* entry = NULL;
		int entry_element_count = 0;
		static const int CANDLE_STICK_ENTRY_BUF_SIZE = 16;
		static char candle_stick_entry_element[CONFIG_CANDLE_STICK_ENTRY_ELEMENT_COUNT][CANDLE_STICK_ENTRY_BUF_SIZE];
		while ((entry = strtok(line_string, " ")) != NULL)
		{
			memset(candle_stick_entry_element[entry_element_count], 0x0, sizeof(char) * CANDLE_STICK_ENTRY_BUF_SIZE);
			memcpy(candle_stick_entry_element[entry_element_count], entry, strlen(entry));
			entry_element_count++;
			if (line_string != NULL)
				line_string = NULL;
		}
		if (entry_element_count != CONFIG_CANDLE_STICK_ENTRY_ELEMENT_COUNT)
		{
			WRITE_FORMAT_ERROR("Incorrect Candle Stick Entry[%s] Element Count, expected: %d, actual: %d", line_string, CONFIG_CANDLE_STICK_ENTRY_ELEMENT_COUNT, entry_element_count);
			return RET_FAILURE_INCORRECT_CONFIG;
		}
// Filter the data if necessary
// Filter the data from date
		if (time_filter_operator != CandleStickFilter_None)
		{
			if (!time_filter_exist)
			{
				ret = convert_time_string2timeval(CandleStick_Day, time_filter_rule, time_filter_timeval);
				if (CHECK_FAILURE(ret))
					return ret;
				time_filter_exist = true;
			}
			timeval candle_stick_timeval;
			ret = convert_time_string2timeval(candle_stick_time_unit, candle_stick_entry_element[CandleStickElement_Time], candle_stick_timeval);
			if (CHECK_FAILURE(ret))
				return ret;
			if (time_filter_operator == CandleStickFilter_LessThan) 
			{
				if (timercmp(&candle_stick_timeval, &time_filter_timeval, <))
					continue;
			}
			else if (time_filter_operator == CandleStickFilter_GreaterThan)
			{
				if (timercmp(&candle_stick_timeval, &time_filter_timeval, >))
					continue;
			}
		}
// Filter the data from volume
		if (volume_filter_operator != CandleStickFilter_None)
		{
			if (!volume_filter_exist)
			{
				volume_filter_threshold = atol(volume_filter_rule);
				volume_filter_exist = true;
			}
			long volume = atol(candle_stick_entry_element[CandleStickElement_Volume]);
			if (volume_filter_operator == CandleStickFilter_LessThan) 
			{
				if (volume < volume_filter_threshold)
					continue;
			}
			else if (volume_filter_operator == CandleStickFilter_GreaterThan)
			{
				if (volume > volume_filter_threshold)
					continue;
			}
		}
// Allocate the memory to keep track of the entry
		PSTOCK_CANDLE_STICK stock_candle_stick = new StockCandleStick(
			candle_stick_entry_element[CandleStickElement_Time],
			atof(candle_stick_entry_element[1]),
			atof(candle_stick_entry_element[2]),
			atol(candle_stick_entry_element[3])
		);
		if (stock_candle_stick == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: stock_candle_stick");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
		(candle_stick_list_array[candle_stick_time_unit]).push_back(stock_candle_stick);	
	}
	return ret;
}

unsigned short StockSupportResistance::find_support_and_resistance(CandleStickTimeUnit candle_stick_time_unit)
{
	WRITE_FORMAT_DEBUG("Calcuate the support and resistance from: %s", CONFIG_CANDLE_STICK_START[candle_stick_time_unit]);
	const StockCandleStickList& candle_stick_list = candle_stick_list_array[candle_stick_time_unit];
	StockPriceRefList& support_price_ref_list = support_price_ref_list_array[candle_stick_time_unit];
	StockPriceRefList& resistance_price_ref_list = resistance_price_ref_list_array[candle_stick_time_unit];
	StockCandleStickList::const_iterator iter = candle_stick_list.begin();
	while (iter != candle_stick_list.end())
	{
		PSTOCK_CANDLE_STICK stock_candle_stick = (PSTOCK_CANDLE_STICK)*iter;
		assert(stock_candle_stick != NULL && "stock_candle_stick should NOT be NULL");
// Check the highest price is the support or resistance 
		PSTOCK_PRICE_REF stock_price_ref_highest = new StockPriceRef(stock_candle_stick->highest_price, stock_candle_stick);
		if (stock_price_ref_highest == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: stock_price_ref_highest");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
		if (close_price >= stock_price_ref_highest->get_price())
		{
			bool can_add = ((limit_percentage != 0 && stock_price_ref_highest->get_price() <= lowest_price_limit) ? false : true);
			if (can_add)
				support_price_ref_list.push_back(stock_price_ref_highest);
		}
		else
		{
			bool can_add = ((limit_percentage != 0 && stock_price_ref_highest->get_price() >= highest_price_limit) ? false : true);
			if (can_add)
				resistance_price_ref_list.push_back(stock_price_ref_highest);
		}
// Check the lowest price is the support or resistance 
		PSTOCK_PRICE_REF stock_price_ref_lowest = new StockPriceRef(stock_candle_stick->lowest_price, stock_candle_stick);
		if (stock_price_ref_lowest == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: stock_price_ref_lowest");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
		if (close_price >= stock_price_ref_lowest->get_price())
		{
			bool can_add = ((limit_percentage != 0 && stock_price_ref_lowest->get_price() <= lowest_price_limit) ? false : true);
			if (can_add)
				support_price_ref_list.push_back(stock_price_ref_lowest);
		}
		else
		{
			bool can_add = ((limit_percentage != 0 && stock_price_ref_lowest->get_price() >= highest_price_limit) ? false : true);
			if (can_add)
				resistance_price_ref_list.push_back(stock_price_ref_lowest);
		}
		++iter;
	}
	support_price_ref_list.sort(candle_stick_support_compare);
	resistance_price_ref_list.sort(candle_stick_resistance_compare);
	return RET_SUCCESS;
}

unsigned short StockSupportResistance::initialize(const char* stock_critical_candle_stick_filepath, float stock_close_price, const char* time_filter_rule, const char* volume_filter_rule)
{
	if (init)
	{
		WRITE_ERROR("The StockSupportResistance object is Initialized !!!");
		return RET_FAILURE_INCORRECT_OPERATION;	
	}
// Modify to the absolute filepath if necessary
	assert(stock_critical_candle_stick_filepath != NULL && "stock_critical_candle_stick_filepath should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	char* absolute_filepath = NULL;
	if (stock_critical_candle_stick_filepath[0] == '~')
	{
		if (stock_critical_candle_stick_filepath[1] != '/')
		{
			WRITE_FORMAT_ERROR("The filepath[%s] is NOT correct format", stock_critical_candle_stick_filepath);
			ret = RET_FAILURE_INVALID_ARGUMENT;
			goto OUT;
		}
		ret = get_absolute_filepath_from_username(&stock_critical_candle_stick_filepath[2], &absolute_filepath);
		if (CHECK_FAILURE(ret))
		{
			WRITE_FORMAT_ERROR("Fail to find the absolute filepath from %s, due to: %s", stock_critical_candle_stick_filepath, get_ret_description(ret));
			goto OUT;
		}
		WRITE_FORMAT_DEBUG("Find the absolute filepath: %s", absolute_filepath);
		stock_critical_candle_stick_filepath = absolute_filepath;
	}
// Read config
	ret = update_data_from_config(stock_critical_candle_stick_filepath, time_filter_rule, volume_filter_rule);
	if (CHECK_FAILURE(ret))
		goto OUT;
	close_price = stock_close_price;
// Calcuate the price limit if necessary
	if (limit_percentage != 0)
	{
		float price_limit_ratio = limit_percentage / 100.0;
		lowest_price_limit = calculate_lowest_price_limit(close_price, price_limit_ratio);
		highest_price_limit = calculate_highest_price_limit(close_price, price_limit_ratio);
		WRITE_FORMAT_DEBUG("Calucate the price[%.2f] limit, LOW: %.2f, HIGH: %.2f\n", close_price, lowest_price_limit, highest_price_limit);
	}
// Initialize the data structure
	for (int i = 0 ; i < CandleStickSize ; i++)
	{
		ret = find_support_and_resistance((CandleStickTimeUnit)i);
		if (CHECK_FAILURE(ret))
			goto OUT;
	}
OUT:
	if (absolute_filepath != NULL)
	{
		delete[] absolute_filepath;
		absolute_filepath = NULL;
	}
	if (CHECK_SUCCESS(ret))
	{
		init = true;
	}
	return ret;
}

unsigned short StockSupportResistance::set_price_limit_percentage(int price_limit_percentage)
{
	if (init)
	{
		WRITE_ERROR("The StockSupportResistance object is Initialized !!!");
		return RET_FAILURE_INCORRECT_OPERATION;	
	}
	if (price_limit_percentage < 0 || price_limit_percentage > 100)
	{
		WRITE_ERROR("Price Limit Percentage should be in the range [0, 100]");
		return RET_FAILURE_INVALID_ARGUMENT;		
	}
	limit_percentage = price_limit_percentage;
	return RET_SUCCESS;
}

unsigned short StockSupportResistance::get_price_limit_percentage(int& price_limit_percentage)const
{
	if (!init)
	{
		WRITE_ERROR("The StockSupportResistance object is Not initialized !!!");
		return RET_FAILURE_INCORRECT_OPERATION;	
	}
	price_limit_percentage = limit_percentage;
	return RET_SUCCESS;
}

unsigned short StockSupportResistance::get_price_list(const StockPriceRefList& price_ref_list, std::list<float>& stock_price_list)const
{
	if (!init)
	{
		WRITE_ERROR("The StockSupportResistance object is Not initialized !!!");
		return RET_FAILURE_INCORRECT_OPERATION;	
	}
	StockPriceRefList::const_iterator iter = price_ref_list.begin();
	while (iter != price_ref_list.end())
	{
		float price = *(float*)*iter;
		stock_price_list.push_back(price);
		++iter;
	}
	return RET_SUCCESS;
}

unsigned short StockSupportResistance::get_support_price_list(CandleStickTimeUnit candle_stick_time_unit, std::list<float>& stock_support_price_list)const
{
	return get_price_list(support_price_ref_list_array[candle_stick_time_unit], stock_support_price_list);
}

unsigned short StockSupportResistance::get_resistance_price_list(CandleStickTimeUnit candle_stick_time_unit, std::list<float>& stock_resistance_price_list)const
{
	return get_price_list(resistance_price_ref_list_array[candle_stick_time_unit], stock_resistance_price_list);;
}

unsigned short StockSupportResistance::get_price_string(const StockPriceRefList& price_ref_list, std::string& stock_price_string, bool show_detail)const
{
	if (!init)
	{
		WRITE_ERROR("The StockSupportResistance object is Not initialized !!!");
		return RET_FAILURE_INCORRECT_OPERATION;	
	}
	static const int BUF_SIZE = 128;
	static char buf[BUF_SIZE];
	StockPriceRefList::const_iterator iter = price_ref_list.begin();
	while (iter != price_ref_list.end())
	{
		PSTOCK_PRICE_REF stock_price_ref = (PSTOCK_PRICE_REF)*iter;
		// assert(stock_price_ref->price != NULL && "stock_price_ref.price should NOT be NULL");
		assert(stock_price_ref->candle_stick != NULL && "stock_price_ref.candle_stick should NOT be NULL");
		if (show_detail)
			snprintf(buf, BUF_SIZE, "\n%.2f[%s]->", stock_price_ref->get_price(), stock_price_ref->candle_stick->to_string());
		else
			snprintf(buf, BUF_SIZE, "%.2f->", stock_price_ref->get_price());
		stock_price_string += buf;
		++iter;
	}
	if (!stock_price_string.empty())
	{
// Remove the last "->"
		int stock_price_string_size = stock_price_string.size();
		stock_price_string.resize(stock_price_string_size - 2);
// Caution: Can NOT do in this way !!!
		// stock_price_string[stock_price_string_size - 2] = '\0'; 
	}
	else 
		stock_price_string = "N/A";
	return RET_SUCCESS;
}

unsigned short StockSupportResistance::get_support_price_string(CandleStickTimeUnit candle_stick_time_unit, std::string& stock_support_price_string, bool show_detail)const
{
	return get_price_string(support_price_ref_list_array[candle_stick_time_unit], stock_support_price_string, show_detail);
}

unsigned short StockSupportResistance::get_resistance_price_string(CandleStickTimeUnit candle_stick_time_unit, string& stock_resistance_price_string, bool show_detail)const
{
	return get_price_string(resistance_price_ref_list_array[candle_stick_time_unit], stock_resistance_price_string, show_detail);
}

unsigned short StockSupportResistance::get_support_resistance_price_full_string(std::string& stock_support_resistance_price_string, bool show_detail)const
{
	unsigned short ret = RET_SUCCESS;
	if (limit_percentage != 0)
	{
		static const int LIMIT_PERCENTAGE_BUF_SIZE = 64;
		static char limit_percentage_buf[LIMIT_PERCENTAGE_BUF_SIZE];
		snprintf(limit_percentage_buf, LIMIT_PERCENTAGE_BUF_SIZE, "Price Limit, L: %.2f, H: %.2f\n", lowest_price_limit, highest_price_limit);
		stock_support_resistance_price_string += limit_percentage_buf;
	}
	// string stock_support_resistance_price_string;
	for (int i = 0 ; i < CandleStickSize ; i++)
	{
// Get Time unit title
		stock_support_resistance_price_string += CONFIG_CANDLE_STICK_START[i];
		stock_support_resistance_price_string += "\n";
// Get support price
		stock_support_resistance_price_string += "Support: ";
		string support_price_string;
		ret = get_support_price_string((CandleStickTimeUnit)i, support_price_string, show_detail);
		if (CHECK_FAILURE(ret))
			return ret;
		stock_support_resistance_price_string += support_price_string;
		stock_support_resistance_price_string += "\n";
// Get resistance price
		stock_support_resistance_price_string += "Resistance: ";
		string resistance_price_string;
		ret = get_resistance_price_string((CandleStickTimeUnit)i, resistance_price_string, show_detail);
		if (CHECK_FAILURE(ret))
			return ret;
		stock_support_resistance_price_string += resistance_price_string;
		stock_support_resistance_price_string += "\n";
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
