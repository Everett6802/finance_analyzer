#include <assert.h>
#include <stdexcept>
#include "data_calculator.h"
#include "data_algorithm.h"
#include "data_output.h"


using namespace std;

static const int RES_INFO_LENGTH = 1024;
// static char res_info[RES_INFO_LENGTH];

///////////////////////////////////////////////////////////////////////////////////
// The Data Calculator Base Class
unsigned short DataCalculatorBase::generate_general_query_set(QuerySet& query_set)
{
/*
* 臺股指數及成交量
	成交金額(2), 發行量加權股價指數(4), 漲跌點數(5)
* 三大法人現貨買賣超
	自營商(自行買賣)_買賣差額(3), 自營商(避險)_買賣差額(6), 投信_買賣差額(9), 外資及陸資_買賣差額(12)
* 現貨融資融券餘額
	融資金額(仟元)_前日餘額(14), 融資金額(仟元)_今日餘額(15)
* 三大法人期貨和選擇權留倉淨額
	自營商_多空淨額_契約金額(6), 投信_多空淨額_契約金額(12), 外資_多空淨額_契約金額(18)
* 三大法人選擇權賣權買權比
	買賣權未平倉量比率(6)
* 三大法人選擇權買賣權留倉淨額
	買權_自營商_買方_口數(1), 買權_自營商_賣方_口數(3), 買權_外資_買方_口數(13), 買權_外資_賣方_口數(15), 賣權_自營商_買方_口數(19), 賣權_自營商_賣方_口數(21), 賣權_外資_買方_口數(31), 賣權_外資_賣方_口數(33)
* 十大交易人及特定法人期貨資訊
	臺股期貨_到期月份_買方_前十大交易人合計_部位數(3), 臺股期貨_到期月份_賣方_前十大交易人合計_部位數(7), 臺股期貨_所有契約_買方_前十大交易人合計_部位數(12), 臺股期貨_所有契約_賣方_前十大交易人合計_部位數(16)
 */
	unsigned short ret = RET_SUCCESS;
	ADD_QUERY(query_set, FinanceMethod_StockExchangeAndVolume, 2);
	ADD_QUERY(query_set, FinanceMethod_StockExchangeAndVolume, 4);
	ADD_QUERY(query_set, FinanceMethod_StockExchangeAndVolume, 5);
	ADD_QUERY(query_set, FinanceMethod_StockTop3LegalPersonsNetBuyOrSell, 3);
	ADD_QUERY(query_set, FinanceMethod_StockTop3LegalPersonsNetBuyOrSell, 6);
	ADD_QUERY(query_set, FinanceMethod_StockTop3LegalPersonsNetBuyOrSell, 9);
	ADD_QUERY(query_set, FinanceMethod_StockTop3LegalPersonsNetBuyOrSell, 12);
	ADD_QUERY(query_set, FinanceMethod_StockMarginTradingAndShortSelling, 14);
	ADD_QUERY(query_set, FinanceMethod_StockMarginTradingAndShortSelling, 15);
	ADD_QUERY(query_set, FinanceMethod_FutureAndOptionTop3LegalPersonsOpenInterest, 6);
	ADD_QUERY(query_set, FinanceMethod_FutureAndOptionTop3LegalPersonsOpenInterest, 12);
	ADD_QUERY(query_set, FinanceMethod_FutureAndOptionTop3LegalPersonsOpenInterest, 18);
	ADD_QUERY(query_set, FinanceMethod_OptionPutCallRatio, 6);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 1);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 3);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 13);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 15);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 19);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 21);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 31);
	ADD_QUERY(query_set, FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest, 33);
	ADD_QUERY(query_set, FinanceMethod_FutureTop10DealersAndLegalPersons, 3);
	ADD_QUERY(query_set, FinanceMethod_FutureTop10DealersAndLegalPersons, 7);
	ADD_QUERY(query_set, FinanceMethod_FutureTop10DealersAndLegalPersons, 12);
	ADD_QUERY(query_set, FinanceMethod_FutureTop10DealersAndLegalPersons, 16);
	return ret;
}

