#include <sys/stat.h>
#include <unistd.h>
#include <new>
#include "common_definition_base.h"
#include "common_definition_data_market_field.h"
#include "common_definition_data_stock_field.h"


using namespace std;

// FinanceAnalysisMode g_finance_analysis_mode = FinanceAnalysis_None;
// bool IS_FINANCE_MARKET_MODE = (g_finance_analysis_mode == FinanceAnalysis_Market ? true : false);
// bool IS_FINANCE_STOCK_MODE = (g_finance_analysis_mode == FinanceAnalysis_Stock ? true : false);

const char* FINANCE_ANALYZER_PROCESS_NAME = "finance_analyzer";

const char* FINANCE_MODE_DESCRIPTION[] = {"Market", "Stock"};
const char* FINANCE_DATA_DESCRIPTION[] = {"SQL", "CSV"};

const char* FINANCE_DATA_MARKET_NAME = "market";
const char* FINANCE_DATA_STOCK_NAME = "stock";

const int MARKET_METHOD_INDEX_LENGTH = FinanceMethod_MarketEnd - FinanceMethod_MarketStart;
const int STOCK_METHOD_INDEX_LENGTH = FinanceMethod_StockEnd - FinanceMethod_StockStart;

const char* DAILY_FINANCE_FILENAME_FORMAT = "daily_finance%04d%02d%02d";
const char* DAILY_FINANCE_EMAIL_TITLE_FORMAT = "daily_finance%04d%02d%02d";
const char* CONFIG_FOLDER_NAME = "conf";
const char* RESULT_FOLDER_NAME = "result";
const char* FINANCE_ANALYZER_CONF_FILENAME = "finance_analyzer.conf";
const char* WORKDAY_CANLENDAR_CONF_FILENAME = ".workday_canlendar.conf";
const char* DATABASE_TIME_RANGE_CONF_FILENAME = ".database_time_range.conf";
const char* COMPANY_PROFILE_CONF_FILENAME = ".company_profile.conf";
const char* COMPANY_GROUP_CONF_FILENAME = ".company_group.conf";
const char* MARKET_STOCK_SWITCH_CONF_FILENAME = "market_stock_switch.conf";
const char* DEFAULT_OUTPUT_FILENAME_FOR_PLOT = "result_data.txt";
const char* DEFAULT_MULTIPLE_2D_GRAPH_CONFIG_FILENAME = ".multiple_2d_graph.conf";
const char* DEFAULT_FINANCE_ROOT_FOLDERPATH = "/opt/finance";
const char* SCRAPY_PROJECT_NAME = "finance_scrapy";
const char* RECORDER_PROJECT_NAME = "finance_recorder";
const char* ANALYZER_PROJECT_NAME = "finance_analyzer";
const char* FINANCE_SCRAPY_ROOT_FOLDERPATH = "/opt/finance/finance_scrapy";
const char* FINANCE_RECORDER_ROOT_FOLDERPATH = "/opt/finance/finance_recorder";
const char* FINANCE_ANALYZER_ROOT_FOLDERPATH = "/opt/finance/finance_analyzer";
const char* DEFAULT_STOCK_SUPPORT_RESISTANCE_ROOT_FOLDERPATH = "~/stock_price_support_resistance";
const char* DEFAULT_SOURCE_COMPANY_PROFILE_CONF_FOLDERPATH = "/opt/finance/finance_scrapy/conf"; 
// const char* DEFAULT_SOURCE_COMPANY_PROFILE_CONF_FOLDERPATH = "/home/super/Projects/finance_scrapy_python/conf"; 
const char* DEFAULT_CSV_ROOT_FOLDERPATH = "/var/tmp/finance";
const char* CONFIG_TIMESTAMP_STRING_PREFIX = "#time@";

// const char* MYSQL_TABLE_NAME_BASE = "year";
const char* MYSQL_MARKET_DATABASE_NAME = "finance_market";
const char* MYSQL_STOCK_DATABASE_NAME = "finance_stock";
const char* MYSQL_DATE_FILED_NAME = "date";
const char* MYSQL_FILED_NAME_BASE = "value";

