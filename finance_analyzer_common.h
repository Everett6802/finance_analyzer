#ifndef FINANCE_ANALYZER_COMMON_H
#define FINANCE_ANALYZER_COMMON_H

#include <stdlib.h>


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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enumeration

enum FinanceSourceType
{
	FinanceSource_StockExchangeAndVolume,
	FinanceSource_StockTop3LegalPersonsNetBuyOrSell,
	FinanceSource_StockMarginTradingAndShortSelling,
	FinanceSource_FutureAndOptionTop3LegalPersonsOpenInterest,
	FinanceSource_FutureOrOptionTop3LegalPersonsOpenInterest,
	FinanceSource_OptionTop3LegalPersonsBuyAndSellOptionOpenInterest,
	FinanceSource_OptionPutCallRatio,
	FinanceSource_FutureTop10DealersAndLegalPersons,
	FinanceSourceSize
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
	ArrayElementCalculation_Avg5,
	ArrayElementCalculation_Avg10,
	ArrayElementCalculation_Avg20,
	ArrayElementCalculation_Avg60,
	ArrayElementCalculationSize,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables

extern const char* DAILY_FINANCE_FILENAME_FORMAT;
extern const char* DAILY_FINANCE_EMAIL_TITLE_FORMAT;
extern const char* CONFIG_FOLDER_NAME;
extern const char* RESULT_FOLDER_NAME;
extern const char* FINANCE_ANALYZER_CONF_FILENAME;
extern const char* WORKDAY_CANLENDAR_CONF_FILENAME;
extern const char* DATABASE_TIME_RANGE_CONF_FILENAME;

extern const char* MYSQL_TABLE_NAME_BASE;
extern const char* MYSQL_DATE_FILED_NAME;
extern const char* MYSQL_FILED_NAME_BASE;

extern const int STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION[];
extern const int STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[];
extern const int STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION[];
extern const int FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[];
extern const int FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[];
extern const int OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[];
extern const int OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION[];
extern const int FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[];

extern const char* FINANCE_DATABASE_NAME_LIST[];
extern const int FINANCE_DATABASE_NAME_LIST_LEN;
extern const char* FINANCE_DATABASE_DESCRIPTION_LIST[];
extern const int* FINANCE_DATABASE_FIELD_TYPE_LIST[];
extern const int FINANCE_DATABASE_FIELD_AMOUNT_LIST[];
extern const char* FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[];

// Return values
extern const unsigned short RET_SUCCESS;

extern const unsigned short RET_WARN_BASE;
extern const unsigned short RET_WARN_INDEX_DUPLICATE;
extern const unsigned short RET_WARN_INDEX_IGNORE;

extern const unsigned short RET_FAILURE_BASE;
extern const unsigned short RET_FAILURE_UNKNOWN;
extern const unsigned short RET_FAILURE_INVALID_ARGUMENT;
extern const unsigned short RET_FAILURE_INVALID_POINTER;
extern const unsigned short RET_FAILURE_INSUFFICIENT_MEMORY;
extern const unsigned short RET_FAILURE_INCORRECT_OPERATION;
extern const unsigned short RET_FAILURE_OPEN_FILE;
extern const unsigned short RET_FAILURE_NOT_FOUND;
extern const unsigned short RET_FAILURE_INCORRECT_CONFIG;
extern const unsigned short RET_FAILURE_INCORRECT_PATH;
extern const unsigned short RET_FAILURE_IO_OPERATION;
extern const unsigned short RET_FAILURE_HANDLE_THREAD;
extern const unsigned short RET_FAILURE_SYSTEM_API;
extern const unsigned short RET_FAILURE_MYSQL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
extern bool SHOW_CONSOLE;

/////////////////////////////////////////////////////////ArrayElementCalculation_Diff///////////////////////////////////////////////////////////////////////////////////
// Functions
const char* get_ret_description(unsigned short ret);
bool check_file_exist(const char* filepath);
bool check_config_file_exist(const char* filepath);
unsigned short create_folder_if_not_exist(const char* path, int mode=0755);
unsigned short create_folder_in_project_if_not_exist(const char* foldername_in_project, int mode=0755);
unsigned short direct_string_to_output_stream(const char* data, const char* filepath=NULL);
unsigned short send_email(const char* title, const char* address, const char* content);
int get_end_index_ex(int end_index, int data_size);

#endif