// const PQUERY_SET DataCalculatorBase::get_general_query_set()
// {
// 	static QuerySet query_set;
// 	// if (!query_set.is_add_query_done())
// 	// {
// 	// 	unsigned short ret = DataCalculatorBase::generate_general_query_set(query_set);
// 	// 	if (CHECK_FAILURE(ret))
// 	// 	{
// 	// 		const int BUF_SIZE = 256;
// 	// 		char buf[BUF_SIZE];
// 	// 		snprintf(buf, BUF_SIZE, "Fail to generate generate query set, due to: %s", get_ret_description(ret));
// 	// 		throw runtime_error(string(buf));
// 	// 	}
// 	// 	query_set.add_query_done();	
// 	// 	// WRITE_FORMAT_DEBUG("There are totally %d query items", query_set.get_size());
// 	// }
// 	return &query_set;
// }

DataCalculatorBase::DataCalculatorBase()
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_WORKDAY_CANLENDAR()
	// IMPLEMENT_DATABASE_TIME_RANGE()
}

DataCalculatorBase::~DataCalculatorBase()
{
	// RELEASE_DATABASE_TIME_RANGE()
	RELEASE_WORKDAY_CANLENDAR()
	RELEASE_MSG_DUMPER()
}

unsigned short DataCalculatorBase::initialize(STRING_LIST& src_email_address_list)
{
	unsigned short ret = RET_SUCCESS;
	ret = copy_email_adress_list(email_address_list);
	if (CHECK_FAILURE(ret))
		return ret;
	return ret;
}

unsigned short DataCalculatorBase::copy_email_adress_list(STRING_LIST& src_email_address_list)
{
	for (STRING_LIST::const_iterator iter = src_email_address_list.begin() ; iter != src_email_address_list.end() ; iter++)
	{
		string email_address = (string)*iter;
		email_address_list.push_back(email_address);
	}
	return RET_SUCCESS;
}

unsigned short DataCalculatorBase::show_result(string& result_str, int show_result_type, const char* data_description)const
{
	// assert(time_cfg != NULL && "time_cfg should NOT be NULL");
	unsigned short ret = RET_SUCCESS;

// Show result on the screen
	if (show_result_type & SHOW_RES_STDOUT)
	{
		if (!SHOW_CONSOLE)
		{
			WRITE_WARN("Disabled; No data will be shown on STDOUT/STDERR");
		}
		else
		{
// Write the data into STDOUT
			if (data_description != NULL)
			{
				string data_description_str = string(data_description) + string("\n");
				ret = direct_string_to_output_stream(data_description_str.c_str());
				if (CHECK_FAILURE(ret))
					return ret;
			}
			ret = direct_string_to_output_stream(result_str.c_str());
			if (CHECK_FAILURE(ret))
				return ret;
		}
	}
// Send the result by email
	if (show_result_type & SHOW_RES_EMAIL)
	{
		if (data_description == NULL)
		{
			static const char* DEF_EMAIL_TITLE = "Finance Calculator";
			data_description = DEF_EMAIL_TITLE;
		}
		for (STRING_LIST::const_iterator iter = email_address_list.begin() ; iter != email_address_list.end() ; iter++)
		{
			string email_address = (string)*iter;
			WRITE_FORMAT_DEBUG("Write daily data by email[%s] to %s", data_description, email_address.c_str());
			ret = send_email(data_description, email_address.c_str(), result_str.c_str());
			if (CHECK_FAILURE(ret))
				return ret;
		}
	}
// Write the data into file
	if (show_result_type & SHOW_RES_FILE)
	{
// Check the folder of keeping track of the result exist
		static bool check_result_folder_exist = false;
		if (!check_result_folder_exist)
		{
			ret = create_folder_in_project_if_not_exist(RESULT_FOLDER_NAME);
			if (CHECK_FAILURE(ret))
				return ret;
			check_result_folder_exist = true;
		}
		// char filename[32];
		// snprintf(filename, 32, DAILY_FINANCE_FILENAME_FORMAT, time_cfg->get_year(), time_cfg->get_month(), time_cfg->get_day());
		if (data_description == NULL)
		{
			static const char* DEF_FILENAME = "finance_statistics";
			data_description = DEF_FILENAME;
		}
		static char filepath[256];
		snprintf(filepath, 256, "%s/%s", RESULT_FOLDER_NAME, data_description);
		WRITE_FORMAT_DEBUG("Write daily data to file[%s]", filepath);
		ret = direct_string_to_output_stream(result_str.c_str(), filepath);
		if (CHECK_FAILURE(ret))
			return ret;
		PRINT("Check the result in file: %s\n", filepath);
	}
// Show result on syslog
	if (show_result_type & SHOW_RES_SYSLOG)
	{
// Write the data into syslog
		if (data_description != NULL)
			WRITE_FORMAT_INFO("*** %s ***\n%s", data_description, result_str.c_str());
		else
			WRITE_INFO(result_str.c_str());
	}

	return RET_SUCCESS;
}

