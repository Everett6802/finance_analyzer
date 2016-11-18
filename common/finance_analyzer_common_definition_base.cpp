#include <sys/stat.h>
#include <unistd.h>
#include <new>
#include "finance_analyzer_common_definition_base.h"
#include "finance_analyzer_common_definition_market_sql.h"
#include "finance_analyzer_common_definition_stock_sql.h"


using namespace std;

const char* FINANCE_DATABASE_MARKET_NAME = "Market";
const char* FINANCE_DATABASE_STOCK_NAME_FORMAT = "Stock%02d";

const int MARKET_SOURCE_TYPE_INDEX_LENGTH = FinanceSource_MarketEnd - FinanceSource_MarketStart;
const int STOCK_SOURCE_TYPE_INDEX_LENGTH = FinanceSource_StockEnd - FinanceSource_StockStart;

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

const char* FINANCE_TABLE_NAME_LIST[] =
{
	"stock_exchange_and_volume",
	"stock_top3_legal_persons_net_buy_or_sell",
	"stock_margin_trading_and_short_selling",
	"future_and_option_top3_legal_persons_open_interest",
	"future_or_option_top3_legal_persons_open_interest",
	"option_top3_legal_persons_buy_and_sell_option_open_interest",
	"option_put_call_ratio",
	"future_top10_dealers_and_legal_persons"
};
const int FINANCE_TABLE_NAME_LIST_LEN = sizeof(FINANCE_TABLE_NAME_LIST) / sizeof(FINANCE_TABLE_NAME_LIST[0]);
const char* FINANCE_DATABASE_DESCRIPTION_LIST[] =
{
	"臺股指數及成交量",
	"三大法人現貨買賣超",
	"現貨融資融券餘額",
	"三大法人期貨和選擇權留倉淨額",
	"三大法人期貨或選擇權留倉淨額",
	"三大法人選擇權買賣權留倉淨額",
	"三大法人選擇權賣權買權比",
	"十大交易人及特定法人期貨資訊"
};
const int* FINANCE_DATABASE_FIELD_TYPE_LIST[] =
{
	STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION,
	STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION,
	STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION,
	FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION,
	FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION
};
const int FINANCE_DATABASE_FIELD_AMOUNT_LIST[] =
{
	STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION_SIZE,
	STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION_SIZE,
	STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION_SIZE,
	FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE,
	OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION_SIZE,
	FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION_SIZE
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

// const int SOURCE_KEY_FIELD_MASK = 0xFF << 8;
const int SOURCE_KEY_SOURCE_TYPE_INDEX_BIT_OFFSET = 0;
const int SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET = 8;
const int SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET = 24;
const int SOURCE_KEY_SOURCE_TYPE_INDEX_MASK = 0xFF << SOURCE_KEY_SOURCE_TYPE_INDEX_BIT_OFFSET;
const int SOURCE_KEY_COMPANY_CODE_NUMBER_MASK = 0xFFFF << SOURCE_KEY_COMPANY_CODE_NUMBER_BIT_OFFSET;
const int SOURCE_KEY_COMPANY_GROUP_NUMBER_MASK = 0xFF << SOURCE_KEY_COMPANY_GROUP_NUMBER_BIT_OFFSET;
const int NO_SOURCE_TYPE_MARKET_SOURCE_KEY_VALUE = 0;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return values

const unsigned short RET_SUCCESS = 0;

const unsigned short RET_WARN_BASE = 0x1;
const unsigned short RET_WARN_INDEX_DUPLICATE = RET_WARN_BASE + 1;
const unsigned short RET_WARN_INDEX_IGNORE = RET_WARN_BASE + 2;

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
const unsigned short RET_FAILURE_IO_OPERATION = RET_FAILURE_BASE + 12;
const unsigned short RET_FAILURE_HANDLE_THREAD = RET_FAILURE_BASE + 14;
const unsigned short RET_FAILURE_SYSTEM_API = RET_FAILURE_BASE + 14;
const unsigned short RET_FAILURE_MYSQL = RET_FAILURE_BASE + 15;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
bool SHOW_CONSOLE = true;