const char* FORMULA_STATSTICS_METHOD_DESCRIPTION[] = 
{
	"Range Value by Formula",
	"Average by Formula",
	"Variance by Formula",
};
const int FORMULA_STATSTICS_METHOD_SIZE = sizeof(FORMULA_STATSTICS_METHOD_DESCRIPTION) / sizeof(FORMULA_STATSTICS_METHOD_DESCRIPTION[0]);
const char* TABLE_STATSTICS_METHOD_DESCRIPTION[] = 
{
};
const int TABLE_STATSTICS_METHOD_SIZE = sizeof(TABLE_STATSTICS_METHOD_DESCRIPTION) / sizeof(TABLE_STATSTICS_METHOD_DESCRIPTION[0]);
const char* GRAPH_STATSTICS_METHOD_DESCRIPTION[] = 
{
};
const int GRAPH_STATSTICS_METHOD_SIZE = sizeof(GRAPH_STATSTICS_METHOD_DESCRIPTION) / sizeof(GRAPH_STATSTICS_METHOD_DESCRIPTION[0]);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* FINANCE_FIELD_TYPE_DESCRIPTION[] = 
{
	"INT", "LONG", "FLOAT", "DATE"
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* FINANCE_METHOD_DATA_NAME_LIST[] =
{
	"stock_exchange_and_volume",
	"stock_top3_legal_persons_net_buy_or_sell",
	"stock_margin_trading_and_short_selling",
	"future_and_option_top3_legal_persons_open_interest",
	"future_or_option_top3_legal_persons_open_interest",
	"option_top3_legal_persons_buy_and_sell_option_open_interest",
	"option_put_call_ratio",
	"future_top10_dealers_and_legal_persons",
	"depository_shareholder_distribution_table",
	"balance_sheet",
	"income_statement",
	"cash_flow_statement",
	"statement_of_changes_in_equity",
	"daily_stock_price_and_volume",
	"top3_legal_persons_stock_net_buy_and_sell_summary"
};
const int FINANCE_METHOD_DATA_NAME_LIST_LEN = sizeof(FINANCE_METHOD_DATA_NAME_LIST) / sizeof(FINANCE_METHOD_DATA_NAME_LIST[0]);

const char* FINANCE_METHOD_DESCRIPTION_LIST[] =
{
	"臺股指數及成交量",
	"三大法人現貨買賣超",
	"現貨融資融券餘額",
	"三大法人期貨和選擇權留倉淨額",
	"三大法人期貨或選擇權留倉淨額",
	"三大法人選擇權買賣權留倉淨額",
	"三大法人選擇權賣權買權比",
	"十大交易人及特定法人期貨資訊",
    "個股集保戶股權分散表",
    "資產負債表",
    "損益表",
    "現金流量表",
    "股東權益變動表",
	"個股日股價及成交量",
	"三大法人個股買賣超日報"
};

const int* FINANCE_SQL_DATA_FIELD_TYPE_LIST[] =
{
	SQL_STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION,
	SQL_STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION,
	SQL_STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION,
	SQL_FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	SQL_FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	SQL_OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	SQL_OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION,
	SQL_FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION,
	SQL_COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION,
	SQL_BALANCE_SHEET_FIELD_TYPE_DEFINITION,
	SQL_INCOME_STATEMENT_FIELD_TYPE_DEFINITION,
	SQL_CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION,
	SQL_STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION
};

const int* FINANCE_CSV_DATA_FIELD_TYPE_LIST[] =
{
	CSV_STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION,
	CSV_STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION,
	CSV_STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION,
	CSV_FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	CSV_FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	CSV_OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	CSV_OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION,
	CSV_FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION,
	CSV_COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION,
	CSV_BALANCE_SHEET_FIELD_TYPE_DEFINITION,
	CSV_INCOME_STATEMENT_FIELD_TYPE_DEFINITION,
	CSV_CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION,
	CSV_STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION
};

const int FINANCE_SQL_DATA_FIELD_AMOUNT_LIST[] =
{
	SQL_STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION_SIZE,
	SQL_STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION_SIZE,
	SQL_STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION_SIZE,
	SQL_FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	SQL_FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	SQL_OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	SQL_OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION_SIZE,
	SQL_FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION_SIZE,
	SQL_COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION_SIZE,
	SQL_BALANCE_SHEET_FIELD_TYPE_DEFINITION_SIZE,
	SQL_INCOME_STATEMENT_FIELD_TYPE_DEFINITION_SIZE,
	SQL_CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION_SIZE,
	SQL_STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION_SIZE
};

const int FINANCE_CSV_DATA_FIELD_AMOUNT_LIST[] =
{
	CSV_STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION_SIZE,
	CSV_STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION_SIZE,
	CSV_STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION_SIZE,
	CSV_FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	CSV_FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	CSV_OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	CSV_OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION_SIZE,
	CSV_FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION_SIZE,
	CSV_COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION_SIZE,
	CSV_BALANCE_SHEET_FIELD_TYPE_DEFINITION_SIZE,
	CSV_INCOME_STATEMENT_FIELD_TYPE_DEFINITION_SIZE,
	CSV_CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION_SIZE,
	CSV_STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION_SIZE
};

const char* FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[] =
{
	"None",
	"Diff",
	"Sum5",
	"Sum10",
	"Sum20",
	"Sum60",
	"Avg5",
	"Avg10",
	"Avg20",
	"Avg60"
};

const int SHOW_RES_STDOUT = 0x1;
const int SHOW_RES_EMAIL = 0x2;
const int SHOW_RES_FILE = 0x4;
const int SHOW_RES_SYSLOG = 0x8;
const int SHOW_RES_DEFAULT = SHOW_RES_STDOUT | SHOW_RES_EMAIL;
const int SHOW_RES_ALL = SHOW_RES_STDOUT | SHOW_RES_EMAIL | SHOW_RES_FILE | SHOW_RES_SYSLOG;

const char* SHOW_RES_TYPE_DESCRIPTION[] =
{
	"Stdout",
	"Email",
	"File",
	"Syslog",
	"Default(Stdout/Email)",
	"All(Stdout/Email/File/Syslog)",
};
const int SHOW_RES_TYPE_SIZE = sizeof(SHOW_RES_TYPE_DESCRIPTION) / sizeof(SHOW_RES_TYPE_DESCRIPTION[0]);

// const char* SEVERITY_NAME[] = {"ERROR", "WARN", "INFO", "DEBUG"};
// const int SEVERITY_NAME_SIZE = sizeof(SEVERITY_NAME) / sizeof(SEVERITY_NAME[0]);

// const int SOURCE_KEY_FIELD_MASK = 0xFF << 8;
const int SOURCE_KEY_METHOD_INDEX_BIT_OFFSET = 0;
const int SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET = 8;
const int SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET = 24;
const int SOURCE_KEY_METHOD_INDEX_MASK = 0xFF << SOURCE_KEY_METHOD_INDEX_BIT_OFFSET;
const int SOURCE_KEY_COMPANY_CODE_NUMBER_MASK = 0xFFFF << SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET;
const int SOURCE_KEY_COMPANY_GROUP_NUMBER_MASK = 0xFF << SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET;
const int NO_METHOD_MARKET_SOURCE_KEY_VALUE = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Company profile field index
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER = 0;
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_NAME = 1;
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_LISTING_DATE = 3;
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_MARKET = 4;
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_INDUSTRY = 5;
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NAME = 7;
const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NUMBER = 8;
const int COMPANY_PROFILE_ENTRY_FIELD_SIZE = 9;

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Test Cases

// const char* TEST_TYPE_DESCRIPTION[] =
// {
// 	"Check Array",
// 	"Check Array Statistics",
// 	"Check Filter Array",
// 	"Check Filter Rule",
// 	"Check Formula",
// 	"Check Filter Formula",
// 	// "Check Data Statistics",
// 	"Check Histogram"
// };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return values

const unsigned short RET_SUCCESS = 0;

const unsigned short RET_WARN_BASE = 0x1;
const unsigned short RET_WARN_INDEX_DUPLICATE = RET_WARN_BASE + 1;
const unsigned short RET_WARN_INDEX_IGNORE = RET_WARN_BASE + 2;
const unsigned short RET_WARN_INTERACTIVE_COMMAND = RET_WARN_BASE + 3;
const unsigned short RET_WARN_SEARCH_RULE_EMPTY = RET_WARN_BASE + 4;

const unsigned short RET_FAILURE_BASE = 0x100;
const unsigned short RET_FAILURE_UNKNOWN = RET_FAILURE_BASE + 1;
const unsigned short RET_FAILURE_INVALID_ARGUMENT = RET_FAILURE_BASE + 2;
const unsigned short RET_FAILURE_INVALID_POINTER = RET_FAILURE_BASE + 3;
const unsigned short RET_FAILURE_INSUFFICIENT_MEMORY = RET_FAILURE_BASE + 4;
const unsigned short RET_FAILURE_INCORRECT_OPERATION = RET_FAILURE_BASE + 5;
const unsigned short RET_FAILURE_OPEN_FILE = RET_FAILURE_BASE + 6;
const unsigned short RET_FAILURE_NOT_FOUND = RET_FAILURE_BASE + 7;
const unsigned short RET_FAILURE_NOT_EQUAL = RET_FAILURE_BASE + 8;
const unsigned short RET_FAILURE_OUT_OF_RANGE = RET_FAILURE_BASE + 9;
const unsigned short RET_FAILURE_INCORRECT_CONFIG = RET_FAILURE_BASE + 10;
const unsigned short RET_FAILURE_INCORRECT_PATH = RET_FAILURE_BASE + 11;
const unsigned short RET_FAILURE_INCORRECT_FORMAT = RET_FAILURE_BASE + 12;
const unsigned short RET_FAILURE_IO_OPERATION = RET_FAILURE_BASE + 13;
const unsigned short RET_FAILURE_HANDLE_THREAD = RET_FAILURE_BASE + 14;
const unsigned short RET_FAILURE_SYSTEM_API = RET_FAILURE_BASE + 15;
const unsigned short RET_FAILURE_MYSQL = RET_FAILURE_BASE + 16;
// const unsigned short RET_FAILURE_INTERACTIVE_COMMAND = RET_FAILURE_BASE + 16;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
bool SHOW_CONSOLE = true;

const int INTERACTIVE_SERVER_PORT = 6802;
const int INTERACTIVE_SERVER_BACKLOG = 5;
const char* INTERACTIVE_PROMPT = "FA> ";

const int START_YEAR = 2000;
const int END_YEAR = 2100;
const char* START_DATE_STR = "2000-01-01";
const char* END_DATE_STR = "2100-01-01";
const char* START_MONTH_STR = "2000-01";
const char* END_MONTH_STR = "2100-01";
const char* START_QUARTER_STR = "2000q1";
const char* END_QUARTER_STR = "2100q1";
const char* START_TIME_STR_ARRAY[] = {START_DATE_STR, START_MONTH_STR, START_QUARTER_STR};
const char* END_TIME_STR_ARRAY[] = {END_DATE_STR, END_MONTH_STR, END_QUARTER_STR};
