#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include <stdexcept>
#include <new>
#include "common_function.h"
#include "msg_dumper_wrapper.h"


using namespace std;

FinanceAnalysisMode get_finance_analysis_mode_from_file()
{
	static FinanceAnalysisMode finance_analysis_mode = FinanceAnalysis_None;
	if (finance_analysis_mode == FinanceAnalysis_None)
	{
		static const int ERRMSG_SIZE = 256;
		static char errmsg[ERRMSG_SIZE];
		CREATE_PROJECT_FILEPATH(file_path, CONFIG_FOLDER_NAME, MARKET_STOCK_SWITCH_CONF_FILENAME)
	// First check if the config file exists
		if (!check_file_exist(file_path))
		{
			snprintf(errmsg, ERRMSG_SIZE, "The %s config file does NOT exist", MARKET_STOCK_SWITCH_CONF_FILENAME);
			throw runtime_error(string(errmsg));
		}
		static const unsigned int BUF_SIZE = 4;
		char buf[BUF_SIZE];
		// unsigned short ret = RET_SUCCESS;
		// WRITE_FORMAT_DEBUG("Parse the config file: %s", MARKET_STOCK_SWITCH_CONF_FILENAME);
		FILE* fp = fopen(file_path, "r");
		if (fp == NULL)
		{
			snprintf(errmsg, ERRMSG_SIZE, "Fail to open the config file: %s, due to: %s", MARKET_STOCK_SWITCH_CONF_FILENAME, strerror(errno));
			throw runtime_error(string(errmsg));
		}
	// Parse the config file
		if (fgets(buf, BUF_SIZE, fp) == NULL)
		{
			snprintf(errmsg, ERRMSG_SIZE, "Incorrect format in config file[%s]", MARKET_STOCK_SWITCH_CONF_FILENAME);
			throw runtime_error(string(errmsg));
			goto OUT;
		}
	OUT:
		if (fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}

		int buf_len = strlen(buf);
		if (buf[buf_len - 1] == '\n')
			buf[buf_len - 1] = '\0';
		int mode = atoi(buf);
		if (mode < 0 || mode >= FinanceAnalysisSize)
		{
			char errmsg[64];
			snprintf(errmsg, 64, "Unknown finance analysis mode: %s", buf);
			throw runtime_error(string(errmsg));
		}
		finance_analysis_mode = (FinanceAnalysisMode)mode;
	}
	return finance_analysis_mode;
}
// bool is_market_mode(){return get_finance_analysis_mode() == FinanceAnalysis_Market;}
// bool is_stock_mode(){return get_finance_analysis_mode() == FinanceAnalysis_Stock;}

int get_source_key(int method_index/*, FinanceAnalysisMode finance_analysis_mode*/)
{
	// if (finance_analysis_mode == FinanceAnalysis_None)
	// 	finance_analysis_mode = g_finance_analysis_mode;
	// if (finance_analysis_mode != FinanceAnalysis_Market)
	// 	throw logic_error(string("It's NOT Market mode"));
	if (method_index == -1)
	{
// No source type mode
		return NO_METHOD_MARKET_SOURCE_KEY_VALUE;
	}
	else
	{
// Source type mode
		return method_index;
	}
}

int get_source_key(int company_group_number, const string& company_code_number, int method_index/*, FinanceAnalysisMode finance_analysis_mode*/)
{
	// if (finance_analysis_mode == FinanceAnalysis_None)
	// 	finance_analysis_mode = g_finance_analysis_mode;
	// if (finance_analysis_mode != FinanceAnalysis_Stock)
	// 	throw logic_error(string("It's NOT Stock mode"));
	int company_code_number_int = atoi(company_code_number.c_str());
	if (method_index == -1)
	{
// No source type mode
		return (company_group_number << SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET | company_code_number_int << SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET);
	}
	else
	{
// Source type mode
		return (company_group_number << SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET | company_code_number_int << SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET | method_index << SOURCE_KEY_METHOD_INDEX_BIT_OFFSET);
	}
}

int get_method(int source_key)
{
	return ((source_key & SOURCE_KEY_METHOD_INDEX_MASK) >> SOURCE_KEY_METHOD_INDEX_BIT_OFFSET);
}

string get_company_code_number(int source_key)
{
	static const int COMPANY_CODE_NUMBER_LEN = 4;
	static char company_code_number[COMPANY_CODE_NUMBER_LEN];
	snprintf(company_code_number, COMPANY_CODE_NUMBER_LEN, "%04d", (source_key & SOURCE_KEY_COMPANY_CODE_NUMBER_MASK) >> SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET);
	return string(company_code_number);
}
int get_company_group_number(int source_key)
{
	return ((source_key & SOURCE_KEY_COMPANY_GROUP_NUMBER_MASK) >> SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET);
}

