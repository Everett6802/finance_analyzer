#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common.h"


using namespace std;

const char* DAILY_FINANCE_FILENAME_FORMAT = "daily_finance%04d%02d%02d";
const char* DAILY_FINANCE_EMAIL_TITLE_FORMAT = "daily_finance%04d%02d%02d";
const char* CONFIG_FOLDER_NAME = "conf";
const char* RESULT_FOLDER_NAME = "result";
const char* FINANCE_ANALYZER_CONF_FILENAME = "finance_analyzer.conf";
const char* WORKDAY_CANLENDAR_CONF_FILENAME = ".workday_canlendar.conf";
const char* DATABASE_TIME_RANGE_CONF_FILENAME = ".database_time_range.conf";

const char* MYSQL_TABLE_NAME_BASE = "year";
const char* MYSQL_DATE_FILED_NAME = "date";
const char* MYSQL_FILED_NAME_BASE = "value";

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
	sizeof(STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION) / sizeof(STOCK_EXCHANGE_AND_VALUE_FIELD_TYPE_DEFINITION[0]),
	sizeof(STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION) / sizeof(STOCK_TOP3_LEGAL_PERSONS_NET_BUY_OR_SELL_FIELD_TYPE_DEFINITION[0]),
	sizeof(STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION) / sizeof(STOCK_MARGIN_TRADING_AND_SHORT_SELLING_FIELD_TYPE_DEFINITION[0]),
	sizeof(FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_AND_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]),
	sizeof(FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_OR_OPTION_TOP3_LEGAL_PERSONS_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]),
	sizeof(OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION) / sizeof(OPTION_TOP3_LEGAL_PERSONS_BUY_AND_SELL_OPTION_OPEN_INTEREST_FIELD_TYPE_DEFINITION[0]),
	sizeof(OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION) / sizeof(OPTION_PUT_CALL_RATIO_FIELD_TYPE_DEFINITION[0]),
	sizeof(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION) / sizeof(FUTURE_TOP10_DEALERS_AND_LEGAL_PERSONS_FIELD_TYPE_DEFINITION[0])
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
const unsigned short RET_FAILURE_INCORRECT_CONFIG = RET_FAILURE_BASE + 8;
const unsigned short RET_FAILURE_INCORRECT_PATH = RET_FAILURE_BASE + 9;
const unsigned short RET_FAILURE_IO_OPERATION = RET_FAILURE_BASE + 10;
const unsigned short RET_FAILURE_HANDLE_THREAD = RET_FAILURE_BASE + 11;
const unsigned short RET_FAILURE_SYSTEM_API = RET_FAILURE_BASE + 12;
const unsigned short RET_FAILURE_MYSQL = RET_FAILURE_BASE + 13;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
bool SHOW_CONSOLE = true;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
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
		"Failure_IncorrectConfig",
		"Failure_IncorrectPath",
		"Failure_IOOperation",
		"Failure_HandleThread",
		"Failure_SystemAPI",
		"Failure_MySQL",
	};
	if (CHECK_FAILURE(ret))
		return failure_ret_descriptions[ret - RET_FAILURE_BASE];
	else if (CHECK_WARN(ret))
		return warn_ret_descriptions[ret - RET_WARN_BASE];
	else
		return success_ret_description;
}

bool check_file_exist(const char* filepath)
{
	assert(filepath != NULL && "filepath should NOT be NULL");
	struct stat dummy;
	return (stat(filepath, &dummy) == 0);
}

bool check_config_file_exist(const char* config_filename)
{
	static const int FILE_PATH_SIZE = 256;
	static char file_path[FILE_PATH_SIZE];
	char current_working_directory[FILE_PATH_SIZE];
	getcwd(current_working_directory, FILE_PATH_SIZE);
	snprintf(file_path, FILE_PATH_SIZE, "%s/%s/%s", current_working_directory, CONFIG_FOLDER_NAME, config_filename);
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
	return ((start_index >= 0 && start_index < range_end) ? true : false);
}

bool check_end_index_in_range(int end_index, int range_start, int range_end)
{
	assert(range_start >= 0 && "range_start should be larger than 0");
	assert(range_end >= 1 && "range_end should be larger than 1");
	return ((end_index > 0 && end_index <= range_end) ? true : false);
}
