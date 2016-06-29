#include <sys/stat.h>
#include <unistd.h>
#include <new>
#include "finance_analyzer_common_definition.h"


using namespace std;

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

// const int STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_LONG, // 成交股數
// 	FinanceField_LONG, // 成交金額
// 	FinanceField_INT, // 成交筆數
// 	FinanceField_FLOAT, // 發行量加權股價指數
// 	FinanceField_FLOAT, // 漲跌點數
// };
// const int STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_LONG, // 自營商(自行買賣)_買進金額
// 	FinanceField_LONG, // 自營商(自行買賣)_賣出金額
// 	FinanceField_LONG, // 自營商(自行買賣)_買賣差額
// 	FinanceField_LONG, // 自營商(避險)_買進金額
// 	FinanceField_LONG, // 自營商(避險)_賣出金額
// 	FinanceField_LONG, // 自營商(避險)_買賣差額
// 	FinanceField_LONG, // 投信_買進金額
// 	FinanceField_LONG, // 投信_賣出金額
// 	FinanceField_LONG, // 投信_買賣差額
// 	FinanceField_LONG, // 外資及陸資_買進金額
// 	FinanceField_LONG, // 外資及陸資_賣出金額
// 	FinanceField_LONG, // 外資及陸資_買賣差額
// };
// const int STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_INT, // 融資(交易單位)_買進
// 	FinanceField_INT, // 融資(交易單位)_賣出
// 	FinanceField_INT, // 融資(交易單位)_現金(券)償還
// 	FinanceField_INT, // 融資(交易單位)_前日餘額
// 	FinanceField_INT, // 融資(交易單位)_今日餘額
// 	FinanceField_INT, // 融券(交易單位)_買進
// 	FinanceField_INT, // 融券(交易單位)_賣出
// 	FinanceField_INT, // 融券(交易單位)_現金(券)償還
// 	FinanceField_INT, // 融券(交易單位)_前日餘額
// 	FinanceField_INT, // 融券(交易單位)_今日餘額
// 	FinanceField_INT, // 融資金額(仟元)_買進
// 	FinanceField_INT, // 融資金額(仟元)_賣出
// 	FinanceField_INT, // 融資金額(仟元)_現金(券)償還
// 	FinanceField_LONG, // 融資金額(仟元)_前日餘額
// 	FinanceField_LONG, // 融資金額(仟元)_今日餘額
// };
// const int FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_INT, // 自營商_多方_口數 int",
// 	FinanceField_INT, // 自營商_多方_契約金額 int",
// 	FinanceField_INT, // 自營商_空方_口數 int",
// 	FinanceField_INT, // 自營商_空方_契約金額 int",
// 	FinanceField_INT, // 自營商_多空淨額_口數 int",
// 	FinanceField_INT, // 自營商_多空淨額_契約金額 int",
// 	FinanceField_INT, // 投信_多方_口數 int",
// 	FinanceField_INT, // 投信_多方_契約金額 int",
// 	FinanceField_INT, // 投信_空方_口數 int",
// 	FinanceField_INT, // 投信_空方_契約金額 int",
// 	FinanceField_INT, // 投信_多空淨額_口數 int",
// 	FinanceField_INT, // 投信_多空淨額_契約金額 int",
// 	FinanceField_INT, // 外資_多方_口數 int",
// 	FinanceField_INT, // 外資_多方_契約金額 int",
// 	FinanceField_INT, // 外資_空方_口數 int",
// 	FinanceField_INT, // 外資_空方_契約金額 int",
// 	FinanceField_INT, // 外資_多空淨額_口數 int",
// 	FinanceField_INT, // 外資_多空淨額_契約金額 int",
// };
// const int FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_INT, // 自營商_多方_口數_期貨 int",
// 	FinanceField_INT, // 自營商_多方_口數_選擇權 int",
// 	FinanceField_INT, // 自營商_多方_契約金額_期貨 int",
// 	FinanceField_INT, // 自營商_多方_契約金額_選擇權 int",
// 	FinanceField_INT, // 自營商_空方_口數_期貨 int",
// 	FinanceField_INT, // 自營商_空方_口數_選擇權 int",
// 	FinanceField_INT, // 自營商_空方_契約金額_期貨 int",
// 	FinanceField_INT, // 自營商_空方_契約金額_選擇權 int",
// 	FinanceField_INT, // 自營商_多空淨額_口數_期貨 int",
// 	FinanceField_INT, // 自營商_多空淨額_口數_選擇權 int",
// 	FinanceField_INT, // 自營商_多空淨額_契約金額_期貨 int",
// 	FinanceField_INT, // 自營商_多空淨額_契約金額_選擇權 int",
// 	FinanceField_INT, // 投信_多方_口數_期貨 int",
// 	FinanceField_INT, // 投信_多方_口數_選擇權 int",
// 	FinanceField_INT, // 投信_多方_契約金額_期貨 int",
// 	FinanceField_INT, // 投信_多方_契約金額_選擇權 int",
// 	FinanceField_INT, // 投信_空方_口數_期貨 int",
// 	FinanceField_INT, // 投信_空方_口數_選擇權 int",
// 	FinanceField_INT, // 投信_空方_契約金額_期貨 int",
// 	FinanceField_INT, // 投信_空方_契約金額_選擇權 int",
// 	FinanceField_INT, // 投信_多空淨額_口數_期貨 int",
// 	FinanceField_INT, // 投信_多空淨額_口數_選擇權 int",
// 	FinanceField_INT, // 投信_多空淨額_契約金額_期貨 int",
// 	FinanceField_INT, // 投信_多空淨額_契約金額_選擇權 int",
// 	FinanceField_INT, // 外資_多方_口數_期貨 int",
// 	FinanceField_INT, // 外資_多方_口數_選擇權 int",
// 	FinanceField_INT, // 外資_多方_契約金額_期貨 int",
// 	FinanceField_INT, // 外資_多方_契約金額_選擇權 int",
// 	FinanceField_INT, // 外資_空方_口數_期貨 int",
// 	FinanceField_INT, // 外資_空方_口數_選擇權 int",
// 	FinanceField_INT, // 外資_空方_契約金額_期貨 int",
// 	FinanceField_INT, // 外資_空方_契約金額_選擇權 int",
// 	FinanceField_INT, // 外資_多空淨額_口數_期貨 int",
// 	FinanceField_INT, // 外資_多空淨額_口數_選擇權 int",
// 	FinanceField_INT, // 外資_多空淨額_契約金額_期貨 int",
// 	FinanceField_INT, // 外資_多空淨額_契約金額_選擇權 int",
// };
// const int OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_INT, // 買權_自營商_買方_口數 int",
// 	FinanceField_INT, // 買權_自營商_買方_契約金額 int",
// 	FinanceField_INT, // 買權_自營商_賣方_口數 int",
// 	FinanceField_INT, // 買權_自營商_賣方_契約金額 int",
// 	FinanceField_INT, // 買權_自營商_買賣差額_口數 int",
// 	FinanceField_INT, // 買權_自營商_買賣差額_契約金額 int",
// 	FinanceField_INT, // 買權_投信_買方_口數 int",
// 	FinanceField_INT, // 買權_投信_買方_契約金額 int",
// 	FinanceField_INT, // 買權_投信_賣方_口數 int",
// 	FinanceField_INT, // 買權_投信_賣方_契約金額 int",
// 	FinanceField_INT, // 買權_投信_買賣差額_口數 int",
// 	FinanceField_INT, // 買權_投信_買賣差額_契約金額 int",
// 	FinanceField_INT, // 買權_外資_買方_口數 int",
// 	FinanceField_INT, // 買權_外資_買方_契約金額 int",
// 	FinanceField_INT, // 買權_外資_賣方_口數 int",
// 	FinanceField_INT, // 買權_外資_賣方_契約金額 int",
// 	FinanceField_INT, // 買權_外資_買賣差額_口數 int",
// 	FinanceField_INT, // 買權_外資_買賣差額_契約金額 int",
// 	FinanceField_INT, // 賣權_自營商_買方_口數 int",
// 	FinanceField_INT, // 賣權_自營商_買方_契約金額 int",
// 	FinanceField_INT, // 賣權_自營商_賣方_口數 int",
// 	FinanceField_INT, // 賣權_自營商_賣方_契約金額 int",
// 	FinanceField_INT, // 賣權_自營商_買賣差額_口數 int",
// 	FinanceField_INT, // 賣權_自營商_買賣差額_契約金額 int",
// 	FinanceField_INT, // 賣權_投信_買方_口數 int",
// 	FinanceField_INT, // 賣權_投信_買方_契約金額 int",
// 	FinanceField_INT, // 賣權_投信_賣方_口數 int",
// 	FinanceField_INT, // 賣權_投信_賣方_契約金額 int",
// 	FinanceField_INT, // 賣權_投信_買賣差額_口數 int",
// 	FinanceField_INT, // 賣權_投信_買賣差額_契約金額 int",
// 	FinanceField_INT, // 賣權_外資_買方_口數 int",
// 	FinanceField_INT, // 賣權_外資_買方_契約金額 int",
// 	FinanceField_INT, // 賣權_外資_賣方_口數 int",
// 	FinanceField_INT, // 賣權_外資_賣方_契約金額 int",
// 	FinanceField_INT, // 賣權_外資_買賣差額_口數 int",
// 	FinanceField_INT, // 賣權_外資_買賣差額_契約金額 int",
// };
// const int OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_INT, // 賣權成交量
// 	FinanceField_INT, // 買權成交量
// 	FinanceField_FLOAT, // 買賣權成交量比率%
// 	FinanceField_INT, // 賣權未平倉量
// 	FinanceField_INT, // 買權未平倉量
// 	FinanceField_FLOAT, // 買賣權未平倉量比率%
// };
// const int FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[] =
// {
// 	FinanceField_DATE, // 日期
// 	FinanceField_INT, // 臺股期貨_到期月份_買方_前五大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_到期月份_買方_前五大交易人比率
// 	FinanceField_INT, // 臺股期貨_到期月份_買方_前十大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_到期月份_買方_前十大交易人比率
// 	FinanceField_INT, // 臺股期貨_到期月份_賣方_前五大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_到期月份_賣方_前五大交易人比率
// 	FinanceField_INT, // 臺股期貨_到期月份_賣方_前十大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_到期月份_賣方_前十大交易人比率
// 	FinanceField_INT, // 臺股期貨_到期月份_全市場未沖銷部位數
// 	FinanceField_INT, // 臺股期貨_所有契約_買方_前五大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_所有契約_買方_前五大交易人比率
// 	FinanceField_INT, // 臺股期貨_所有契約_買方_前十大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_所有契約_買方_前十大交易人比率
// 	FinanceField_INT, // 臺股期貨_所有契約_賣方_前五大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_所有契約_賣方_前五大交易人比率
// 	FinanceField_INT, // 臺股期貨_所有契約_賣方_前十大交易人部位數
// 	FinanceField_FLOAT, // 臺股期貨_所有契約_賣方_前十大交易人比率
// 	FinanceField_INT, // 臺股期貨_所有契約_全市場未沖銷部位數
// };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* FINANCE_FIELD_TYPE_DESCRIPTION[] = 
{
	"INT", "LONG", "FLOAT", "DATE"
};