const char* get_ret_description(unsigned short ret)
{
	static const char* success_ret_description = "Success";
	static const char* warn_ret_descriptions[] =
	{
		"Warn_Base",
		"Warn_IndexDuplicate",
		"Warn_IndexIgnore",
		"Warn_InteractiveCommand",
		"Warn_SearchRuleEmpty"
	};
	static const char* failure_ret_descriptions[] =
	{
		"Failure_Base",
		"Failure_Unknown",
		"Failure_InvalidArgument",
		"Failure_InvalidPointer",
		"Failure_InsufficientMemory",
		"Failure_IncorrectOperation",
		"Failure_OpenFile",
		"Failure_NotFound",
		"Failure_NotEqual",
		"Failure_OutOfRange",
		"Failure_IncorrectConfig",
		"Failure_IncorrectPath",
		"Failure_IncorrectFormat",
		"Failure_IOOperation",
		"Failure_HandleThread",
		"Failure_SystemAPI",
		"Failure_MySQL",
		// "Failure_InteractiveCommand",
	};
	if (CHECK_FAILURE(ret))
		return failure_ret_descriptions[ret - RET_FAILURE_BASE];
	else if (CHECK_WARN(ret))
		return warn_ret_descriptions[ret - RET_WARN_BASE];
	else
		return success_ret_description;
}
const int** get_finance_sql_data_field_type_list(){return FINANCE_SQL_DATA_FIELD_TYPE_LIST;}
const int** get_finance_csv_data_field_type_list(){return FINANCE_CSV_DATA_FIELD_TYPE_LIST;}
const int** get_finance_data_field_type_list(FinanceDataType finance_data_type)
{
	typedef const int** (*GET_FINANCE_DATA_FIELD_TYPE_LIST_FUNC_PTR)();
	static GET_FINANCE_DATA_FIELD_TYPE_LIST_FUNC_PTR get_finance_data_field_type_list_func_ptr[] =
	{
		get_finance_sql_data_field_type_list,
		get_finance_csv_data_field_type_list
	};
	return (*(get_finance_data_field_type_list_func_ptr[finance_data_type]))();
}

const int* get_finance_sql_data_field_amount_list(){return FINANCE_SQL_DATA_FIELD_AMOUNT_LIST;}
const int* get_finance_csv_data_field_amount_list(){return FINANCE_CSV_DATA_FIELD_AMOUNT_LIST;}
const int* get_finance_data_field_amount_list(FinanceDataType finance_data_type)
{
	typedef const int* (*GET_FINANCE_DATA_FIELD_AMOUNT_LIST_FUNC_PTR)();
	static GET_FINANCE_DATA_FIELD_AMOUNT_LIST_FUNC_PTR get_finance_data_field_amount_list_func_ptr[] =
	{
		get_finance_sql_data_field_amount_list,
		get_finance_csv_data_field_amount_list
	};
	return (*(get_finance_data_field_amount_list_func_ptr[finance_data_type]))();
}

const char* get_sql_field_description(int method_index, int field_index)
{
	switch(method_index)
	{
		case FinanceMethod_StockExchangeAndVolume:
			return SQL_STOCK_EXCHANGE_AND_VALUE_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_StockTop3LegalPersonsNetBuyOrSell:
			return SQL_STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_StockMarginTradingAndShortSelling:
			return SQL_STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_FutureAndOptionTop3LegalPersonsOpenInterest:
			return SQL_FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_FutureOrOptionTop3LegalPersonsOpenInterest:
			return SQL_FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest:
			return SQL_OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_OptionPutCallRatio:
			return SQL_OPTION_PUT_CALL_RATIO_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_FutureTop10DealersAndLegalPersons:
			return SQL_FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_DepositoryShareholderDistributionTable:
			return SQL_COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_BalanceSheet:
			return SQL_BALANCE_SHEET_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_IncomeStatement:
			return SQL_INCOME_STATEMENT_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_CashFlowStatement:
			return SQL_CASH_FLOW_STATEMENT_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_StatementOfChangesInEquity:
			return SQL_STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_DESCRIPTION[field_index];
		default:
		{
			static const int BUF_SIZE = 256;
			static char buf[BUF_SIZE];
			snprintf(buf, BUF_SIZE, "Unknown source index: %d", method_index);
			throw invalid_argument(buf);
		}
		break;
	}
	return NULL;
}

