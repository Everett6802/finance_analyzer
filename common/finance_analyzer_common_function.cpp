#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common_function.h"


using namespace std;

string get_usage_string(bool interactive, FinanceAnalysisMode finance_analysis_mode)
{
	static string usage_string;
	if (!usage_string.empty())
		return usage_string;
	static const int BUF_SIZE = 256;
	static char buf[BUF_SIZE];
	usage_string += string("====================== Usage ======================\n");
	if (interactive)
	{
		usage_string += string("* get_finance_mode\n Description: Get current finance mode\n");
		usage_string += string("* set_finance_mode\n Description: Set current finance mode\n");
		usage_string += string(" Finance mode:\n");
		for(int i = 0 ; i < FinanceAnalysisSize ; i++)
		{
			snprintf(buf, BUF_SIZE, "  %s: %d\n", FINANCE_MODE_DESCRIPTION[i], i);
			usage_string += string(buf);
		}
		usage_string += string("\n");
	}
	else
	{
		snprintf(buf, BUF_SIZE, "--market_mode --stock_mode\n Description: Switch the market/stock mode\n Caution: Read parameters from %s when NOT set\n", MARKET_STOCK_SWITCH_CONF_FILENAME);
		usage_string += string(buf);
	}
	if (interactive)
		usage_string += string("* help\n Description: The usage\n\n");
	else
		usage_string += string("-h|--help\n Description: The usage\n Caution: Other flags are ignored\n");
	if (!interactive)
		usage_string += string("--silent\n Description: Disable print log on console\n");
	if (interactive)
	{
// No support in interactive mode
		// usage_string += string("* test\n Description: Run test case\n");
		// usage_string += string("* test_verbose\n Description: Run test case and show detailed steps\n");
		// usage_string += string(" Test case list:");
		// int source_type_start_index, source_type_end_index;
		// get_source_type_index_range(source_type_start_index, source_type_end_index, finance_analysis_mode);
		// for (int i = source_type_start_index ; i < source_type_end_index ; i++)
		// {
		// 	snprintf(buf, BUF_SIZE, "  %s: %d\n", TEST_TYPE_DESCRIPTION[i], i);
		// 	usage_string += string(buf);
		// }
		// usage_string += string("  Format: 1,2,4 (Start from 0)\n");
		// usage_string += string("  all: All types\n");
	}
	else
	{
		usage_string += string("--test\nDescription: Run test case\nCaution: Other flags are ignored\n");
		usage_string += string("--test_verbose\nDescription: Run test case and show detailed steps\nCaution: Other flags are ignored\n");
		usage_string += string(" Test case list: ");
		int source_type_start_index, source_type_end_index;
		get_source_type_index_range(source_type_start_index, source_type_end_index);
		for (int i = source_type_start_index ; i < source_type_end_index ; i++)
		{
			snprintf(buf, BUF_SIZE, "%s[%d] ", TEST_TYPE_DESCRIPTION[i], i);
			usage_string += string(buf);
		}
		usage_string += string("\n");
		usage_string += string("  Format: 1,2,4 (Start from 0)\n");
		usage_string += string("  all: All types\n");	
	}
	if (!interactive)
		usage_string += string("-i|--interactive\n Description: Run the program in the interactive mode\n Caution: All flags except --daemonize are ignored\n");
	// usage_string += string("--daemonize\n Description: Daemonize the process\n Caution: Must be in the interactive mode\n");
	// // usage_string += string("--calculate_statistics\nDescription: Calculate the statistics by different analysis method\n");
	// // for (int i = 0 ; i < FORMULA_STATSTICS_METHOD_SIZE ; i++)
	// // 	usage_string += string("%s[%d] ", FORMULA_STATSTICS_METHOD_DESCRIPTION[i], i);
	// // usage_string += string("\n");
	// // usage_string += string("--fillout_statistics\nDescription: Fillout the statistics by different analysis method\n");
	// // for (int i = 0 ; i < TABLE_STATSTICS_METHOD_SIZE ; i++)
	// // 	usage_string += string("%s[%d] ", TABLE_STATSTICS_METHOD_DESCRIPTION[i], i);
	// // usage_string += string("\n");
	// // usage_string += string("--plot_statistics\nDescription: Plot the statistics by different analysis method\n");
	// // for (int i = 0 ; i < GRAPH_STATSTICS_METHOD_SIZE ; i++)
	// // 	usage_string += string("%s[%d] ", GRAPH_STATSTICS_METHOD_DESCRIPTION[i], i);
	// // usage_string += string("\n");
	// // usage_string += string("--statistics_time_range\nDescription: The time range of statistics result\n");
 // //    usage_string += string("  Format 1 (start_time): 2015-01-01\n");
 // //    usage_string += string("  Format 2 (start_time,end_time): 2015-01-01,2015-09-04\n");
 // //    usage_string += string("  Format 3 (,end_time): ,2015-09-04\n");
	// // usage_string += string("\n");
	usage_string += string("===================================================\n");
	return usage_string;
}

