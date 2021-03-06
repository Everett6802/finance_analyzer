#ifndef COMMON_DEF_BASE_H
#define COMMON_DEF_BASE_H

#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macro

#ifndef CHECK_SUCCESS
#define CHECK_SUCCESS(X) ((X == RET_SUCCESS) ? true : false)
#endif

#ifndef CHECK_WARN
#define CHECK_WARN(X) ((X > RET_WARN_BASE && X < RET_FAILURE_BASE) ? true : false)
#endif

#ifndef CHECK_FAILURE
#define CHECK_FAILURE(X) ((X >= RET_FAILURE_BASE) ? true : false)
#endif

#ifndef FAILURE_IS_OUT_OF_RANGE
#define FAILURE_IS_OUT_OF_RANGE(X) ((X == RET_FAILURE_OUT_OF_RANGE) ? true : false)
#endif

#ifndef FAILURE_IS_NOT_FOUND
#define FAILURE_IS_NOT_FOUND(X) ((X == RET_FAILURE_NOT_FOUND) ? true : false)
#endif

// #ifndef FAILURE_IS_INTERACTIVE_COMMAND
// #define FAILURE_IS_INTERACTIVE_COMMAND(X) ((X == RET_FAILURE_INTERACTIVE_COMMAND) ? true : false)
// #endif

#ifndef CHECK_SUCCESS_IGNORE_WARN
#define CHECK_SUCCESS_IGNORE_WARN(X) !CHECK_FAILURE(X)
#endif

#ifndef RELEASE_OBJ
#define RELEASE_OBJ(X)\
if (X != NULL)\
{\
	delete X;\
	X = NULL;\
}
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(X)\
if (X != NULL)\
{\
	X->release();\
	X = NULL;\
}
#endif

#ifndef MIN
#define MIN(X,Y) (X <= Y ? X : Y)
#endif

#ifndef MAX
#define MAX(X,Y) (X >= Y ? X : Y)
#endif

#ifndef INDEX_IN_RANGE
#define INDEX_IN_RANGE(SI, EI, RS, RE)\
do{\
	char errmsg[256];\
	if (!check_start_index_in_range(SI, RS, RE))\
	{\
		snprintf(errmsg, 256, "start index[%d] is NOT in [%d, %d)", SI, RS, RE);\
		throw out_of_range(errmsg);\
	}\
	if (!check_end_index_in_range(EI, RS, RE))\
	{\
		snprintf(errmsg, 256, "end index[%d] is NOT in (%d, %d]", EI, RS, RE);\
		throw out_of_range(errmsg);\
	}\
}while(0)
#endif

#define PRINT(...) if (SHOW_CONSOLE) printf(__VA_ARGS__);
#define FPRINT(stream, ...) if (SHOW_CONSOLE) fprintf(stream, __VA_ARGS__);