const char* get_csv_field_description(int method_index, int field_index)
{
	switch(method_index)
	{
		case FinanceMethod_StockExchangeAndVolume:
			return CSV_STOCK_EXCHANGE_AND_VALUE_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_StockTop3LegalPersonsNetBuyOrSell:
			return CSV_STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_StockMarginTradingAndShortSelling:
			return CSV_STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_FutureAndOptionTop3LegalPersonsOpenInterest:
			return CSV_FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_FutureOrOptionTop3LegalPersonsOpenInterest:
			return CSV_FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest:
			return CSV_OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_OptionPutCallRatio:
			return CSV_OPTION_PUT_CALL_RATIO_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_FutureTop10DealersAndLegalPersons:
			return CSV_FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_DepositoryShareholderDistributionTable:
			return CSV_COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_BalanceSheet:
			return CSV_BALANCE_SHEET_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_IncomeStatement:
			return CSV_INCOME_STATEMENT_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_CashFlowStatement:
			return CSV_CASH_FLOW_STATEMENT_FIELD_DESCRIPTION[field_index];
		case FinanceMethod_StatementOfChangesInEquity:
			return CSV_STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_DESCRIPTION[field_index];
		default:
		{
			static const int BUF_SIZE = 256;
			static char buf[BUF_SIZE];
			snprintf(buf, BUF_SIZE, "Unknown source index: %d", method_index);
			throw invalid_argument(buf);
		}
		break;
	}
	return NULL;
}

const char* get_field_description(int method_index, int field_index/*, FinanceAnalysisMode finance_analysis_mode*/, FinanceDataType finance_data_type)
{
	// assert((method_index >= 0 && method_index < FinanceSourceSize) && "method_index is out of range");
	// assert((field_index >= 0 && field_index < FINANCE_DATA_FIELD_AMOUNT_LIST[method_index]) && "field_index is out of range");
	// assert(check_method_index_in_range(method_index, finance_analysis_mode) && "method_index is out of range");
	assert(check_field_index_in_range(method_index, field_index, finance_data_type) && "field_index is out of range");
	typedef const char* (*GET_FILED_DESCRIPTION_FUNC_PTR)(int method_index, int field_index);
	static GET_FILED_DESCRIPTION_FUNC_PTR get_field_description_func_ptr[] = 
	{
		get_sql_field_description,
		get_csv_field_description
	};
	return (*(get_field_description_func_ptr[finance_data_type]))(method_index, field_index);
}

bool check_method_index_in_range(int method_index, FinanceAnalysisMode finance_analysis_mode)
{
    if (finance_analysis_mode == FinanceAnalysis_Market)
    {
        if (method_index >= FinanceMethod_MarketStart && method_index < FinanceMethod_MarketEnd)
            return true;
    }
    else if (finance_analysis_mode == FinanceAnalysis_Stock)
    {
        if (method_index >= FinanceMethod_StockStart && method_index < FinanceMethod_StockEnd)
            return true;
    }
    else
    	throw invalid_argument(string("finance_analysis_mode should NOT be FinanceAnalysis_None"));
    return false;
}

void get_method_index_range(int& method_index_start, int& method_index_end, FinanceAnalysisMode finance_analysis_mode)
{
	// if (finance_analysis_mode == FinanceAnalysis_None)
	// 	finance_analysis_mode = g_finance_analysis_mode;
	switch(finance_analysis_mode)
	{
	case FinanceAnalysis_Market:
	{
		method_index_start = FinanceMethod_MarketStart;
		method_index_end = FinanceMethod_MarketEnd;
	}
	break;
	case FinanceAnalysis_Stock:
	{
		method_index_start = FinanceMethod_StockStart;
		method_index_end = FinanceMethod_StockEnd;
	}
	break;
	default:
		throw std::invalid_argument(string("Unknown finance analysis mode"));
	break;
	}
}

int get_method_size(FinanceAnalysisMode finance_analysis_mode)
{
	// if (finance_analysis_mode == FinanceAnalysis_None)
	// 	finance_analysis_mode = g_finance_analysis_mode;
	if (finance_analysis_mode == FinanceAnalysis_Market)
		return MARKET_METHOD_INDEX_LENGTH;
	else if (finance_analysis_mode == FinanceAnalysis_Stock)
		return STOCK_METHOD_INDEX_LENGTH;
	else
		throw runtime_error(string("Unknown finance mode"));
}

bool check_sql_field_index_in_range(int method_index, int field_index)
{
	if(field_index < 0 && field_index >= FINANCE_SQL_DATA_FIELD_AMOUNT_LIST[method_index])
	{
// If field_index == -1, it means select all field in the table
		if (field_index != -1)
			return false;
	}
	return true;
}

bool check_csv_field_index_in_range(int method_index, int field_index)
{
	if(field_index < 0 && field_index >= FINANCE_CSV_DATA_FIELD_AMOUNT_LIST[method_index])
	{
// If field_index == -1, it means select all field in the table
		if (field_index != -1)
			return false;
	}
	return true;
}

bool check_field_index_in_range(int method_index, int field_index, FinanceDataType finance_data_type)
{
	typedef bool (*CHECK_DATA_FIELD_INDEX_IN_RANGE_FUNC_PTR)(int method_index, int field_index);
	static CHECK_DATA_FIELD_INDEX_IN_RANGE_FUNC_PTR check_data_field_index_in_range_func_ptr[] = 
	{
		check_sql_field_index_in_range,
		check_csv_field_index_in_range
	};
	return (*(check_data_field_index_in_range_func_ptr[finance_data_type]))(method_index, field_index);
}