// void DataCalculatorBase::enable_res_info(bool enable)
// {
// 	need_res_info = enable;
// }

// const char* DataCalculatorBase::get_last_res_info()const
// {
// 	return res_info;
// }

const char* DataCalculatorBase::get_description_head(const PRESULT_SET result_set, const PRESULT_SET_ACCESS_PARAM result_set_access_param, const PTIME_RANGE_CFG time_range_cfg)const
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(result_set_access_param != NULL && "result_set_access_param should NOT be NULL");

	static const int BUF_SIZE = 512;
	static char buf[BUF_SIZE];
	// assert(result_set != NULL && "result_set should NOT be NULL");
	int start_index = 0;
	int end_index = result_set->get_data_size();
	if (time_range_cfg != NULL)
	{
		if (time_range_cfg->get_start_time() != NULL)
		{
			start_index = result_set->get_date_array_element_index(time_range_cfg->get_start_time()->to_string());
			if (start_index == -1)
			{
				snprintf(buf, BUF_SIZE, "Fail to find the start index of the date: %s", time_range_cfg->get_start_time()->to_string());
				throw invalid_argument(string(buf));
			}
		}
		if (time_range_cfg->get_end_time() != NULL)
		{
			end_index = result_set->get_date_array_element_index(time_range_cfg->get_end_time()->to_string());
			if (end_index == -1)
			{
				snprintf(buf, BUF_SIZE, "Fail to find the end index of the date: %s", time_range_cfg->get_end_time()->to_string());
				throw invalid_argument(string(buf));
			}
		}
	}
	// int data_amount = end_index - start_index;
	// snprintf(buf, BUF_SIZE, "%s:%s %s:%s %d", 
	// 	FINANCE_METHOD_DESCRIPTION_LIST[result_set_access_param->get_finance_source_type()],
	// 	get_database_field_description(result_set_access_param->get_finance_source_type(), result_set_access_param->get_finance_field_no()),
	// 	result_set->get_date_array_element(start_index),
	// 	result_set->get_date_array_element(end_index),
	// 	data_amount
	// );
	return buf;
}

const char* DataCalculatorBase::get_no_data_description(int method_index, int field_index, const PTIME_RANGE_CFG time_range_cfg)const
{
	assert(time_range_cfg != NULL && "time_range_cfg should NOT be NULL");
	assert(time_range_cfg->get_start_time() != NULL && "time_range_cfg::start_time should NOT be NULL");
	assert(time_range_cfg->get_end_time() != NULL && "time_range_cfg::end_time should NOT be NULL");
	static const int BUF_SIZE = 512;
	static char buf[BUF_SIZE];

	// snprintf(buf, BUF_SIZE, "%s:%s %s:%s *** No Data in MySQL ***", 
	// 	FINANCE_METHOD_DESCRIPTION_LIST[method_index],
	// 	get_database_field_description(method_index, field_index),
	// 	time_range_cfg->get_start_time()->to_string(),
	// 	time_range_cfg->get_end_time()->to_string()
	// );
	return buf;
}

// unsigned short DataCalculatorBase::get_restricted_time_range(int method_index, int field_index, const PTIME_RANGE_CFG time_range_cfg, SmartPointer<TimeRangeCfg>& sp_restricted_time_range_cfg)const
// {
// 	set<int> method_index_set;
// 	// int method_index, field_index;
// 	// for (int index = 0 ; index < FinanceSourceSize ; index++)
// 	// {
// 	// 	const INT_DEQUE& query_field = (*sp_query_set.get_instance())[index];
// 	// 	if (!query_field.empty())
// 	// 	{
// 	// 		method_index_set.insert(index);
// 	// 		method_index = index;
// 	// 		field_index = query_field[0];
// 	// 		break;
// 	// 	}
// 	// }
// 	// assert(method_index_set.size() == 1 && "The size of method_index_set should be 1");
// 	method_index_set.insert(method_index);

