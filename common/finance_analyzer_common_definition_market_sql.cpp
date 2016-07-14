#include "finance_analyzer_common_definition_base.h"
#include "finance_analyzer_common_definition_market_sql.h"


const int STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_LONG, // 成交股數
	FinanceField_LONG, // 成交金額
	FinanceField_INT, // 成交筆數
	FinanceField_FLOAT, // 發行量加權股價指數
	FinanceField_FLOAT, // 漲跌點數
};
const int STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_LONG, // 自營商(自行買賣)_買進金額
	FinanceField_LONG, // 自營商(自行買賣)_賣出金額
	FinanceField_LONG, // 自營商(自行買賣)_買賣差額
	FinanceField_LONG, // 自營商(避險)_買進金額
	FinanceField_LONG, // 自營商(避險)_賣出金額
	FinanceField_LONG, // 自營商(避險)_買賣差額
	FinanceField_LONG, // 投信_買進金額
	FinanceField_LONG, // 投信_賣出金額
	FinanceField_LONG, // 投信_買賣差額
	FinanceField_LONG, // 外資及陸資_買進金額
	FinanceField_LONG, // 外資及陸資_賣出金額
	FinanceField_LONG, // 外資及陸資_買賣差額
};
const int STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 融資(交易單位)_買進
	FinanceField_INT, // 融資(交易單位)_賣出
	FinanceField_INT, // 融資(交易單位)_現金(券)償還
	FinanceField_INT, // 融資(交易單位)_前日餘額
	FinanceField_INT, // 融資(交易單位)_今日餘額
	FinanceField_INT, // 融券(交易單位)_買進
	FinanceField_INT, // 融券(交易單位)_賣出
	FinanceField_INT, // 融券(交易單位)_現金(券)償還
	FinanceField_INT, // 融券(交易單位)_前日餘額
	FinanceField_INT, // 融券(交易單位)_今日餘額
	FinanceField_INT, // 融資金額(仟元)_買進
	FinanceField_INT, // 融資金額(仟元)_賣出
	FinanceField_INT, // 融資金額(仟元)_現金(券)償還
	FinanceField_LONG, // 融資金額(仟元)_前日餘額
	FinanceField_LONG, // 融資金額(仟元)_今日餘額
};
const int FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 自營商_多方_口數 int",
	FinanceField_INT, // 自營商_多方_契約金額 int",
	FinanceField_INT, // 自營商_空方_口數 int",
	FinanceField_INT, // 自營商_空方_契約金額 int",
	FinanceField_INT, // 自營商_多空淨額_口數 int",
	FinanceField_INT, // 自營商_多空淨額_契約金額 int",
	FinanceField_INT, // 投信_多方_口數 int",
	FinanceField_INT, // 投信_多方_契約金額 int",
	FinanceField_INT, // 投信_空方_口數 int",
	FinanceField_INT, // 投信_空方_契約金額 int",
	FinanceField_INT, // 投信_多空淨額_口數 int",
	FinanceField_INT, // 投信_多空淨額_契約金額 int",
	FinanceField_INT, // 外資_多方_口數 int",
	FinanceField_INT, // 外資_多方_契約金額 int",
	FinanceField_INT, // 外資_空方_口數 int",
	FinanceField_INT, // 外資_空方_契約金額 int",
	FinanceField_INT, // 外資_多空淨額_口數 int",
	FinanceField_INT, // 外資_多空淨額_契約金額 int",
};
const int FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 自營商_多方_口數_期貨 int",
	FinanceField_INT, // 自營商_多方_口數_選擇權 int",
	FinanceField_INT, // 自營商_多方_契約金額_期貨 int",
	FinanceField_INT, // 自營商_多方_契約金額_選擇權 int",
	FinanceField_INT, // 自營商_空方_口數_期貨 int",
	FinanceField_INT, // 自營商_空方_口數_選擇權 int",
	FinanceField_INT, // 自營商_空方_契約金額_期貨 int",
	FinanceField_INT, // 自營商_空方_契約金額_選擇權 int",
	FinanceField_INT, // 自營商_多空淨額_口數_期貨 int",
	FinanceField_INT, // 自營商_多空淨額_口數_選擇權 int",
	FinanceField_INT, // 自營商_多空淨額_契約金額_期貨 int",
	FinanceField_INT, // 自營商_多空淨額_契約金額_選擇權 int",
	FinanceField_INT, // 投信_多方_口數_期貨 int",
	FinanceField_INT, // 投信_多方_口數_選擇權 int",
	FinanceField_INT, // 投信_多方_契約金額_期貨 int",
	FinanceField_INT, // 投信_多方_契約金額_選擇權 int",
	FinanceField_INT, // 投信_空方_口數_期貨 int",
	FinanceField_INT, // 投信_空方_口數_選擇權 int",
	FinanceField_INT, // 投信_空方_契約金額_期貨 int",
	FinanceField_INT, // 投信_空方_契約金額_選擇權 int",
	FinanceField_INT, // 投信_多空淨額_口數_期貨 int",
	FinanceField_INT, // 投信_多空淨額_口數_選擇權 int",
	FinanceField_INT, // 投信_多空淨額_契約金額_期貨 int",
	FinanceField_INT, // 投信_多空淨額_契約金額_選擇權 int",
	FinanceField_INT, // 外資_多方_口數_期貨 int",
	FinanceField_INT, // 外資_多方_口數_選擇權 int",
	FinanceField_INT, // 外資_多方_契約金額_期貨 int",
	FinanceField_INT, // 外資_多方_契約金額_選擇權 int",
	FinanceField_INT, // 外資_空方_口數_期貨 int",
	FinanceField_INT, // 外資_空方_口數_選擇權 int",
	FinanceField_INT, // 外資_空方_契約金額_期貨 int",
	FinanceField_INT, // 外資_空方_契約金額_選擇權 int",
	FinanceField_INT, // 外資_多空淨額_口數_期貨 int",
	FinanceField_INT, // 外資_多空淨額_口數_選擇權 int",
	FinanceField_INT, // 外資_多空淨額_契約金額_期貨 int",
	FinanceField_INT, // 外資_多空淨額_契約金額_選擇權 int",
};
const int OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 買權_自營商_買方_口數 int",
	FinanceField_INT, // 買權_自營商_買方_契約金額 int",
	FinanceField_INT, // 買權_自營商_賣方_口數 int",
	FinanceField_INT, // 買權_自營商_賣方_契約金額 int",
	FinanceField_INT, // 買權_自營商_買賣差額_口數 int",
	FinanceField_INT, // 買權_自營商_買賣差額_契約金額 int",
	FinanceField_INT, // 買權_投信_買方_口數 int",
	FinanceField_INT, // 買權_投信_買方_契約金額 int",
	FinanceField_INT, // 買權_投信_賣方_口數 int",
	FinanceField_INT, // 買權_投信_賣方_契約金額 int",
	FinanceField_INT, // 買權_投信_買賣差額_口數 int",
	FinanceField_INT, // 買權_投信_買賣差額_契約金額 int",
	FinanceField_INT, // 買權_外資_買方_口數 int",
	FinanceField_INT, // 買權_外資_買方_契約金額 int",
	FinanceField_INT, // 買權_外資_賣方_口數 int",
	FinanceField_INT, // 買權_外資_賣方_契約金額 int",
	FinanceField_INT, // 買權_外資_買賣差額_口數 int",
	FinanceField_INT, // 買權_外資_買賣差額_契約金額 int",
	FinanceField_INT, // 賣權_自營商_買方_口數 int",
	FinanceField_INT, // 賣權_自營商_買方_契約金額 int",
	FinanceField_INT, // 賣權_自營商_賣方_口數 int",
	FinanceField_INT, // 賣權_自營商_賣方_契約金額 int",
	FinanceField_INT, // 賣權_自營商_買賣差額_口數 int",
	FinanceField_INT, // 賣權_自營商_買賣差額_契約金額 int",
	FinanceField_INT, // 賣權_投信_買方_口數 int",
	FinanceField_INT, // 賣權_投信_買方_契約金額 int",
	FinanceField_INT, // 賣權_投信_賣方_口數 int",
	FinanceField_INT, // 賣權_投信_賣方_契約金額 int",
	FinanceField_INT, // 賣權_投信_買賣差額_口數 int",
	FinanceField_INT, // 賣權_投信_買賣差額_契約金額 int",
	FinanceField_INT, // 賣權_外資_買方_口數 int",
	FinanceField_INT, // 賣權_外資_買方_契約金額 int",
	FinanceField_INT, // 賣權_外資_賣方_口數 int",
	FinanceField_INT, // 賣權_外資_賣方_契約金額 int",
	FinanceField_INT, // 賣權_外資_買賣差額_口數 int",
	FinanceField_INT, // 賣權_外資_買賣差額_契約金額 int",
};
const int OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 賣權成交量
	FinanceField_INT, // 買權成交量
	FinanceField_FLOAT, // 買賣權成交量比率%
	FinanceField_INT, // 賣權未平倉量
	FinanceField_INT, // 買權未平倉量
	FinanceField_FLOAT, // 買賣權未平倉量比率%
};
const int FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 臺股期貨_到期月份_買方_前五大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_買方_前五大交易人比率
	FinanceField_INT, // 臺股期貨_到期月份_買方_前十大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_買方_前十大交易人比率
	FinanceField_INT, // 臺股期貨_到期月份_賣方_前五大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_賣方_前五大交易人比率
	FinanceField_INT, // 臺股期貨_到期月份_賣方_前十大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_賣方_前十大交易人比率
	FinanceField_INT, // 臺股期貨_到期月份_全市場未沖銷部位數
	FinanceField_INT, // 臺股期貨_所有契約_買方_前五大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_買方_前五大交易人比率
	FinanceField_INT, // 臺股期貨_所有契約_買方_前十大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_買方_前十大交易人比率
	FinanceField_INT, // 臺股期貨_所有契約_賣方_前五大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_賣方_前五大交易人比率
	FinanceField_INT, // 臺股期貨_所有契約_賣方_前十大交易人部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_賣方_前十大交易人比率
	FinanceField_INT, // 臺股期貨_所有契約_全市場未沖銷部位數
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* STOCK_EXCHANGE_AND_VALUE_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"成交股數", // FinanceField_LONG
	"成交金額", // FinanceField_LONG
	"成交筆數", // FinanceField_INT
	"發行量加權股價指數", // FinanceField_FLOAT
	"漲跌點數", // FinanceField_FLOAT
};
const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"自營商(自行買賣)_買進金額", // FinanceField_LONG
	"自營商(自行買賣)_賣出金額", // FinanceField_LONG
	"自營商(自行買賣)_買賣差額", // FinanceField_LONG
	"自營商(避險)_買進金額", // FinanceField_LONG
	"自營商(避險)_賣出金額", // FinanceField_LONG
	"自營商(避險)_買賣差額", // FinanceField_LONG
	"投信_買進金額", // FinanceField_LONG
	"投信_賣出金額", // FinanceField_LONG
	"投信_買賣差額", // FinanceField_LONG
	"外資及陸資_買進金額", // FinanceField_LONG
	"外資及陸資_賣出金額", // FinanceField_LONG
	"外資及陸資_買賣差額", // FinanceField_LONG
};
const char* STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"融資(交易單位)_買進", // FinanceField_INT
	"融資(交易單位)_賣出", // FinanceField_INT
	"融資(交易單位)_現金(券)償還", // FinanceField_INT
	"融資(交易單位)_前日餘額", // FinanceField_INT
	"融資(交易單位)_今日餘額", // FinanceField_INT
	"融券(交易單位)_買進", // FinanceField_INT
	"融券(交易單位)_賣出", // FinanceField_INT
	"融券(交易單位)_現金(券)償還", // FinanceField_INT
	"融券(交易單位)_前日餘額", // FinanceField_INT
	"融券(交易單位)_今日餘額", // FinanceField_INT
	"融資金額(仟元)_買進", // FinanceField_INT
	"融資金額(仟元)_賣出", // FinanceField_INT
	"融資金額(仟元)_現金(券)償還", // FinanceField_INT
	"融資金額(仟元)_前日餘額", // FinanceField_LONG
	"融資金額(仟元)_今日餘額", // FinanceField_LONG
};
const char* FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"自營商_多方_口數", // FinanceField_INT,
	"自營商_多方_契約金額", // FinanceField_INT,
	"自營商_空方_口數", // FinanceField_INT,
	"自營商_空方_契約金額", // FinanceField_INT,
	"自營商_多空淨額_口數", // FinanceField_INT,
	"自營商_多空淨額_契約金額", // FinanceField_INT,
	"投信_多方_口數", // FinanceField_INT,
	"投信_多方_契約金額", // FinanceField_INT,
	"投信_空方_口數", // FinanceField_INT,
	"投信_空方_契約金額", // FinanceField_INT,
	"投信_多空淨額_口數", // FinanceField_INT,
	"投信_多空淨額_契約金額", // FinanceField_INT,
	"外資_多方_口數", // FinanceField_INT,
	"外資_多方_契約金額", // FinanceField_INT,
	"外資_空方_口數", // FinanceField_INT,
	"外資_空方_契約金額", // FinanceField_INT,
	"外資_多空淨額_口數", // FinanceField_INT,
	"外資_多空淨額_契約金額", // FinanceField_INT,
};
const char* FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"自營商_多方_口數_期貨", // FinanceField_INT,
	"自營商_多方_口數_選擇權", // FinanceField_INT,
	"自營商_多方_契約金額_期貨", // FinanceField_INT,
	"自營商_多方_契約金額_選擇權", // FinanceField_INT,
	"自營商_空方_口數_期貨", // FinanceField_INT,
	"自營商_空方_口數_選擇權", // FinanceField_INT,
	"自營商_空方_契約金額_期貨", // FinanceField_INT,
	"自營商_空方_契約金額_選擇權", // FinanceField_INT,
	"自營商_多空淨額_口數_期貨", // FinanceField_INT,
	"自營商_多空淨額_口數_選擇權", // FinanceField_INT,
	"自營商_多空淨額_契約金額_期貨", // FinanceField_INT,
	"自營商_多空淨額_契約金額_選擇權", // FinanceField_INT,
	"投信_多方_口數_期貨", // FinanceField_INT,
	"投信_多方_口數_選擇權", // FinanceField_INT,
	"投信_多方_契約金額_期貨", // FinanceField_INT,
	"投信_多方_契約金額_選擇權", // FinanceField_INT,
	"投信_空方_口數_期貨", // FinanceField_INT,
	"投信_空方_口數_選擇權", // FinanceField_INT,
	"投信_空方_契約金額_期貨", // FinanceField_INT,
	"投信_空方_契約金額_選擇權", // FinanceField_INT,
	"投信_多空淨額_口數_期貨", // FinanceField_INT,
	"投信_多空淨額_口數_選擇權", // FinanceField_INT,
	"投信_多空淨額_契約金額_期貨", // FinanceField_INT,
	"投信_多空淨額_契約金額_選擇權", // FinanceField_INT,
	"外資_多方_口數_期貨", // FinanceField_INT,
	"外資_多方_口數_選擇權", // FinanceField_INT,
	"外資_多方_契約金額_期貨", // FinanceField_INT,
	"外資_多方_契約金額_選擇權", // FinanceField_INT,
	"外資_空方_口數_期貨", // FinanceField_INT,
	"外資_空方_口數_選擇權", // FinanceField_INT,
	"外資_空方_契約金額_期貨", // FinanceField_INT,
	"外資_空方_契約金額_選擇權", // FinanceField_INT,
	"外資_多空淨額_口數_期貨", // FinanceField_INT,
	"外資_多空淨額_口數_選擇權", // FinanceField_INT,
	"外資_多空淨額_契約金額_期貨", // FinanceField_INT,
	"外資_多空淨額_契約金額_選擇權", // FinanceField_INT,
};
const char* OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"買權_自營商_買方_口數", // FinanceField_INT,
	"買權_自營商_買方_契約金額", // FinanceField_INT,
	"買權_自營商_賣方_口數", // FinanceField_INT,
	"買權_自營商_賣方_契約金額", // FinanceField_INT,
	"買權_自營商_買賣差額_口數", //FinanceField_INT,
	"買權_自營商_買賣差額_契約金額", // FinanceField_INT,
	"買權_投信_買方_口數", // FinanceField_INT,
	"買權_投信_買方_契約金額", // FinanceField_INT,
	"買權_投信_賣方_口數", // FinanceField_INT,
	"買權_投信_賣方_契約金額", // FinanceField_INT,
	"買權_投信_買賣差額_口數", // FinanceField_INT,
	"買權_投信_買賣差額_契約金額", // FinanceField_INT,
	"買權_外資_買方_口數", // FinanceField_INT,
	"買權_外資_買方_契約金額", // FinanceField_INT,
	"買權_外資_賣方_口數", // FinanceField_INT,
	"買權_外資_賣方_契約金額", // FinanceField_INT,
	"買權_外資_買賣差額_口數", // FinanceField_INT,
	"買權_外資_買賣差額_契約金額", // FinanceField_INT,
	"賣權_自營商_買方_口數", // FinanceField_INT,
	"賣權_自營商_買方_契約金額", // FinanceField_INT,
	"賣權_自營商_賣方_口數", // FinanceField_INT,
	"賣權_自營商_賣方_契約金額", // FinanceField_INT,
	"賣權_自營商_買賣差額_口數", // FinanceField_INT,
	"賣權_自營商_買賣差額_契約金額", // FinanceField_INT,
	"賣權_投信_買方_口數", // FinanceField_INT,
	"賣權_投信_買方_契約金額", // FinanceField_INT,
	"賣權_投信_賣方_口數", // FinanceField_INT,
	"賣權_投信_賣方_契約金額", // FinanceField_INT,
	"賣權_投信_買賣差額_口數", // FinanceField_INT,
	"賣權_投信_買賣差額_契約金額", // FinanceField_INT,
	"賣權_外資_買方_口數", // FinanceField_INT,
	"賣權_外資_買方_契約金額", // FinanceField_INT,
	"賣權_外資_賣方_口數", // FinanceField_INT,
	"賣權_外資_賣方_契約金額", // FinanceField_INT,
	"賣權_外資_買賣差額_口數", // FinanceField_INT,
	"賣權_外資_買賣差額_契約金額", // FinanceField_INT,
};
const char* OPTION_PUT_CALL_RATIO_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"賣權成交量", // FinanceField_INT
	"買權成交量", // FinanceField_INT
	"買賣權成交量比率", // FinanceField_FLOAT
	"賣權未平倉量", // FinanceField_INT
	"買權未平倉量", // FinanceField_INT
	"買賣權未平倉量比率", // FinanceField_FLOAT
};
const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"臺股期貨_到期月份_買方_前五大交易人部位數", // FinanceField_INT
	"臺股期貨_到期月份_買方_前五大交易人比率", // FinanceField_FLOAT
	"臺股期貨_到期月份_買方_前十大交易人部位數", // FinanceField_INT
	"臺股期貨_到期月份_買方_前十大交易人比率", // FinanceField_FLOAT
	"臺股期貨_到期月份_賣方_前五大交易人部位數", // FinanceField_INT
	"臺股期貨_到期月份_賣方_前五大交易人比率", // FinanceField_FLOAT
	"臺股期貨_到期月份_賣方_前十大交易人部位數", // FinanceField_INT
	"臺股期貨_到期月份_賣方_前十大交易人比率", // FinanceField_FLOAT
	"臺股期貨_到期月份_全市場未沖銷部位數", // FinanceField_INT
	"臺股期貨_所有契約_買方_前五大交易人部位數", // FinanceField_INT
	"臺股期貨_所有契約_買方_前五大交易人比率", // FinanceField_FLOAT
	"臺股期貨_所有契約_買方_前十大交易人部位數", // FinanceField_INT
	"臺股期貨_所有契約_買方_前十大交易人比率", // FinanceField_FLOAT
	"臺股期貨_所有契約_賣方_前五大交易人部位數", // FinanceField_INT
	"臺股期貨_所有契約_賣方_前五大交易人比率", // FinanceField_FLOAT
	"臺股期貨_所有契約_賣方_前十大交易人部位數", // FinanceField_INT
	"臺股期貨_所有契約_賣方_前十大交易人比率", // FinanceField_FLOAT
	"臺股期貨_所有契約_全市場未沖銷部位數", // FinanceField_INT
};

//////////////////////////////////////////////////////////////////////////////////

const int STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION_SIZE = sizeof(STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION) / sizeof(STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION[0]);
const int STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION_SIZE = sizeof(STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION) / sizeof(STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[0]);
const int STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION_SIZE = sizeof(STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION) / sizeof(STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION[0]);
const int FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE = sizeof(FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]);
const int FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE = sizeof(FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]);
const int OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION_SIZE = sizeof(OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]);
const int OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION_SIZE = sizeof(OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION) / sizeof(OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION[0]);
const int FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION_SIZE = sizeof(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[0]);