bool check_calculation_type_in_range(int calculation_type)
{
    if (calculation_type < 0 && calculation_type >= ArrayElementCalculationSize)
        return false;
    return true;
}

bool check_file_exist(const char* filepath)
{
	assert(filepath != NULL && "filepath should NOT be NULL");
	struct stat dummy;
	return (stat(filepath, &dummy) == 0);
}

bool check_config_file_exist(const char* config_filename)
{
	// static const int FILE_PATH_SIZE = 256;
	// static char file_path[FILE_PATH_SIZE];
	// char current_working_directory[FILE_PATH_SIZE];
	// getcwd(current_working_directory, FILE_PATH_SIZE);
	// snprintf(file_path, FILE_PATH_SIZE, "%s/%s/%s", current_working_directory, CONFIG_FOLDER_NAME, config_filename);
	CREATE_PROJECT_FILEPATH(file_path, CONFIG_FOLDER_NAME, config_filename)
	return check_file_exist(file_path);
}

unsigned short get_file_line_count(unsigned int &line_count, const char* filepath)
{
	if (!check_file_exist(filepath))
	{
		STATIC_WRITE_FORMAT_ERROR("The file[%s] does NOT exist", filepath);
		return RET_FAILURE_NOT_FOUND;		
	}
	FILE* fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to open the file[%s], due to: %s", filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	static const int BUF_SIZE = 512;
	static char line_buf[BUF_SIZE];
	line_count = 0;
	while (fgets(line_buf, BUF_SIZE, fp) != NULL) line_count++;
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;	
}

unsigned short read_file_lines_ex(std::list<std::string>& line_list, const char* filepath, const char* file_read_attribute, PTIME_RANGE_PARAM time_range_param, char data_seperate_character)
{
	if (!check_file_exist(filepath))
	{
		STATIC_WRITE_FORMAT_ERROR("The file[%s] does NOT exist", filepath);
		return RET_FAILURE_NOT_FOUND;		
	}
	FILE* fp = fopen(filepath, file_read_attribute);
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to open the file[%s], due to: %s", filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	static const int BUF_SIZE = 512;
	static char line_buf[BUF_SIZE];
	int last_character_in_string_index = 0;
	while (fgets(line_buf, BUF_SIZE, fp) != NULL) 
	{
		if (line_buf[0] == '\n' || line_buf[0] == '#')
			continue;
		last_character_in_string_index = strlen(line_buf) - 1;
		if (line_buf[last_character_in_string_index] == '\n')
			line_buf[last_character_in_string_index] = '\0';
// CAUTION: To check the time range, I assume the time field index is 0 
		if (time_range_param != NULL)
		{
			static const int TIME_BUF_SIZE = 16;
			static char time_buf[TIME_BUF_SIZE];
			char* pos = strchr(line_buf, data_seperate_character);
			if (pos == NULL)
			{
				STATIC_WRITE_FORMAT_ERROR("Incorrect data format in file[%s]: %s", filepath, line_buf);
				return RET_FAILURE_INCORRECT_FORMAT;
			}
			int pos_index = pos - line_buf;
			if (pos_index >= TIME_BUF_SIZE)
			{
				STATIC_WRITE_FORMAT_ERROR("Incorrect time format in file[%s], time string length: %d", filepath, pos_index);
				return RET_FAILURE_INCORRECT_FORMAT;
			}
			memset(time_buf, 0x0, sizeof(char) * TIME_BUF_SIZE);
			memcpy(time_buf, line_buf, sizeof(char) * pos_index);
			TimeInRangeType time_in_range_type = TimeRangeParam::time_in_range_type(time_range_param, time_buf);
			if (time_in_range_type == TIME_BEFORE_RANGE)
				continue;
			else if (time_in_range_type == TIME_AFTER_RANGE)
				goto OUT;
		}
		string line_str(line_buf);
		line_list.push_back(line_str);
	}
OUT:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;
}

unsigned short read_config_file_lines_ex(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_file_read_attribute, const char* config_folderpath)
{
	CREATE_PROJECT_FILEPATH(filepath, CONFIG_FOLDER_NAME, config_filename);
	return read_file_lines_ex(conf_line_list, filepath, config_file_read_attribute);
}

unsigned short read_config_file_lines(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_folderpath)
{
	return read_config_file_lines_ex(conf_line_list, config_filename, "r", config_folderpath);
}

