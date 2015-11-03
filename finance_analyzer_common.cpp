#include "finance_analyzer_common.h"


const char* FINANCE_DATABASE_NAME_LIST[] =
{
	"stock_top3_legal_persons_net_buy_or_sell",
	"future_top3_legal_persons_open_interest",
	"future_top10_dealers_and_legal_persons"
};
const int FINANCE_DATABASE_NAME_LIST_LEN = sizeof(FINANCE_DATABASE_NAME_LIST) / sizeof(FINANCE_DATABASE_NAME_LIST[0]);
const char* FINANCE_DATABASE_DESCRIPTION_LIST[] =
{
	"三大法人現貨買賣超",
	"三大法人期貨留倉淨額",
	"十大交易人及特定法人期貨資訊"
};
const char* STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_DEFINITION[] =
{
	"date DATE NOT NULL PRIMARY KEY", // 日期
	"value1 BIGINT", // 自營商(自行買賣)_買進金額
	"value2 BIGINT", // 自營商(自行買賣)_賣出金額
	"value3 BIGINT", // 自營商(自行買賣)_買賣差額
	"value4 BIGINT", // 自營商(避險)_買進金額
	"value5 BIGINT", // 自營商(避險)_賣出金額
	"value6 BIGINT", // 自營商(避險)_買賣差額
	"value7 BIGINT", // 投信_買進金額
	"value8 BIGINT", // 投信_賣出金額
	"value9 BIGINT", // 投信_買賣差額
	"value10 BIGINT", // 外資及陸資_買進金額
	"value11 BIGINT", // 外資及陸資_賣出金額
	"value12 BIGINT", // 外資及陸資_買賣差額
};
const char* FUTURE_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_DEFINITION[] =
{
	"date DATE NOT NULL PRIMARY KEY", // 日期
	"value1 INT", // 自營商_多方_口數 int",
	"value2 INT", // 自營商_多方_契約金額 int",
	"value3 INT", // 自營商_空方_口數 int",
	"value4 INT", // 自營商_空方_契約金額 int",
	"value5 INT", // 自營商_多空淨額_口數 int",
	"value6 INT", // 自營商_多空淨額_契約金額 int",
	"value7 INT", // "投信_多方_口數 int",
	"value8 INT", // 投信_多方_契約金額 int",
	"value9 INT", // 投信_空方_口數 int",
	"value10 INT", // 投信_空方_契約金額 int",
	"value11 INT", // 投信_多空淨額_口數 int",
	"value12 INT", // 投信_多空淨額_契約金額 int",
	"value13 INT", // 外資_多方_口數 int",
	"value14 INT", // 外資_多方_契約金額 int",
	"value15 INT", // 外資_空方_口數 int",
	"value16 INT", // 外資_空方_契約金額 int",
	"value17 INT", // 外資_多空淨額_口數 int",
	"value18 INT", // 外資_多空淨額_契約金額 int",
};
const char* FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_DEFINITION[] =
{
	"date DATE NOT NULL PRIMARY KEY", // 日期
	"value1 INT", // 臺股期貨_到期月份_買方_前五大交易人合計_部位數
	"value2 FLOAT", // 臺股期貨_到期月份_買方_前五大交易人合計_百分比
	"value3 INT", // 臺股期貨_到期月份_買方_前十大交易人合計_部位數
	"value4 FLOAT", // 臺股期貨_到期月份_買方_前十大交易人合計_百分比
	"value5 INT", // 臺股期貨_到期月份_賣方_前五大交易人合計_部位數
	"value6 FLOAT", // 臺股期貨_到期月份_賣方_前五大交易人合計_百分比
	"value7 INT", // 臺股期貨_到期月份_賣方_前十大交易人合計_部位數
	"value8 FLOAT", // 臺股期貨_到期月份_賣方_前十大交易人合計_百分比
	"value9 INT", // 臺股期貨_到期月份_全市場未沖銷部位數
	"value10 INT", // 臺股期貨_所有契約_買方_前五大交易人合計_部位數
	"value11 FLOAT", // 臺股期貨_所有契約_買方_前五大交易人合計_百分比
	"value12 INT", // 臺股期貨_所有契約_買方_前十大交易人合計_部位數
	"value13 FLOAT", // 臺股期貨_所有契約_買方_前十大交易人合計_百分比
	"value14 INT", // 臺股期貨_所有契約_賣方_前五大交易人合計_部位數
	"value15 FLOAT", // 臺股期貨_所有契約_賣方_前五大交易人合計_百分比
	"value16 INT", // 臺股期貨_所有契約_賣方_前十大交易人合計_部位數
	"value17 FLOAT", // 臺股期貨_所有契約_賣方_前十大交易人合計_百分比
	"value18 INT", // 臺股期貨_所有契約_全市場未沖銷部位數
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
