#include <assert.h>
#include <string>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common.h"


using namespace std;

const char* FINANCE_DATABASE_NAME_LIST[] =
{
	"stock_top3_legal_persons_net_buy_or_sell",
	"future_and_option_top3_legal_persons_open_interest",
	"option_top3_legal_persons_buy_and_sell_option_open_interest",
	"future_top10_dealers_and_legal_persons"
};
const int FINANCE_DATABASE_NAME_LIST_LEN = sizeof(FINANCE_DATABASE_NAME_LIST) / sizeof(FINANCE_DATABASE_NAME_LIST[0]);
const char* FINANCE_DATABASE_DESCRIPTION_LIST[] =
{
	"三大法人現貨買賣超",
	"三大法人期貨選擇權留倉淨額",
	"三大法人選擇權買賣權留倉淨額",
	"十大交易人及特定法人期貨資訊"
};
const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DEFINITION[] =
{
	"date", // 日期
	"value1", // 自營商(自行買賣)_買進金額
	"value2", // 自營商(自行買賣)_賣出金額
	"value3", // 自營商(自行買賣)_買賣差額
	"value4", // 自營商(避險)_買進金額
	"value5", // 自營商(避險)_賣出金額
	"value6", // 自營商(避險)_買賣差額
	"value7", // 投信_買進金額
	"value8", // 投信_賣出金額
	"value9", // 投信_買賣差額
	"value10", // 外資及陸資_買進金額
	"value11", // 外資及陸資_賣出金額
	"value12", // 外資及陸資_買賣差額
};
const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[] =
{
	"DATE NOT NULL PRIMARY KEY", // 日期
	"BIGINT", // 自營商(自行買賣)_買進金額
	"BIGINT", // 自營商(自行買賣)_賣出金額
	"BIGINT", // 自營商(自行買賣)_買賣差額
	"BIGINT", // 自營商(避險)_買進金額
	"BIGINT", // 自營商(避險)_賣出金額
	"BIGINT", // 自營商(避險)_買賣差額
	"BIGINT", // 投信_買進金額
	"BIGINT", // 投信_賣出金額
	"BIGINT", // 投信_買賣差額
	"BIGINT", // 外資及陸資_買進金額
	"BIGINT", // 外資及陸資_賣出金額
	"BIGINT", // 外資及陸資_買賣差額
};
const char* FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DEFINITION[] =
{
	"date", // 日期
	"value1", // 自營商_多方_口數 int",
	"value2", // 自營商_多方_契約金額 int",
	"value3", // 自營商_空方_口數 int",
	"value4", // 自營商_空方_契約金額 int",
	"value5", // 自營商_多空淨額_口數 int",
	"value6", // 自營商_多空淨額_契約金額 int",
	"value7", // 投信_多方_口數 int",
	"value8", // 投信_多方_契約金額 int",
	"value9", // 投信_空方_口數 int",
	"value10", // 投信_空方_契約金額 int",
	"value11", // 投信_多空淨額_口數 int",
	"value12", // 投信_多空淨額_契約金額 int",
	"value13", // 外資_多方_口數 int",
	"value14", // 外資_多方_契約金額 int",
	"value15", // 外資_空方_口數 int",
	"value16", // 外資_空方_契約金額 int",
	"value17", // 外資_多空淨額_口數 int",
	"value18", // 外資_多空淨額_契約金額 int",
};
const char* FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
{
	"DATE NOT NULL PRIMARY KEY", // 日期
	"INT", // 自營商_多方_口數 int",
	"INT", // 自營商_多方_契約金額 int",
	"INT", // 自營商_空方_口數 int",
	"INT", // 自營商_空方_契約金額 int",
	"INT", // 自營商_多空淨額_口數 int",
	"INT", // 自營商_多空淨額_契約金額 int",
	"INT", // 投信_多方_口數 int",
	"INT", // 投信_多方_契約金額 int",
	"INT", // 投信_空方_口數 int",
	"INT", // 投信_空方_契約金額 int",
	"INT", // 投信_多空淨額_口數 int",
	"INT", // 投信_多空淨額_契約金額 int",
	"INT", // 外資_多方_口數 int",
	"INT", // 外資_多方_契約金額 int",
	"INT", // 外資_空方_口數 int",
	"INT", // 外資_空方_契約金額 int",
	"INT", // 外資_多空淨額_口數 int",
	"INT", // 外資_多空淨額_契約金額 int",
};
const char* OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_DEFINITION[] =
{
	"date", // 日期
	"value1", // 買權_自營商_買方_口數 int",
	"value2", // 買權_自營商_買方_契約金額 int",
	"value3", // 買權_自營商_賣方_口數 int",
	"value4", // 買權_自營商_賣方_契約金額 int",
	"value5", // 買權_自營商_買賣差額_口數 int",
	"value6", // 買權_自營商_買賣差額_契約金額 int",
	"value7", // 買權_投信_買方_口數 int",
	"value8", // 買權_投信_買方_契約金額 int",
	"value9", // 買權_投信_賣方_口數 int",
	"value10", // 買權_投信_賣方_契約金額 int",
	"value11", // 買權_投信_買賣差額_口數 int",
	"value12", // 買權_投信_買賣差額_契約金額 int",
	"value13", // 買權_外資_買方_口數 int",
	"value14", // 買權_外資_買方_契約金額 int",
	"value15", // 買權_外資_賣方_口數 int",
	"value16", // 買權_外資_賣方_契約金額 int",
	"value17", // 買權_外資_買賣差額_口數 int",
	"value18", // 買權_外資_買賣差額_契約金額 int",
	"value19", // 賣權_自營商_買方_口數 int",
	"value20", // 賣權_自營商_買方_契約金額 int",
	"value21", // 賣權_自營商_賣方_口數 int",
	"value22", // 賣權_自營商_賣方_契約金額 int",
	"value23", // 賣權_自營商_買賣差額_口數 int",
	"value24", // 賣權_自營商_買賣差額_契約金額 int",
	"value25", // 賣權_投信_買方_口數 int",
	"value26", // 賣權_投信_買方_契約金額 int",
	"value27", // 賣權_投信_賣方_口數 int",
	"value28", // 賣權_投信_賣方_契約金額 int",
	"value29", // 賣權_投信_買賣差額_口數 int",
	"value30", // 賣權_投信_買賣差額_契約金額 int",
	"value31", // 賣權_外資_買方_口數 int",
	"value32", // 賣權_外資_買方_契約金額 int",
	"value33", // 賣權_外資_賣方_口數 int",
	"value34", // 賣權_外資_賣方_契約金額 int",
	"value35", // 賣權_外資_買賣差額_口數 int",
	"value36", // 賣權_外資_買賣差額_契約金額 int",
};
const char* OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[] =
{
	"DATE NOT NULL PRIMARY KEY", // 日期
	"INT", // 買權_自營商_買方_口數 int",
	"INT", // 買權_自營商_買方_契約金額 int",
	"INT", // 買權_自營商_賣方_口數 int",
	"INT", // 買權_自營商_賣方_契約金額 int",
	"INT", // 買權_自營商_買賣差額_口數 int",
	"INT", // 買權_自營商_買賣差額_契約金額 int",
	"INT", // 買權_投信_買方_口數 int",
	"INT", // 買權_投信_買方_契約金額 int",
	"INT", // 買權_投信_賣方_口數 int",
	"INT", // 買權_投信_賣方_契約金額 int",
	"INT", // 買權_投信_買賣差額_口數 int",
	"INT", // 買權_投信_買賣差額_契約金額 int",
	"INT", // 買權_外資_買方_口數 int",
	"INT", // 買權_外資_買方_契約金額 int",
	"INT", // 買權_外資_賣方_口數 int",
	"INT", // 買權_外資_賣方_契約金額 int",
	"INT", // 買權_外資_買賣差額_口數 int",
	"INT", // 買權_外資_買賣差額_契約金額 int",
	"INT", // 賣權_自營商_買方_口數 int",
	"INT", // 賣權_自營商_買方_契約金額 int",
	"INT", // 賣權_自營商_賣方_口數 int",
	"INT", // 賣權_自營商_賣方_契約金額 int",
	"INT", // 賣權_自營商_買賣差額_口數 int",
	"INT", // 賣權_自營商_買賣差額_契約金額 int",
	"INT", // 賣權_投信_買方_口數 int",
	"INT", // 賣權_投信_買方_契約金額 int",
	"INT", // 賣權_投信_賣方_口數 int",
	"INT", // 賣權_投信_賣方_契約金額 int",
	"INT", // 賣權_投信_買賣差額_口數 int",
	"INT", // 賣權_投信_買賣差額_契約金額 int",
	"INT", // 賣權_外資_買方_口數 int",
	"INT", // 賣權_外資_買方_契約金額 int",
	"INT", // 賣權_外資_賣方_口數 int",
	"INT", // 賣權_外資_賣方_契約金額 int",
	"INT", // 賣權_外資_買賣差額_口數 int",
	"INT", // 賣權_外資_買賣差額_契約金額 int",
};
const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DEFINITION[] =
{
	"date", // 日期
	"value1", // 臺股期貨_到期月份_買方_前五大交易人合計_部位數
	"value2", // 臺股期貨_到期月份_買方_前五大交易人合計_百分比
	"value3", // 臺股期貨_到期月份_買方_前十大交易人合計_部位數
	"value4", // 臺股期貨_到期月份_買方_前十大交易人合計_百分比
	"value5", // 臺股期貨_到期月份_賣方_前五大交易人合計_部位數
	"value6", // 臺股期貨_到期月份_賣方_前五大交易人合計_百分比
	"value7", // 臺股期貨_到期月份_賣方_前十大交易人合計_部位數
	"value8", // 臺股期貨_到期月份_賣方_前十大交易人合計_百分比
	"value9", // 臺股期貨_到期月份_全市場未沖銷部位數
	"value10", // 臺股期貨_所有契約_買方_前五大交易人合計_部位數
	"value11", // 臺股期貨_所有契約_買方_前五大交易人合計_百分比
	"value12", // 臺股期貨_所有契約_買方_前十大交易人合計_部位數
	"value13", // 臺股期貨_所有契約_買方_前十大交易人合計_百分比
	"value14", // 臺股期貨_所有契約_賣方_前五大交易人合計_部位數
	"value15", // 臺股期貨_所有契約_賣方_前五大交易人合計_百分比
	"value16", // 臺股期貨_所有契約_賣方_前十大交易人合計_部位數
	"value17", // 臺股期貨_所有契約_賣方_前十大交易人合計_百分比
	"value18", // 臺股期貨_所有契約_全市場未沖銷部位數
};
const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[] =
{
	"DATE NOT NULL PRIMARY KEY", // 日期
	"INT", // 臺股期貨_到期月份_買方_前五大交易人合計_部位數
	"FLOAT", // 臺股期貨_到期月份_買方_前五大交易人合計_百分比
	"INT", // 臺股期貨_到期月份_買方_前十大交易人合計_部位數
	"FLOAT", // 臺股期貨_到期月份_買方_前十大交易人合計_百分比
	"INT", // 臺股期貨_到期月份_賣方_前五大交易人合計_部位數
	"FLOAT", // 臺股期貨_到期月份_賣方_前五大交易人合計_百分比
	"INT", // 臺股期貨_到期月份_賣方_前十大交易人合計_部位數
	"FLOAT", // 臺股期貨_到期月份_賣方_前十大交易人合計_百分比
	"INT", // 臺股期貨_到期月份_全市場未沖銷部位數
	"INT", // 臺股期貨_所有契約_買方_前五大交易人合計_部位數
	"FLOAT", // 臺股期貨_所有契約_買方_前五大交易人合計_百分比
	"INT", // 臺股期貨_所有契約_買方_前十大交易人合計_部位數
	"FLOAT", // 臺股期貨_所有契約_買方_前十大交易人合計_百分比
	"INT", // 臺股期貨_所有契約_賣方_前五大交易人合計_部位數
	"FLOAT", // 臺股期貨_所有契約_賣方_前五大交易人合計_百分比
	"INT", // 臺股期貨_所有契約_賣方_前十大交易人合計_部位數
	"FLOAT", // 臺股期貨_所有契約_賣方_前十大交易人合計_百分比
	"INT", // 臺股期貨_所有契約_全市場未沖銷部位數
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return values
const unsigned short RET_SUCCESS = 0;

const unsigned short RET_FAILURE_UNKNOWN = 1;
const unsigned short RET_FAILURE_INVALID_ARGUMENT = 2;
const unsigned short RET_FAILURE_INVALID_POINTER = 3;
const unsigned short RET_FAILURE_INSUFFICIENT_MEMORY = 4;
const unsigned short RET_FAILURE_INCORRECT_OPERATION = 5;
const unsigned short RET_FAILURE_OPEN_FILE = 6;
const unsigned short RET_FAILURE_NOT_FOUND = 7;
const unsigned short RET_FAILURE_INCORRECT_CONFIG = 8;
const unsigned short RET_FAILURE_INCORRECT_PATH = 9;
const unsigned short RET_FAILURE_IO_OPERATION = 10;
const unsigned short RET_FAILURE_HANDLE_THREAD = 11;
const unsigned short RET_FAILURE_SYSTEM_API = 12;
const unsigned short RET_FAILURE_MYSQL = 13;