unsigned short write_file_lines_ex(const std::list<std::string>& line_list, const char* filepath, const char* file_write_attribute)
{
	static string new_line("\n");
	FILE* fp = fopen(filepath, file_write_attribute);
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to open the file[%s], due to: %s", filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	list<string>::const_iterator iter = line_list.begin();
	while (iter != line_list.end())
	{
		string line = (string)*iter + new_line;
		fputs(line.c_str(), fp);
		iter++;
	}
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;
}

unsigned short write_config_file_lines_ex(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_file_write_attribute, const char* config_folderpath)
{
	CREATE_PROJECT_FILEPATH(filepath, CONFIG_FOLDER_NAME, config_filename);
	return write_file_lines_ex(conf_line_list, filepath, config_file_write_attribute);
}

unsigned short write_config_file_lines(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_folderpath)
{
	return write_config_file_lines_ex(conf_line_list, config_filename, "w", config_folderpath);
}

unsigned short get_config_file_timestamp(std::string& timestamp_string, const char* config_filename, const char* config_folderpath)
{
	FILE* fp = NULL;
	if (config_folderpath != NULL)
	{
// Open the config file which is NOT located in the default config folder path
		static const int FILEPATH_SIZE = 256;
		static char filepath[FILEPATH_SIZE];
		snprintf(filepath, FILEPATH_SIZE, "%s/%s", config_folderpath, config_filename);
		if (!check_file_exist(filepath))
		{
			STATIC_WRITE_FORMAT_ERROR("The file[%s] does NOT exist", filepath);
			return RET_FAILURE_NOT_FOUND;		
		}
		fp = fopen(filepath, "r");
	}
	else
	{
// Open the config file which is located in the default config folder path
		if (!check_config_file_exist(config_filename))
		{
			STATIC_WRITE_FORMAT_ERROR("The config file[%s] does NOT exist", config_filename);
			return RET_FAILURE_NOT_FOUND;		
		}
		CREATE_PROJECT_FILEPATH(filepath, CONFIG_FOLDER_NAME, config_filename);
		fp = fopen(filepath, "r");
	}
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to open the config file[%s], due to: %s", config_filename, strerror(errno));
		return RET_FAILURE_SYSTEM_API;				
	}
	static const int TIMESTAMP_BUF_SIZE = 64;
	static char line_buf[TIMESTAMP_BUF_SIZE];
	if (fgets(line_buf, TIMESTAMP_BUF_SIZE, fp) == NULL) 
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to read the config file[%s]", config_filename);
		return RET_FAILURE_SYSTEM_API;				
	}
	int last_character_in_string_index = strlen(line_buf) - 1;
	if (line_buf[last_character_in_string_index] == '\n')
		line_buf[last_character_in_string_index] = '\0';
	static char timestamp_prefix_buf[TIMESTAMP_BUF_SIZE];
	static char date_buf[TIMESTAMP_BUF_SIZE];
	static char time_buf[TIMESTAMP_BUF_SIZE];
	sscanf(line_buf, "%s %s %s", timestamp_prefix_buf, date_buf, time_buf);
	if (strstr(timestamp_prefix_buf, CONFIG_TIMESTAMP_STRING_PREFIX) == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to timestamp in the config file[%s], due to: %s", config_filename, strerror(errno));
		return RET_FAILURE_INCORRECT_CONFIG;
	}
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	static char timestamp_buf[TIMESTAMP_BUF_SIZE];
	snprintf(timestamp_buf, TIMESTAMP_BUF_SIZE, "%s %s", date_buf, time_buf);
	timestamp_string.assign(timestamp_buf);
	return RET_SUCCESS;
}

bool check_config_file_timestamp_equal(const char* config_filename1, const char* config_filename2, const char* config_folderpath1, const char* config_folderpath2)
{
	unsigned short ret;
	string timestamp1;
	string timestamp2;
	ret = get_config_file_timestamp(timestamp1, config_filename1, config_folderpath1);
	if (CHECK_FAILURE(ret))
		return false;
	ret = get_config_file_timestamp(timestamp2, config_filename2, config_folderpath2);
	if (CHECK_FAILURE(ret))
		return false;
	return (timestamp1 == timestamp2 ? true : false);
}

unsigned short create_folder_if_not_exist(const char* path, int mode)
{
	if (!check_file_exist(path))
	{
		/* Directory does not exist. EEXIST for race condition */
		if (mkdir(path, mode) != 0 && errno != EEXIST)
		{
			fprintf(stderr, "Fail to create the folder: %s, due to: %s", path, strerror(errno));
			return RET_FAILURE_SYSTEM_API;
		}
	}
	return RET_SUCCESS;
}

unsigned short create_folder_in_project_if_not_exist(const char* foldername_in_project, int mode)
{
	static const int FILE_PATH_SIZE = 256;
	static char folder_path[FILE_PATH_SIZE];
	char current_working_directory[FILE_PATH_SIZE];
	getcwd(current_working_directory, FILE_PATH_SIZE);
	snprintf(folder_path, FILE_PATH_SIZE, "%s/%s", current_working_directory, foldername_in_project);
	return create_folder_if_not_exist(folder_path, mode);
}