FinanceAnalysisMode get_finance_analysis_mode()
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
bool is_market_mode(){return get_finance_analysis_mode() == FinanceAnalysis_Market;}
bool is_stock_mode(){return get_finance_analysis_mode() == FinanceAnalysis_Stock;}

int get_source_key(int source_type_index)
{
	if (!is_market_mode())
		throw logic_error(string("It's NOT Market mode"));
	if (source_type_index == -1)
	{
// No source type mode
		return NO_SOURCE_TYPE_MARKET_SOURCE_KEY_VALUE;
	}
	else
	{
// Source type mode
		return source_type_index;
	}
}

int get_source_key(int company_group_number, const string& company_code_number, int source_type_index)
{
	if (!is_stock_mode())
		throw logic_error(string("It's NOT Stock mode"));
	int company_code_number_int = atoi(company_code_number.c_str());
	if (source_type_index == -1)
	{
// No source type mode
		return (company_group_number << SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET | company_code_number_int << SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET);
	}
	else
	{
// Source type mode
		return (company_group_number << SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET | company_code_number_int << SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET | source_type_index << SOURCE_KEY_SOURCE_TYPE_INDEX_BIT_OFFSET);
	}
}

int get_source_type(int source_key)
{
	return ((source_key & SOURCE_KEY_SOURCE_TYPE_INDEX_MASK) >> SOURCE_KEY_SOURCE_TYPE_INDEX_BIT_OFFSET);
}

string get_company_code_number(int source_key)
{
	if (!is_stock_mode())
		throw logic_error(string("It's NOT Stock mode"));
	static const int COMPANY_CODE_NUMBER_LEN = 4;
	static char company_code_number[COMPANY_CODE_NUMBER_LEN];
	snprintf(company_code_number, COMPANY_CODE_NUMBER_LEN, "%04d", (source_key & SOURCE_KEY_COMPANY_CODE_NUMBER_MASK) >> SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET);
	return string(company_code_number);
}
int get_company_group_number(int source_key)
{
	if (!is_stock_mode())
		throw logic_error(string("It's NOT Stock mode"));
	return ((source_key & SOURCE_KEY_COMPANY_GROUP_NUMBER_MASK) >> SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET);
}

const char* get_ret_description(unsigned short ret)
{
	static const char* success_ret_description = "Success";
	static const char* warn_ret_descriptions[] =
	{
		"Warn_Base",
		"Warn_IndexDuplicate",
		"Warn_IndexIgnore"
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
		"Failure_IOOperation",
		"Failure_HandleThread",
		"Failure_SystemAPI",
		"Failure_MySQL",
		"Failure_InteractiveCommand",
	};
	if (CHECK_FAILURE(ret))
		return failure_ret_descriptions[ret - RET_FAILURE_BASE];
	else if (CHECK_WARN(ret))
		return warn_ret_descriptions[ret - RET_WARN_BASE];
	else
		return success_ret_description;
}