// 	unsigned short ret = RET_SUCCESS;
// // Check the time range of this database
// 	// SmartPointer<TimeRangeCfg> sp_restricted_time_range_cfg;
// 	if (time_range_cfg != NULL)
// 	{
// 		sp_restricted_time_range_cfg.set_new(new TimeRangeCfg(*time_range_cfg));
// 	}
// 	else
// 	{
// 		ret = database_time_range->get_max_database_time_range(sp_restricted_time_range_cfg);
// 		if (CHECK_FAILURE(ret))
// 			return ret;
// 		WRITE_FORMAT_DEBUG("time_rage_cfg is NULL, use the max database time range: %s", sp_restricted_time_range_cfg->to_string());
// 	}
	
// 	ret = database_time_range->restrict_time_range(
// 		method_index_set, 
// 		sp_restricted_time_range_cfg.get_instance()
// 		);
// 	if (CHECK_FAILURE(ret))
// 		return ret;
// 	WRITE_FORMAT_DEBUG("The search time range: %s", sp_restricted_time_range_cfg->to_string());
// 	return ret;
// }

// unsigned short DataCalculatorBase::query_from_database(int method_index, int field_index, const SmartPointer<TimeRangeCfg>& sp_restricted_time_range_cfg, SmartPointer<ResultSetMap>& sp_result_set_map)const
// {
// 	unsigned short ret = RET_SUCCESS;
// 	SmartPointer<QuerySet> sp_query_set(new QuerySet());
// 	sp_query_set->add_query(method_index, field_index);
// 	sp_query_set->add_query_done();
// 	// SmartPointer<ResultSet> sp_result_set(new ResultSet());	
// // Query the data from MySQL
// 	ret = SqlReader::query_market(
// 		(const PTIME_RANGE_CFG)sp_restricted_time_range_cfg.get_const_instance(), 
// 		(const PQUERY_SET)sp_query_set.get_const_instance(), 
// 		// parent_obj->sql_reader, 
// 		sp_result_set_map.get_instance()
// 		);
// #ifdef DO_DEBUG
// // Show the result
// 	ret = sp_result_set->show_data();
// 	if (CHECK_FAILURE(ret))
// 		return ret;
// #endif
// 	return ret;
// }

unsigned short DataCalculatorBase::get_statistics_result(StatisticsMethod statistics_method, int method_index, int field_index, const PTIME_RANGE_CFG time_range_cfg, string& result_str)const
{
	// typedef unsigned short (DataCalculatorBase::*get_result_str_func_ptr)(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const;
	// static get_result_str_func_ptr get_result_str_func_array[] =
	// {
	// 	&DataCalculatorBase::get_range_value_result_str,
	// 	&DataCalculatorBase::get_average_result_str,
	// 	&DataCalculatorBase::get_standard_deviation_result_str
	// };

	// static const int BUF_SIZE = 1024;
	// static char buf[BUF_SIZE];

	// unsigned short ret = RET_SUCCESS;
	// int method_index = statistics_method - StatisticsFormula_Start;
// // Find the time range of this database
// 	SmartPointer<TimeRangeCfg> sp_restricted_time_range_cfg;
// 	ret = get_restricted_time_range(method_index, field_index, time_range_cfg, sp_restricted_time_range_cfg);
// 	if (CHECK_FAILURE(ret))
// 	{
// 		if (!FAILURE_IS_OUT_OF_RANGE(ret))
// 			return ret;
// 		const char* description = get_no_data_description(method_index, field_index, (const PTIME_RANGE_CFG)sp_restricted_time_range_cfg.get_const_instance());
// 		result_str = string(description);
// 	}
// 	else
// 	{
// // Query the data from MySQL
// 		SmartPointer<ResultSetMap> sp_result_set_map(new ResultSetMap());	
// 		ret = query_from_database(method_index, field_index, sp_restricted_time_range_cfg, sp_result_set_map);
// 		if (CHECK_FAILURE(ret))
// 			return ret;

// 		string new_str;
// 		// int finance_field_type_index = FINANCE_DATA_FIELD_TYPE_LIST[method_index][field_index];
// 		ret = (this->*(get_result_str_func_array[method_index]))(sp_result_set->get_array(method_index, field_index), new_str);
// 		if (CHECK_FAILURE(ret))
// 		{
// 			WRITE_FORMAT_ERROR("Fail to Calculate %s array[%d,%d] %s",
// 				FORMULA_STATSTICS_METHOD_DESCRIPTION[method_index], 
// 				method_index, 
// 				field_index, 
// 				FINANCE_FIELD_TYPE_DESCRIPTION[FINANCE_DATA_FIELD_TYPE_LIST[method_index][field_index]]
// 				);
// 			return ret;
// 		}

// 		SmartPointer<ResultSetAccessParam> sp_result_set_access_param(new ResultSetAccessParam((FinanceSourceType)method_index, field_index));
// 		const char* description_head = get_description_head(
// 			sp_result_set.get_instance(), 
// 			sp_result_set_access_param.get_instance(), 
// 			sp_restricted_time_range_cfg.get_instance()
// 			);
// 		snprintf(buf, BUF_SIZE, "%s, %s", description_head, new_str.c_str());

// 		result_str = string(buf);
// 	}

	return RET_SUCCESS;
}