unsigned short direct_string_to_output_stream(const char* data, const char* filepath)
{
	assert(data != NULL && "data should NOT be NULL");
//	assert(filepath != NULL && "filepath should NOT be NULL");
	FILE* fp = NULL;
	if(filepath != NULL)
	{
		fp = fopen(filepath, "w");
		if (fp == NULL)
		{
			fprintf(stderr, "Fail to open file: %s, due to: %s", filepath, strerror(errno));
			return RET_FAILURE_SYSTEM_API;
		}
	}
	else
		fp = stdout;
	fputs(data, fp);
	if(filepath != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;
}

unsigned short send_email(const char* title, const char* address, const char* content)
{
	static int buf_size_torlerance = 128;
	assert(title != NULL && "title should NOT be NULL");
	assert(address != NULL && "address should NOT be NULL");
	assert(content != NULL && "content should NOT be NULL");

	int content_len = strlen(content);
	char* buf = new char[strlen(content) + buf_size_torlerance];
	if (buf == NULL)
	{
		fprintf(stderr, "Fail to allocate the memory: buf\n");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}

	snprintf(buf, buf_size_torlerance + content_len, "echo \"%s\" | mail -s \"%s\" %s", content, title, address);
	FILE* fp = popen(buf, "r");
	if (fp != NULL)
	{
		pclose(fp);
		fp = NULL;
	}

	if (buf != NULL)
	{
		delete[] buf;
		buf = NULL;
	}

	return RET_SUCCESS;
}

// End index range: Positive: (0, data_size-1); Negative: (-1, -data_size)
int get_end_index_ex(int end_index, int data_size){return ((end_index < 0) ? end_index = data_size + end_index + 1 : end_index);}

bool check_start_index_in_range(int start_index, int range_start, int range_end)
{
	assert(range_start >= 0 && "range_start should be larger than 0");
	assert(range_end >= 1 && "range_end should be larger than 1");
	return ((start_index >= range_start && start_index < range_end) ? true : false);
}

bool check_end_index_in_range(int end_index, int range_start, int range_end)
{
	assert(range_start >= 0 && "range_start should be larger than 0");
	assert(range_end >= 1 && "range_end should be larger than 1");
	return ((end_index > range_start && end_index <= range_end) ? true : false);
}

int get_array_index(int value, const int array[], int array_size)
{
	for(int index = 0 ; index < array_size ; index++)
	{
		if (array[index] == value)
			return index;
	}
	return -1;
}

int get_array_sum(const int array[], int array_size)
{
	int sum = 0;
	for(int index = 0 ; index < array_size ; index++)
		sum += array[index];
	return sum;
}

unsigned short read_text_file(STRING_LIST& text_result_list, const char* text_filepath)
{
	assert(text_filepath != NULL && "text_filepath should NOT be NULL");

	if (!check_file_exist(text_filepath))
	{
		fprintf(stderr, "Fail to find the file: %s\n", text_filepath);
		return RET_FAILURE_NOT_FOUND;
	}

	unsigned int BUF_SIZE = 1024;
	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
	if (buf == NULL)
	{
		fprintf(stderr, "Fail to allocate memoery: buf\n");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}

	FILE* fp = fopen(text_filepath, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Fail to open file: %s, due to: %s\n", text_filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}

	while (fgets(buf, BUF_SIZE, fp) != NULL) 
	{
// Check if the buffer size is enough
		if (strlen(buf) == BUF_SIZE - 1 && buf[BUF_SIZE - 1] == '\0')
		{
			do
			{
				int BUF_SIZE_OLD = BUF_SIZE;
				char* buf_old = buf;
				BUF_SIZE <<= 1;
				buf = (char*)realloc(buf_old, sizeof(char) * BUF_SIZE);
				fgets(&buf[BUF_SIZE_OLD - 1], BUF_SIZE - BUF_SIZE_OLD, fp);
			}while(strlen(buf) == BUF_SIZE - 1 && buf[BUF_SIZE - 1] == '\0');
		}
        // printf("data: %s", buf);
        text_result_list.push_back(string(buf));
    }
// OUT:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}

	return RET_SUCCESS;
}

unsigned short read_text_file(string& text_result_string, const char* text_filepath)
{
	STRING_LIST text_result_list;
	unsigned short ret = read_text_file(text_result_list, text_filepath);
	if (CHECK_SUCCESS(ret))
	{
		STRING_LIST::iterator iter = text_result_list.begin();
		while (iter != text_result_list.end())
		{
			text_result_string += *iter;
			iter++;
		}
	}
	return ret;
}

void daemonize() 
{ 
// Step 1: Fork off the parent process
 	pid_t pid = fork();
  	if (pid < 0) exit(EXIT_FAILURE);
	if (pid != 0) exit (EXIT_SUCCESS);
// Step 2: Create a unique session ID
   	pid = setsid();
	if (pid < -1) exit(EXIT_FAILURE);
// Step 3: Change the working directory
	chdir ("/"); 
// Step 4: Close the standard file descriptors
	int fd = open ("/dev/null", O_RDWR, 0);
	if (fd != -1) 
	{
		dup2 (fd, STDIN_FILENO);
		dup2 (fd, STDOUT_FILENO);
		dup2 (fd, STDERR_FILENO);
		if (fd > 2) close (fd);
	}
// Step 5: Change the file mode mask
	umask (0027);
	// RELEASE_MSG_DUMPER();
	closelog();
}

void get_int_deque_from_range_string(const char* int_range_string, INT_DEQUE& int_deque)
{
	static const int ERRMSG_SIZE = 64;
	static char errmsg[ERRMSG_SIZE];
	if (int_range_string == NULL)
		throw invalid_argument("int_range_string should NOT be NULL");
	if (strchr(int_range_string, '-') == NULL)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The string[%s] is NOT a range string", int_range_string);
		throw invalid_argument(errmsg);
	}
	int int_range_start, int_range_end;
	sscanf(int_range_string, "%d-%d", &int_range_start, &int_range_end);
	if (int_range_start > int_range_end)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The start integer[%d] should NOT be greater than the end one[%d]", int_range_start, int_range_end);
		throw invalid_argument(errmsg);
	}
	for (int i = int_range_start ; i <= int_range_end ; i++)
	{
		// printf("Add index: %d into Deque\n", i);
		int_deque.push_back(i);
	}
}