// const char* STOCK_EXCHANGE_AND_VALUE_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"成交股數", // FinanceField_LONG
// 	"成交金額", // FinanceField_LONG
// 	"成交筆數", // FinanceField_INT
// 	"發行量加權股價指數", // FinanceField_FLOAT
// 	"漲跌點數", // FinanceField_FLOAT
// };
// const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"自營商(自行買賣)_買進金額", // FinanceField_LONG
// 	"自營商(自行買賣)_賣出金額", // FinanceField_LONG
// 	"自營商(自行買賣)_買賣差額", // FinanceField_LONG
// 	"自營商(避險)_買進金額", // FinanceField_LONG
// 	"自營商(避險)_賣出金額", // FinanceField_LONG
// 	"自營商(避險)_買賣差額", // FinanceField_LONG
// 	"投信_買進金額", // FinanceField_LONG
// 	"投信_賣出金額", // FinanceField_LONG
// 	"投信_買賣差額", // FinanceField_LONG
// 	"外資及陸資_買進金額", // FinanceField_LONG
// 	"外資及陸資_賣出金額", // FinanceField_LONG
// 	"外資及陸資_買賣差額", // FinanceField_LONG
// };
// const char* STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"融資(交易單位)_買進", // FinanceField_INT
// 	"融資(交易單位)_賣出", // FinanceField_INT
// 	"融資(交易單位)_現金(券)償還", // FinanceField_INT
// 	"融資(交易單位)_前日餘額", // FinanceField_INT
// 	"融資(交易單位)_今日餘額", // FinanceField_INT
// 	"融券(交易單位)_買進", // FinanceField_INT
// 	"融券(交易單位)_賣出", // FinanceField_INT
// 	"融券(交易單位)_現金(券)償還", // FinanceField_INT
// 	"融券(交易單位)_前日餘額", // FinanceField_INT
// 	"融券(交易單位)_今日餘額", // FinanceField_INT
// 	"融資金額(仟元)_買進", // FinanceField_INT
// 	"融資金額(仟元)_賣出", // FinanceField_INT
// 	"融資金額(仟元)_現金(券)償還", // FinanceField_INT
// 	"融資金額(仟元)_前日餘額", // FinanceField_LONG
// 	"融資金額(仟元)_今日餘額", // FinanceField_LONG
// };
// const char* FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"自營商_多方_口數", // FinanceField_INT,
// 	"自營商_多方_契約金額", // FinanceField_INT,
// 	"自營商_空方_口數", // FinanceField_INT,
// 	"自營商_空方_契約金額", // FinanceField_INT,
// 	"自營商_多空淨額_口數", // FinanceField_INT,
// 	"自營商_多空淨額_契約金額", // FinanceField_INT,
// 	"投信_多方_口數", // FinanceField_INT,
// 	"投信_多方_契約金額", // FinanceField_INT,
// 	"投信_空方_口數", // FinanceField_INT,
// 	"投信_空方_契約金額", // FinanceField_INT,
// 	"投信_多空淨額_口數", // FinanceField_INT,
// 	"投信_多空淨額_契約金額", // FinanceField_INT,
// 	"外資_多方_口數", // FinanceField_INT,
// 	"外資_多方_契約金額", // FinanceField_INT,
// 	"外資_空方_口數", // FinanceField_INT,
// 	"外資_空方_契約金額", // FinanceField_INT,
// 	"外資_多空淨額_口數", // FinanceField_INT,
// 	"外資_多空淨額_契約金額", // FinanceField_INT,
// };
// const char* FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"自營商_多方_口數_期貨", // FinanceField_INT,
// 	"自營商_多方_口數_選擇權", // FinanceField_INT,
// 	"自營商_多方_契約金額_期貨", // FinanceField_INT,
// 	"自營商_多方_契約金額_選擇權", // FinanceField_INT,
// 	"自營商_空方_口數_期貨", // FinanceField_INT,
// 	"自營商_空方_口數_選擇權", // FinanceField_INT,
// 	"自營商_空方_契約金額_期貨", // FinanceField_INT,
// 	"自營商_空方_契約金額_選擇權", // FinanceField_INT,
// 	"自營商_多空淨額_口數_期貨", // FinanceField_INT,
// 	"自營商_多空淨額_口數_選擇權", // FinanceField_INT,
// 	"自營商_多空淨額_契約金額_期貨", // FinanceField_INT,
// 	"自營商_多空淨額_契約金額_選擇權", // FinanceField_INT,
// 	"投信_多方_口數_期貨", // FinanceField_INT,
// 	"投信_多方_口數_選擇權", // FinanceField_INT,
// 	"投信_多方_契約金額_期貨", // FinanceField_INT,
// 	"投信_多方_契約金額_選擇權", // FinanceField_INT,
// 	"投信_空方_口數_期貨", // FinanceField_INT,
// 	"投信_空方_口數_選擇權", // FinanceField_INT,
// 	"投信_空方_契約金額_期貨", // FinanceField_INT,
// 	"投信_空方_契約金額_選擇權", // FinanceField_INT,
// 	"投信_多空淨額_口數_期貨", // FinanceField_INT,
// 	"投信_多空淨額_口數_選擇權", // FinanceField_INT,
// 	"投信_多空淨額_契約金額_期貨", // FinanceField_INT,
// 	"投信_多空淨額_契約金額_選擇權", // FinanceField_INT,
// 	"外資_多方_口數_期貨", // FinanceField_INT,
// 	"外資_多方_口數_選擇權", // FinanceField_INT,
// 	"外資_多方_契約金額_期貨", // FinanceField_INT,
// 	"外資_多方_契約金額_選擇權", // FinanceField_INT,
// 	"外資_空方_口數_期貨", // FinanceField_INT,
// 	"外資_空方_口數_選擇權", // FinanceField_INT,
// 	"外資_空方_契約金額_期貨", // FinanceField_INT,
// 	"外資_空方_契約金額_選擇權", // FinanceField_INT,
// 	"外資_多空淨額_口數_期貨", // FinanceField_INT,
// 	"外資_多空淨額_口數_選擇權", // FinanceField_INT,
// 	"外資_多空淨額_契約金額_期貨", // FinanceField_INT,
// 	"外資_多空淨額_契約金額_選擇權", // FinanceField_INT,
// };
// const char* OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"買權_自營商_買方_口數", // FinanceField_INT,
// 	"買權_自營商_買方_契約金額", // FinanceField_INT,
// 	"買權_自營商_賣方_口數", // FinanceField_INT,
// 	"買權_自營商_賣方_契約金額", // FinanceField_INT,
// 	"買權_自營商_買賣差額_口數", //FinanceField_INT,
// 	"買權_自營商_買賣差額_契約金額", // FinanceField_INT,
// 	"買權_投信_買方_口數", // FinanceField_INT,
// 	"買權_投信_買方_契約金額", // FinanceField_INT,
// 	"買權_投信_賣方_口數", // FinanceField_INT,
// 	"買權_投信_賣方_契約金額", // FinanceField_INT,
// 	"買權_投信_買賣差額_口數", // FinanceField_INT,
// 	"買權_投信_買賣差額_契約金額", // FinanceField_INT,
// 	"買權_外資_買方_口數", // FinanceField_INT,
// 	"買權_外資_買方_契約金額", // FinanceField_INT,
// 	"買權_外資_賣方_口數", // FinanceField_INT,
// 	"買權_外資_賣方_契約金額", // FinanceField_INT,
// 	"買權_外資_買賣差額_口數", // FinanceField_INT,
// 	"買權_外資_買賣差額_契約金額", // FinanceField_INT,
// 	"賣權_自營商_買方_口數", // FinanceField_INT,
// 	"賣權_自營商_買方_契約金額", // FinanceField_INT,
// 	"賣權_自營商_賣方_口數", // FinanceField_INT,
// 	"賣權_自營商_賣方_契約金額", // FinanceField_INT,
// 	"賣權_自營商_買賣差額_口數", // FinanceField_INT,
// 	"賣權_自營商_買賣差額_契約金額", // FinanceField_INT,
// 	"賣權_投信_買方_口數", // FinanceField_INT,
// 	"賣權_投信_買方_契約金額", // FinanceField_INT,
// 	"賣權_投信_賣方_口數", // FinanceField_INT,
// 	"賣權_投信_賣方_契約金額", // FinanceField_INT,
// 	"賣權_投信_買賣差額_口數", // FinanceField_INT,
// 	"賣權_投信_買賣差額_契約金額", // FinanceField_INT,
// 	"賣權_外資_買方_口數", // FinanceField_INT,
// 	"賣權_外資_買方_契約金額", // FinanceField_INT,
// 	"賣權_外資_賣方_口數", // FinanceField_INT,
// 	"賣權_外資_賣方_契約金額", // FinanceField_INT,
// 	"賣權_外資_買賣差額_口數", // FinanceField_INT,
// 	"賣權_外資_買賣差額_契約金額", // FinanceField_INT,
// };
// const char* OPTION_PUT_CALL_RATIO_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"賣權成交量", // FinanceField_INT
// 	"買權成交量", // FinanceField_INT
// 	"買賣權成交量比率", // FinanceField_FLOAT
// 	"賣權未平倉量", // FinanceField_INT
// 	"買權未平倉量", // FinanceField_INT
// 	"買賣權未平倉量比率", // FinanceField_FLOAT
// };
// const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DESCRIPTION[] =
// {
// 	"日期", // FinanceField_DATE
// 	"臺股期貨_到期月份_買方_前五大交易人部位數", // FinanceField_INT
// 	"臺股期貨_到期月份_買方_前五大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_到期月份_買方_前十大交易人部位數", // FinanceField_INT
// 	"臺股期貨_到期月份_買方_前十大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_到期月份_賣方_前五大交易人部位數", // FinanceField_INT
// 	"臺股期貨_到期月份_賣方_前五大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_到期月份_賣方_前十大交易人部位數", // FinanceField_INT
// 	"臺股期貨_到期月份_賣方_前十大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_到期月份_全市場未沖銷部位數", // FinanceField_INT
// 	"臺股期貨_所有契約_買方_前五大交易人部位數", // FinanceField_INT
// 	"臺股期貨_所有契約_買方_前五大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_所有契約_買方_前十大交易人部位數", // FinanceField_INT
// 	"臺股期貨_所有契約_買方_前十大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_所有契約_賣方_前五大交易人部位數", // FinanceField_INT
// 	"臺股期貨_所有契約_賣方_前五大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_所有契約_賣方_前十大交易人部位數", // FinanceField_INT
// 	"臺股期貨_所有契約_賣方_前十大交易人比率", // FinanceField_FLOAT
// 	"臺股期貨_所有契約_全市場未沖銷部位數", // FinanceField_INT
// };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* FINANCE_DATABASE_NAME_LIST[] =
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
const int FINANCE_DATABASE_NAME_LIST_LEN = sizeof(FINANCE_DATABASE_NAME_LIST) / sizeof(FINANCE_DATABASE_NAME_LIST[0]);
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