unsigned short DataCalculatorBase::get_range_value_result_str(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const
{
	assert(data_array != NULL && "data_array should NOT be NULL");

	static const int STATISTICS_RESULT_DESCRIPTION_SIZE = 512;

	char statistics_result_description[STATISTICS_RESULT_DESCRIPTION_SIZE];
	unsigned short ret = RET_SUCCESS;
	switch(data_array->get_type())
	{
		case FinanceField_INT:
		{
			PFINANCE_INT_DATA_ARRAY int_data_array = (PFINANCE_INT_DATA_ARRAY)data_array;
			int min_value, max_value;
			ret = get_data_range(*int_data_array, min_value, max_value);
			if (CHECK_FAILURE(ret))
				return ret;
			snprintf(statistics_result_description, STATISTICS_RESULT_DESCRIPTION_SIZE, "min: %d, max: %d", min_value, max_value);
			result_str = string(statistics_result_description);
		}
		break;
		case FinanceField_LONG:
		{
			PFINANCE_LONG_DATA_ARRAY long_data_array = (PFINANCE_LONG_DATA_ARRAY)data_array;
			long min_value, max_value;
			ret = get_data_range(*long_data_array, min_value, max_value);
			if (CHECK_FAILURE(ret))
				return ret;
			snprintf(statistics_result_description, STATISTICS_RESULT_DESCRIPTION_SIZE, "min: %ld, max: %ld", min_value, max_value);
			result_str = string(statistics_result_description);
		}
		break;
		case FinanceField_FLOAT:
		{
			PFINANCE_FLOAT_DATA_ARRAY float_data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array;
			float min_value, max_value;
			ret = get_data_range(*float_data_array, min_value, max_value);
			if (CHECK_FAILURE(ret))
				return ret;
			snprintf(statistics_result_description, STATISTICS_RESULT_DESCRIPTION_SIZE, "min: %.2f, max: %.2f", min_value, max_value);
			result_str = string(statistics_result_description);
		}
		break;
		case FinanceField_DATE:
			WRITE_ERROR("The DATE field type is NOT supported");
			return RET_FAILURE_INVALID_ARGUMENT;
		default:
			WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array->get_type());
			return RET_FAILURE_INVALID_ARGUMENT;
	}

	return RET_SUCCESS;
}

