#include <assert.h>
#include <string>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common.h"


using namespace std;

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
const int FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 臺股期貨_到期月份_買方_前五大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_買方_前五大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_到期月份_買方_前十大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_買方_前十大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_到期月份_賣方_前五大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_賣方_前五大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_到期月份_賣方_前十大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_到期月份_賣方_前十大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_到期月份_全市場未沖銷部位數
	FinanceField_INT, // 臺股期貨_所有契約_買方_前五大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_買方_前五大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_所有契約_買方_前十大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_買方_前十大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_所有契約_賣方_前五大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_賣方_前五大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_所有契約_賣方_前十大交易人合計_部位數
	FinanceField_FLOAT, // 臺股期貨_所有契約_賣方_前十大交易人合計_百分比
	FinanceField_INT, // 臺股期貨_所有契約_全市場未沖銷部位數
};

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
const int* FINANCE_DATABASE_FIELD_TYPE_LIST[] =
{
	STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION,
	FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION,
	FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION,
};
const int FINANCE_DATABASE_FIELD_AMOUNT_LIST[] =
{
	sizeof(STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION) / sizeof(STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[0]),
	sizeof(FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]),
	sizeof(OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]),
	sizeof(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[0]),
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