void get_int_deque_from_partial_string(char* int_range_string, int int_range_string_len, INT_DEQUE& int_deque)
{
	// static const int ERRMSG_SIZE = 64;
	// static char errmsg[ERRMSG_SIZE];
	if (int_range_string == NULL)
		throw invalid_argument("int_range_string should NOT be NULL");
	int int_range_string_actual_len = strlen(int_range_string);
	char* int_range_string_tmp = NULL;
	bool full_string = false;
	if (int_range_string_actual_len <= int_range_string_len)
	{
		int_range_string_len = int_range_string_actual_len;
		int_range_string_tmp = int_range_string;
		full_string = true;
	}
	else
	{
		int_range_string_tmp = new char[int_range_string_len + 1];
		if (int_range_string_tmp == NULL)
			throw invalid_argument("Fail to allocate memory: int_range_string_tmp");
		memset(int_range_string_tmp, 0x0, sizeof(char) * (int_range_string_len + 1));
		memcpy(int_range_string_tmp, int_range_string, sizeof(char) * int_range_string_len);
	}
	if (strchr(int_range_string_tmp, '-') != NULL)
		get_int_deque_from_range_string(int_range_string_tmp, int_deque);
	else
	{
		int value = atoi(int_range_string_tmp);
		// printf("Add index: %d into Deque\n", value);
		int_deque.push_back(value);
	}
	if (!full_string)
		delete [] int_range_string_tmp; 
}