unsigned short DataCalculatorBase::get_average_result_str(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const
{
	assert(data_array != NULL && "data_array should NOT be NULL");
	static const int STATISTICS_RESULT_DESCRIPTION_SIZE = 512;

	char statistics_result_description[STATISTICS_RESULT_DESCRIPTION_SIZE];
	float average_value = 0.0f;
	try
	{
		average_value = average(data_array);
	}
	catch (const std::invalid_argument& e) 
	{
		WRITE_FORMAT_ERROR("Fail to calculate average, due to: %s", e.what());
	  	return RET_FAILURE_INVALID_ARGUMENT;
  	}
	snprintf(statistics_result_description, STATISTICS_RESULT_DESCRIPTION_SIZE, "average: %.2f", average_value);
	result_str = string(statistics_result_description);
	return RET_SUCCESS;
}

unsigned short DataCalculatorBase::get_standard_deviation_result_str(const PFINANCE_DATA_ARRAY_BASE data_array, std::string& result_str)const
{
	assert(data_array != NULL && "data_array should NOT be NULL");
	static const int STATISTICS_RESULT_DESCRIPTION_SIZE = 512;

	char statistics_result_description[STATISTICS_RESULT_DESCRIPTION_SIZE];
	float standard_deviation_value = 0.0f;
	try
	{
		standard_deviation_value = standard_deviation(data_array);
	}
	catch (const std::invalid_argument& e) 
	{
		WRITE_FORMAT_ERROR("Fail to calculate standard deviation, due to: %s", e.what());
	  	return RET_FAILURE_INVALID_ARGUMENT;
  	}
	snprintf(statistics_result_description, STATISTICS_RESULT_DESCRIPTION_SIZE, "standard deviation: %.2f", standard_deviation_value);
	result_str = string(statistics_result_description);
	return RET_SUCCESS;
}

unsigned short DataCalculatorBase::calculate_statistics(StatisticsMethod statistics_method, const SmartPointer<TimeRangeCfg>& sp_time_range_cfg)const
{
	// static const char* DEF_DESCRIPTION_FORMAT = "***** %s Analysis Result *****";
	// static const int DESCRIPTION_SIZE = 256;
	// static char description[DESCRIPTION_SIZE];

	// int index = statistics_method - StatisticsFormula_Start;
	unsigned short ret = RET_SUCCESS;
	// const PQUERY_SET query_set = DataCalculatorBase::get_general_query_set();
	// string result_str = "";
	// PINT_SET method_index_set = query_set->get_method_index_set();
	// INT_SET_ITER iter = method_index_set->begin();
	// // for (int method_index = 0 ; method_index < FinanceSourceSize ; method_index++)
	// while (iter != method_index_set->end())
	// {
	// 	int method_index = *iter;
	// 	const INT_DEQUE& query_field = (*query_set)[method_index];
	// 	// if (query_field.empty())
	// 	// 	continue;
	// 	int query_field_size = query_field.size();
	// 	for (int i = 0 ; i < query_field_size ; i++)
	// 	{
	// 		int field_index = query_field[i];
	// 		WRITE_FORMAT_INFO("Calcuate the %s from data[%d, %d](%s)", FORMULA_STATSTICS_METHOD_DESCRIPTION[index], method_index, field_index, get_database_field_description(method_index, field_index));
	// 		string new_str;
	// 		ret = get_statistics_result(
	// 			statistics_method,
	// 			method_index, 
	// 			field_index, 
	// 			(const PTIME_RANGE_CFG)sp_time_range_cfg.get_const_instance(), 
	// 			new_str
	// 			);
	// 		if (CHECK_FAILURE(ret))
	// 			return ret;
	// 		result_str += (new_str + "\n");
	// 	}
	// 	iter++;
	// }
	// snprintf(description, DESCRIPTION_SIZE, DEF_DESCRIPTION_FORMAT, FORMULA_STATSTICS_METHOD_DESCRIPTION[index]);
	// ret = parent_obj->show_result(result_str, SHOW_RES_STDOUT, description);
	return ret;
}
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Data Calculator Class
MarketDataCalculator::MarketDataCalculator()
{
}

MarketDataCalculator::~MarketDataCalculator()
{
}
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Data Calculator Class
StockDataCalculator::StockDataCalculator() :
	stock_support_resistance_folderpath(DEFAULT_STOCK_SUPPORT_RESISTANCE_ROOT_FOLDERPATH)
{
}

StockDataCalculator::~StockDataCalculator()
{
}

unsigned short StockDataCalculator::set_stock_support_resistance_folderpath(const std::string& new_stock_support_resistance_folderpath)
{
	stock_support_resistance_folderpath = new_stock_support_resistance_folderpath;
	return RET_SUCCESS;
}

unsigned short StockDataCalculator::get_stock_support_resistance_string(const std::string& company_code_number, float stock_close_price, string& stock_support_resistance_string, bool show_detail, const char* time_filter_rule, const char* volume_filter_rule)
{
	static const int FILEPATH_BUF_SIZE = 256;
	static char filepath_buf[FILEPATH_BUF_SIZE];
	unsigned short ret = RET_SUCCESS;
	snprintf(filepath_buf, FILEPATH_BUF_SIZE, "%s/%s", stock_support_resistance_folderpath.c_str(), company_code_number.c_str());
	WRITE_FORMAT_DEBUG("Find company[%s] support and resistance", company_code_number.c_str());
	StockSupportResistance stock_support_resistance;
	ret = stock_support_resistance.initialize(filepath_buf, stock_close_price, time_filter_rule, volume_filter_rule);
	if (CHECK_FAILURE(ret))
		return ret;
	ret = stock_support_resistance.get_support_resistance_price_full_string(stock_support_resistance_string, show_detail);
	if (CHECK_FAILURE(ret))
		return ret;
	return ret;
}
///////////////////////////////////////////////////////////////////////////////////