const char* get_database_field_description(int source_type_index, int field_index)
{
	// assert((source_type_index >= 0 && source_type_index < FinanceSourceSize) && "source_type_index is out of range");
	// assert((field_index >= 0 && field_index < FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_type_index]) && "field_index is out of range");
	assert(check_source_type_index_in_range(source_type_index) && "source_type_index is out of range");
	assert(check_field_index_in_range(source_type_index, field_index) && "field_index is out of range");

	switch(source_type_index)
	{
		case FinanceSource_StockExchangeAndVolume:
			return STOCK_EXCHANGE_AND_VALUE_FIELD_DESCRIPTION[field_index];
		case FinanceSource_StockTop3LegalPersonsNetBuyOrSell:
			return STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DESCRIPTION[field_index];
		case FinanceSource_StockMarginTradingAndShortSelling:
			return STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_DESCRIPTION[field_index];
		case FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest:
			return FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceSource_FutureOrOptionTop3LegalPersonsOpenInterest:
			return FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest:
			return OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_DESCRIPTION[field_index];
		case FinanceSource_OptionPutCallRatio:
			return OPTION_PUT_CALL_RATIO_FIELD_DESCRIPTION[field_index];
		case FinanceSource_FutureTop10DealersAndLegalPersons:
			return FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DESCRIPTION[field_index];
		default:
		{
			static const int BUF_SIZE = 256;
			static char buf[BUF_SIZE];
			snprintf(buf, BUF_SIZE, "Unknown source index: %d", source_type_index);
			throw invalid_argument(buf);
		}
		break;
	}
	return NULL;
}

bool check_source_type_index_in_range(int source_type_index)
{
    if (IS_FINANCE_MARKET_MODE)
    {
        if (source_type_index >= FinanceSource_MarketStart && source_type_index < FinanceSource_MarketEnd)
            return true;
    }
    else if (IS_FINANCE_STOCK_MODE)
    {
        if (source_type_index >= FinanceSource_StockStart && source_type_index < FinanceSource_StockEnd)
            return true;
    }
    return false;
}

void get_source_type_index_range(int& source_type_index_start, int& source_type_index_end, FinanceAnalysisMode finance_analysis_mode)
{
	switch(finance_analysis_mode)
	{
	case FinanceAnalysis_None:
	{
		if (IS_FINANCE_MARKET_MODE)
		{
			source_type_index_start = FinanceSource_MarketStart;
			source_type_index_end = FinanceSource_MarketEnd;
		}
		else if (IS_FINANCE_STOCK_MODE)
		{
			source_type_index_start = FinanceSource_StockStart;
			source_type_index_end = FinanceSource_StockEnd;
		}
		else
			throw runtime_error(string("Unknown finance mode"));
	}	
	break;
	case FinanceAnalysis_Market:
	{
		source_type_index_start = FinanceSource_MarketStart;
		source_type_index_end = FinanceSource_MarketEnd;
	}
	break;
	case FinanceAnalysis_Stock:
	{
		source_type_index_start = FinanceSource_StockStart;
		source_type_index_end = FinanceSource_StockEnd;
	}
	break;
	default:
		throw std::invalid_argument(string("Unknown finance analysis mode"));
	break;
	}
}

int get_source_type_size()
{
	if (IS_FINANCE_MARKET_MODE)
		return MARKET_SOURCE_TYPE_INDEX_LENGTH;
	else if (IS_FINANCE_STOCK_MODE)
		return STOCK_SOURCE_TYPE_INDEX_LENGTH;
	else
		throw runtime_error(string("Unknown finance mode"));
}

bool check_field_index_in_range(int source_type_index, int field_index)
{
	if(field_index < 0 && field_index >= FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_type_index])
	{
// If field_index == -1, it means select all field in the table
		if (field_index != -1)
			return false;
	}
	return true;
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