unsigned short copy_file(const char* src_absolute_filepath, const char* dst_absolute_filepath)
{
	assert(src_absolute_filepath != NULL && dst_absolute_filepath != NULL && "src_absolute_filepath != NULL and dst_absolute_filepath != NULL");
	if (!check_file_exist(src_absolute_filepath))
	{
		STATIC_WRITE_FORMAT_ERROR("The source file[%s] does NOT exist", src_absolute_filepath);
		return RET_FAILURE_NOT_FOUND;
	}
	static const int CMD_SIZE = 512;
	static char cmd[CMD_SIZE];
	snprintf(cmd, CMD_SIZE, "cp %s %s", src_absolute_filepath, dst_absolute_filepath);
	FILE *fp = popen(cmd, "r");
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("Fail to copy the file[%s], due to: %s", cmd, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Caution: Nothing to read
	// char buf[256];
	// while(fgets(buf, sizeof(buf), fp)!=NULL) 
	// {
 //        printf("%s\n", buf);
 //    }
	pclose(fp);
	return RET_SUCCESS;
}

unsigned short copy_config_file(const char* config_filename, const char* src_config_folderpath)
{
	static const int SRC_FILEPATH_SIZE = 256;
	static char src_filepath[SRC_FILEPATH_SIZE];
	snprintf(src_filepath, SRC_FILEPATH_SIZE, "%s/%s", src_config_folderpath, config_filename);
	CREATE_PROJECT_FILEPATH(dst_filepath, CONFIG_FOLDER_NAME, config_filename);
	return copy_file(src_filepath, dst_filepath);
}

unsigned short get_process_count(const char* process_name, int& proccss_count)
{
	static const int CMD_SIZE = 256;
	static char cmd[CMD_SIZE];
	unsigned short ret = RET_SUCCESS;
	snprintf(cmd, CMD_SIZE, "ps aux | grep %s | grep -v grep | wc -l", process_name);
	FILE* fp = popen(cmd, "r");
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("popen() fails by command[%s], due to: %s", cmd, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	static const int BUF_SIZE = 16;
	static char buf[BUF_SIZE];
	if (fgets(buf, BUF_SIZE, fp) == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("fgets() fails, due to: %s", strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
	proccss_count = atoi(buf);
OUT:
	if (fp != NULL)
	{
		pclose(fp);
		fp = NULL;
	}
	return ret;
}

unsigned short get_process_username(const char* process_name, string& process_username)
{
	static const int CMD_SIZE = 256;
	static char cmd[CMD_SIZE];
	unsigned short ret = RET_SUCCESS;
// Check process exist
	int process_count;
	ret = get_process_count(process_name, process_count);
	if (CHECK_FAILURE(ret))
		return ret;
	if (process_count == 0)
	{
		STATIC_WRITE_FORMAT_ERROR("The process[%s] does NOT exist", process_name);
		return RET_FAILURE_SYSTEM_API;
	}
	snprintf(cmd, CMD_SIZE, "ps aux | grep %s | grep -v grep | awk '{print $1}'", process_name);
	FILE* fp = popen(cmd, "r");
	if (fp == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("popen() fails by command[%s], due to: %s", cmd, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	static const int BUF_SIZE = 16;
	static char buf[BUF_SIZE];
	int buf_len;
	if (fgets(buf, BUF_SIZE, fp) == NULL)
	{
		STATIC_WRITE_FORMAT_ERROR("fgets() fails, due to: %s", strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
	buf_len = strlen(buf);
	if (buf[buf_len - 1] == '\n')
		buf[buf_len - 1] = '\0';
	process_username = string(buf);
OUT:
	if (fp != NULL)
	{
		pclose(fp);
		fp = NULL;
	}
	return ret;
}

unsigned short get_absolute_filepath_from_username(const char* relative_filepath, char** absolute_filepath)
{
	assert(relative_filepath != NULL && "relative_filepath should NOT be NULL");
	assert(absolute_filepath != NULL && "absolute_filepath should NOT be NULL");
	// uid_t uid = geteuid();
	// struct passwd *pw = getpwuid(uid);
	// // printf("%s\n", pw->pw_name);
	// // if (relative_filepath[0] != '~')
	// // {
	// // 	STATIC_WRITE_FORMAT_ERROR("The filepath[%s] does NOT start from ~", relative_filepath);
	// // 	return RET_FAILURE_INVALID_ARGUMENT;
	// // }
	// // if (relative_filepath[1] != '/')
	// // {
	// // 	STATIC_WRITE_FORMAT_ERROR("The filepath[%s] is NOT correct format", relative_filepath);
	// // 	return RET_FAILURE_INVALID_ARGUMENT;
	// // }
	string process_username;
	unsigned short ret = get_process_username(FINANCE_ANALYZER_PROCESS_NAME, process_username);
	if (CHECK_FAILURE(ret))
		return ret;
	static const int ABSOLUTE_FILEPATH_BUF_SIZE = 256;
	char* absolute_filepath_tmp = new char[ABSOLUTE_FILEPATH_BUF_SIZE];
	if (absolute_filepath_tmp == NULL)
		throw bad_alloc();
	// if (strcmp("root", pw->pw_name) == 0)
	// 	snprintf(absolute_filepath_tmp, ABSOLUTE_FILEPATH_BUF_SIZE, "/%s", relative_filepath);
	// else
	// 	snprintf(absolute_filepath_tmp, ABSOLUTE_FILEPATH_BUF_SIZE, "/home/%s/%s", pw->pw_name, relative_filepath);	
	if (strcmp("root", process_username.c_str()) == 0)
		snprintf(absolute_filepath_tmp, ABSOLUTE_FILEPATH_BUF_SIZE, "/%s", relative_filepath);
	else
		snprintf(absolute_filepath_tmp, ABSOLUTE_FILEPATH_BUF_SIZE, "/home/%s/%s", process_username.c_str(), relative_filepath);	
	*absolute_filepath = absolute_filepath_tmp;
	return RET_SUCCESS;
}

unsigned short check_string_is_digit(const char* time_string, int time_string_len)
{
	assert(time_string != NULL && "time_string should NOT be NULL");
	if (time_string_len == -1)
		time_string_len = strlen(time_string);
	for (int i = 0 ; i < time_string_len ; i++)
	{
		if (time_string[i] < '0' || time_string[i] > '9')
		{
			STATIC_WRITE_FORMAT_ERROR("Not all characters in the string[%s] are digits", time_string);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}
	return RET_SUCCESS;
}