#define CREATE_PROJECT_FILEPATH(variable_name, foldername, filename)\
const int variable_name##_buf_size = 256;\
char variable_name[variable_name##_buf_size];\
do{\
	static const int FILE_PATH_SIZE = 256;\
	char current_working_directory[FILE_PATH_SIZE];\
	getcwd(current_working_directory, FILE_PATH_SIZE);\
	snprintf(variable_name, variable_name##_buf_size, "%s/%s/%s", current_working_directory, foldername, filename);\
}while(0);

#define IS_FORMULA_STATISTICS_METHOD(X)\
((X >= StatisticsFormula_Start && X < StatisticsFormula_End) ? true : false)
#define IS_TABLE_STATISTICS_METHOD(X)\
((X >= StatisticsTable_Start && X < StatisticsTable_End) ? true : false)
#define IS_GRAPH_STATISTICS_METHOD(X)\
((X >= StatisticsGraph_Start && X < StatisticsGraph_End) ? true : false)

#define DECLARE_FINANCE_DATA_FIELD_TYPE_LIST() const int** FINANCE_DATA_FIELD_TYPE_LIST;
#define DECLARE_FINANCE_DATA_FIELD_AMOUNT_LIST() const int* FINANCE_DATA_FIELD_AMOUNT_LIST;

#define DEFINE_FINANCE_DATA_FIELD_TYPE_LIST(finance_data_type) FINANCE_DATA_FIELD_TYPE_LIST = get_finance_data_field_type_list(finance_data_type);
#define DEFINE_FINANCE_DATA_FIELD_AMOUNT_LIST(finance_data_type) FINANCE_DATA_FIELD_AMOUNT_LIST = get_finance_data_field_amount_list(finance_data_type);

#define DECLARE_AND_DEFINE_FINANCE_DATA_FIELD_TYPE_LIST(finance_data_type) const int** DEFINE_FINANCE_DATA_FIELD_TYPE_LIST(finance_data_type)
#define DECLARE_AND_DEFINE_FINANCE_DATA_FIELD_AMOUNT_LIST(finance_data_type) const int* DEFINE_FINANCE_DATA_FIELD_AMOUNT_LIST(finance_data_type)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definition
typedef std::deque<std::string> STRING_DEQUE;
typedef STRING_DEQUE* PSTRING_DEQUE;
typedef STRING_DEQUE::iterator STRING_DEQUE_ITER;
typedef STRING_DEQUE::const_iterator STRING_DEQUE_CONST_ITER;
typedef std::list<std::string> STRING_LIST;
typedef STRING_LIST* PSTRING_LIST;
typedef std::deque<int> INT_DEQUE;
typedef INT_DEQUE* PINT_DEQUE;
typedef INT_DEQUE::iterator INT_DEQUE_ITER;
typedef std::set<int> INT_SET;
typedef INT_SET* PINT_SET;
typedef INT_SET::iterator INT_SET_ITER;

typedef std::map<int, PINT_DEQUE> INT_INT_DEQUE_MAP;
typedef INT_INT_DEQUE_MAP* PINT_INT_DEQUE_MAP;
typedef INT_INT_DEQUE_MAP::iterator INT_INT_DEQUE_MAP_ITER;
typedef INT_INT_DEQUE_MAP::const_iterator INT_INT_DEQUE_MAP_CONST_ITER;
typedef INT_INT_DEQUE_MAP_ITER* PINT_INT_DEQUE_MAP_ITER;

typedef std::map<int, PSTRING_DEQUE> INT_STRING_DEQUE_MAP;
typedef INT_STRING_DEQUE_MAP* PINT_STRING_DEQUE_MAP;
typedef INT_STRING_DEQUE_MAP::iterator INT_STRING_DEQUE_MAP_ITER;
typedef INT_STRING_DEQUE_MAP_ITER* PINT_STRING_DEQUE_MAP_ITER;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enumeration
enum FinanceDataType
{
	FinanceData_SQL,
	FinanceData_CSV,
	FinanceDataSize,
	FinanceData_None
};

enum FinanceMethodType
{
////////////////////////////////////////////////////////////////////////////////////////////////
// Market data source
	FinanceMethod_MarketStart = 0, // Keep in mind to update the value at the right time 
	FinanceMethod_StockExchangeAndVolume = 0,
	FinanceMethod_StockTop3LegalPersonsNetBuyOrSell = 1,
	FinanceMethod_StockMarginTradingAndShortSelling = 2,
	FinanceMethod_FutureAndOptionTop3LegalPersonsOpenInterest = 3,
	FinanceMethod_FutureOrOptionTop3LegalPersonsOpenInterest = 4,
	FinanceMethod_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest = 5,
	FinanceMethod_OptionPutCallRatio = 6,
	FinanceMethod_FutureTop10DealersAndLegalPersons = 7,
	FinanceMethod_MarketEnd = 8, // Keep in mind to update the value at the right time
////////////////////////////////////////////////////////////////////////////////////////////////
// Stock data source
	FinanceMethod_StockStart = 8, // Keep in mind to update the value at the right time
	FinanceMethod_DepositoryShareholderDistributionTable = 8,
	FinanceMethod_BalanceSheet = 9,
	FinanceMethod_IncomeStatement = 10,
	FinanceMethod_CashFlowStatement = 11,
	FinanceMethod_StatementOfChangesInEquity = 12,
	FinanceMethod_DailyStockPriceAndVolume = 13,
	FinanceMethod_Top3LegalPersonsStockNetBuyOrSellSummary = 14,
	FinanceMethod_StockEnd = 15 // Keep in mind to update the value at the right time
};

enum FinanceFieldType
{
	FinanceField_UNKNOWN = -1,
	FinanceField_INT,
	FinanceField_LONG,
	FinanceField_FLOAT,
	FinanceField_DATE,
	FinanceFieldSize,
};

enum ArrayElementCalculationType
{
	ArrayElementCalculation_None,
	ArrayElementCalculation_Diff,
	ArrayElementCalculation_Rate,
	ArrayElementCalculation_Sum5,
	ArrayElementCalculation_Sum10,
	ArrayElementCalculation_Sum20,
	ArrayElementCalculation_Sum60,
	ArrayElementCalculation_Avg5,
	ArrayElementCalculation_Avg10,
	ArrayElementCalculation_Avg20,
	ArrayElementCalculation_Avg60,
	ArrayElementCalculation_WAvg5,
	ArrayElementCalculation_WAvg10,
	ArrayElementCalculation_WAvg20,
	ArrayElementCalculation_WAvg60,
	ArrayElementCalculationSize,
};

enum StatisticsMethod
{
	StatisticsMethod_None = -1,
	StatisticsFormula_Start = 0,
	StatisticsFormula_RangeValue = 0,
	StatisticsFormula_Average = 1,
	StatisticsFormula_Variance = 2,
	StatisticsFormula_End,
	StatisticsTable_Start = 0x100,
	StatisticsTable_End,
	StatisticsGraph_Start = 0x200,
	StatisticsGraph_End,
};

enum FinanceAnalysisMode
{
	FinanceAnalysis_Market,
	FinanceAnalysis_Stock,
	FinanceAnalysisSize,
	FinanceAnalysis_None
};

enum ResultSetDataUnit
{
	ResultSetDataUnit_SingleMethod, // Only single method data are stored in the ResultSet object
	ResultSetDataUnit_MultipleMethod // mutliple methods data are stored in the ResultSet object
};

enum TestType{
	Test_CheckArray, 
	Test_CheckArrayStatistics, 
	Test_CheckFilterArray, 
	Test_CheckFilterRule, 
	Test_CheckFormula, 
	Test_CheckFilterFormula, 
	// Test_CheckDataStatistics, 
	Test_CheckHistogram, 
	TestTypeSize
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables

// extern FinanceAnalysisMode g_finance_analysis_mode; // Caution: This variable is NOT used in interactive mode
// extern bool IS_FINANCE_MARKET_MODE; // Caution: This variable is NOT used in interactive mode
// extern bool IS_FINANCE_STOCK_MODE; // Caution: This variable is NOT used in interactive mode

extern const char* FINANCE_ANALYZER_PROCESS_NAME;

extern const char* FINANCE_MODE_DESCRIPTION[];
extern const char* FINANCE_DATA_DESCRIPTION[];

extern const char* FINANCE_DATA_MARKET_NAME;
extern const char* FINANCE_DATA_STOCK_NAME;

extern const int MARKET_METHOD_INDEX_LENGTH;
extern const int STOCK_METHOD_INDEX_LENGTH;

extern const char* DAILY_FINANCE_FILENAME_FORMAT;
extern const char* DAILY_FINANCE_EMAIL_TITLE_FORMAT;
extern const char* CONFIG_FOLDER_NAME;
extern const char* RESULT_FOLDER_NAME;
extern const char* FINANCE_ANALYZER_CONF_FILENAME;
extern const char* WORKDAY_CANLENDAR_CONF_FILENAME;
extern const char* DATABASE_TIME_RANGE_CONF_FILENAME;
extern const char* COMPANY_PROFILE_CONF_FILENAME;
extern const char* COMPANY_GROUP_CONF_FILENAME;
extern const char* MARKET_STOCK_SWITCH_CONF_FILENAME;
extern const char* DEFAULT_OUTPUT_FILENAME_FOR_PLOT;
extern const char* DEFAULT_MULTIPLE_2D_GRAPH_CONFIG_FILENAME;
extern const char* DEFAULT_FINANCE_ROOT_FOLDERPATH;
extern const char* SCRAPY_PROJECT_NAME;
extern const char* RECORDER_PROJECT_NAME;
extern const char* ANALYZER_PROJECT_NAME;
extern const char* FINANCE_SCRAPY_ROOT_FOLDERPATH;
extern const char* FINANCE_RECORDER_ROOT_FOLDERPATH;
extern const char* FINANCE_ANALYZER_ROOT_FOLDERPATH;
extern const char* DEFAULT_STOCK_SUPPORT_RESISTANCE_ROOT_FOLDERPATH;
extern const char* DEFAULT_SOURCE_COMPANY_PROFILE_CONF_FOLDERPATH;
extern const char* DEFAULT_CSV_ROOT_FOLDERPATH;
extern const char* CONFIG_TIMESTAMP_STRING_PREFIX;

// extern const char* MYSQL_TABLE_NAME_BASE;
extern const char* MYSQL_MARKET_DATABASE_NAME;
extern const char* MYSQL_STOCK_DATABASE_NAME;
extern const char* MYSQL_DATE_FILED_NAME;
extern const char* MYSQL_FILED_NAME_BASE;

extern const char* FINANCE_FIELD_TYPE_DESCRIPTION[];

extern const char* FORMULA_STATSTICS_METHOD_DESCRIPTION[];
extern const int FORMULA_STATSTICS_METHOD_SIZE;
extern const char* TABLE_STATSTICS_METHOD_DESCRIPTION[];
extern const int TABLE_STATSTICS_METHOD_SIZE;
extern const char* GRAPH_STATSTICS_METHOD_DESCRIPTION[];
extern const int GRAPH_STATSTICS_METHOD_SIZE;

extern const char* FINANCE_METHOD_DATA_NAME_LIST[];
extern const int FINANCE_METHOD_DATA_NAME_LIST_LEN;
#define FINANCE_SQL_TABLE_NAME_LIST FINANCE_METHOD_DATA_NAME_LIST
#define FINANCE_SQL_TABLE_NAME_LIST_LEN FINANCE_METHOD_DATA_NAME_LIST_LEN
#define FINANCE_CSV_FILE_NAME_LIST FINANCE_METHOD_DATA_NAME_LIST
#define FINANCE_CSV_FILE_NAME_LIST_LEN FINANCE_METHOD_DATA_NAME_LIST_LEN
extern const char* FINANCE_METHOD_DESCRIPTION_LIST[];
extern const int* FINANCE_SQL_DATA_FIELD_TYPE_LIST[];
extern const int* FINANCE_CSV_DATA_FIELD_TYPE_LIST[];
extern const int FINANCE_SQL_DATA_FIELD_AMOUNT_LIST[];
extern const int FINANCE_CSV_DATA_FIELD_AMOUNT_LIST[];
extern const char* FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[];

extern const int SHOW_RES_STDOUT;
extern const int SHOW_RES_EMAIL;
extern const int SHOW_RES_FILE;
extern const int SHOW_RES_SYSLOG;
extern const int SHOW_RES_DEFAULT;
extern const int SHOW_RES_ALL;

extern const char* SHOW_RES_TYPE_DESCRIPTION[];
extern const int SHOW_RES_TYPE_SIZE;

// extern const char* SEVERITY_NAME[];
// extern const int SEVERITY_NAME_SIZE;

// extern const int SOURCE_KEY_FIELD_MASK;
extern const int SOURCE_KEY_METHOD_INDEX_BIT_OFFSET;
extern const int SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET;
extern const int SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET;
extern const int SOURCE_KEY_METHOD_INDEX_MASK;
extern const int SOURCE_KEY_COMPANY_CODE_NUMBER_MASK;
extern const int SOURCE_KEY_COMPANY_GROUP_NUMBER_MASK;
extern const int NO_METHOD_MARKET_SOURCE_KEY_VALUE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Company profile field index
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER;
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_NAME;
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_LISTING_DATE;
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_MARKET;
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_INDUSTRY;
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NAME;
extern const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NUMBER;
extern const int COMPANY_PROFILE_ENTRY_FIELD_SIZE;

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Test Cases
// extern const char* TEST_TYPE_DESCRIPTION[];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return values
extern const unsigned short RET_SUCCESS;

extern const unsigned short RET_WARN_BASE;
extern const unsigned short RET_WARN_INDEX_DUPLICATE;
extern const unsigned short RET_WARN_INDEX_IGNORE;
extern const unsigned short RET_WARN_INTERACTIVE_COMMAND;
extern const unsigned short RET_WARN_SEARCH_RULE_EMPTY;

extern const unsigned short RET_FAILURE_BASE;
extern const unsigned short RET_FAILURE_UNKNOWN;
extern const unsigned short RET_FAILURE_INVALID_ARGUMENT;
extern const unsigned short RET_FAILURE_INVALID_POINTER;
extern const unsigned short RET_FAILURE_INSUFFICIENT_MEMORY;
extern const unsigned short RET_FAILURE_INCORRECT_OPERATION;
extern const unsigned short RET_FAILURE_OPEN_FILE;
extern const unsigned short RET_FAILURE_NOT_FOUND;
extern const unsigned short RET_FAILURE_NOT_EQUAL;
extern const unsigned short RET_FAILURE_OUT_OF_RANGE;
extern const unsigned short RET_FAILURE_INCORRECT_CONFIG;
extern const unsigned short RET_FAILURE_INCORRECT_PATH;
extern const unsigned short RET_FAILURE_INCORRECT_FORMAT;
extern const unsigned short RET_FAILURE_IO_OPERATION;
extern const unsigned short RET_FAILURE_HANDLE_THREAD;
extern const unsigned short RET_FAILURE_SYSTEM_API;
extern const unsigned short RET_FAILURE_MYSQL;
// extern const unsigned short RET_FAILURE_INTERACTIVE_COMMAND;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
extern bool SHOW_CONSOLE;

extern const int INTERACTIVE_SERVER_PORT;
extern const int INTERACTIVE_SERVER_BACKLOG;
extern const char* INTERACTIVE_PROMPT;

extern const int START_YEAR;
extern const int END_YEAR;
extern const char* START_DATE_STR;
extern const char* END_DATE_STR;
extern const char* START_MONTH_STR;
extern const char* END_MONTH_STR;
extern const char* START_QUARTER_STR;
extern const char* END_QUARTER_STR;
extern const char* START_TIME_STR_ARRAY[];
extern const char* END_TIME_STR_ARRAY[];

#endif
