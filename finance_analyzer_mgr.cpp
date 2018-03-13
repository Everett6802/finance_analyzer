#include <assert.h>
#include <stdexcept>
#include <string>
#include <set>
#include "finance_analyzer_mgr.h"
// #include "data_collector.h"
#include "data_calculator.h"
#include "data_output.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// The Manager Base Class
FinanceAnalyzerMgrBase::FinanceAnalyzerMgrBase() :
	// data_collector(NULL),
	finance_data_type(FinanceData_SQL),
	continue_when_non_exist(true),
	csv_root_folderpath(NULL),
	shm_root_folderpath(NULL),
	data_calculator(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_DATA_READER()
// Caution: Data Collector/ Data Calculator is allocated in the derived class
	// IMPLEMENT_WORKDAY_CANLENDAR()
	// IMPLEMENT_DATABASE_TIME_RANGE()
	static const int DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH_LEN = strlen(DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH) + 1; 
	csv_root_folderpath = new char[DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH_LEN];
	if (csv_root_folderpath == NULL)
		throw bad_alloc();
	memcpy(csv_root_folderpath, DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH, sizeof(char) * DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH_LEN);
	static const int DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH_LEN = strlen(DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH) + 1; 
	shm_root_folderpath = new char[DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH_LEN];
	if (shm_root_folderpath == NULL)
		throw bad_alloc();
	memcpy(shm_root_folderpath, DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH, sizeof(char) * DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH_LEN);
}

FinanceAnalyzerMgrBase::~FinanceAnalyzerMgrBase()
{
	if (shm_root_folderpath != NULL)
	{
		delete[] shm_root_folderpath;
		shm_root_folderpath = NULL;
	}
	if (csv_root_folderpath != NULL)
	{
		delete[] csv_root_folderpath;
		csv_root_folderpath = NULL;
	}
	if (data_calculator != NULL)
	{
		delete data_calculator;
		data_calculator = NULL;
	}
	// if (data_collector != NULL)
	// {
	// 	delete data_collector;
	// 	data_collector = NULL;
	// }
	// RELEASE_DATABASE_TIME_RANGE()
	// RELEASE_WORKDAY_CANLENDAR()
	RELEASE_DATA_READER()
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerMgrBase::parse_config()
{
// Open the config file
	char filepath[32];
	snprintf(filepath, 32, "%s/%s", CONFIG_FOLDER_NAME, FINANCE_ANALYZER_CONF_FILENAME);
	FILE* fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the file: %s, due to: %s", filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Parse the config file
	static const int BUF_SIZE = 256;
	char buf[BUF_SIZE];
	ConfigFieldType config_field_type = CONFIG_FIELD_UNKNOWN;
	while (fgets(buf, BUF_SIZE, fp))
	{
		if (buf[0] == '[')
		{
			static const char* config_field[] = {"Unknown", "[email_address]"};
			config_field_type = CONFIG_FIELD_UNKNOWN;
			for (int i = 0 ; i < CONFIG_FIELD_SIZE ; i++)
			{
				if (strncmp(buf, config_field[i], strlen(config_field[i])) == 0)
				{
					WRITE_FORMAT_DEBUG("Parse the parameter in the file: %s", config_field[i]);
					config_field_type = (ConfigFieldType)i;
					break;
				}
			}
			if (config_field_type == CONFIG_FIELD_UNKNOWN)
			{
				WRITE_FORMAT_ERROR("Unknown field title in config[%s]: %s", filepath, buf);
				return RET_FAILURE_INCORRECT_CONFIG;
			}
		}
		else
		{
			int buf_len = strlen(buf);
			if (buf_len != 0)
			{
				if (buf[buf_len - 1] == '\n')
					buf[buf_len - 1] = '\0';
			}
			if (strlen(buf) == 0)
				continue;
			switch(config_field_type)
			{
			case CONFIG_FIELD_EMAIL_ADDRESS:
			{
				string address(buf);
				email_address_list.push_back(address);
			}
			break;
			default:
			{
				WRITE_FORMAT_ERROR("Unsupported field type: %d", config_field_type);
				return RET_FAILURE_INCORRECT_CONFIG;
			}
			break;
			}
		}
	}

// CLose the handle of the config file
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMgrBase::initialize()
{
	unsigned short ret = RET_SUCCESS;
	ret = parse_config();
	if (CHECK_FAILURE(ret))
		return ret;
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerMgrBase::search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)
{
	assert(search_rule_set != NULL && "search_rule_set should NOT be NULL");
	assert(result_set_map != NULL && "result_set_map should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	// void* reader_obj = NULL;
	PISOURCE source_obj = NULL;
	switch (finance_data_type)
	{
		case FinanceData_SQL:
		{
			WRITE_DEBUG("Set the SQL parameters for searching......");
			PSQL_SOURCE sql_source_obj = new SqlSource();
			if (sql_source_obj == NULL)
				throw bad_alloc();
			ret = sql_source_obj->set_continue_when_non_exist(continue_when_non_exist);
			if (CHECK_FAILURE(ret))
				goto OUT;
			source_obj = sql_source_obj;
		}
		break;
		case FinanceData_CSV:
		{
			WRITE_DEBUG("Set the CSV parameters for searching......");
			PCSV_SOURCE csv_source_obj = new CsvSource();
			if (csv_source_obj == NULL)
				throw bad_alloc();
			ret = csv_source_obj->set_continue_when_non_exist(continue_when_non_exist);
			if (CHECK_FAILURE(ret))
				goto OUT;
			ret = csv_source_obj->set_root_folderpath(csv_root_folderpath);
			if (CHECK_FAILURE(ret))
				goto OUT;
			source_obj = csv_source_obj;
		}
		break;
		case FinanceData_SHM:
		{
			WRITE_DEBUG("Set the SHM parameters for searching......");
			PSHM_SOURCE shm_source_obj = new ShmSource();
			if (shm_source_obj == NULL)
				throw bad_alloc();
			ret = shm_source_obj->set_continue_when_non_exist(continue_when_non_exist);
			if (CHECK_FAILURE(ret))
				goto OUT;
			ret = shm_source_obj->set_root_folderpath(shm_root_folderpath);
			if (CHECK_FAILURE(ret))
				goto OUT;
			source_obj = shm_source_obj;
		}
		break;
		default:
		{
			WRITE_FORMAT_DEBUG("Unknown finance data type: %d", finance_data_type);
			throw runtime_error(string("Unknown finance data type"));
		}
		break;
	}
	ret = DATA_READ_BY_OBJECT(finance_data_type, search_rule_set, source_obj, result_set_map);
OUT:
	if (source_obj != NULL)
	{
		delete source_obj;
		source_obj = NULL;
	}
	return ret;
}

void FinanceAnalyzerMgrBase::set_data_type(FinanceDataType data_type){finance_data_type = data_type;}
FinanceDataType FinanceAnalyzerMgrBase::get_data_type()const{return finance_data_type;}

void FinanceAnalyzerMgrBase::set_continue_when_non_exist(bool non_stop){continue_when_non_exist = non_stop;}
bool FinanceAnalyzerMgrBase::is_continue_when_non_exist()const{return continue_when_non_exist;}

unsigned short FinanceAnalyzerMgrBase::set_csv_root_folderpath(const char* root_folderpath)
{
	assert(root_folderpath != NULL && "root_folderpath should NOT be NULL");
	if (csv_root_folderpath != NULL)
		delete[] csv_root_folderpath;
	int root_folderpath_len = strlen(root_folderpath) + 1;
	csv_root_folderpath = new char[root_folderpath_len];
	if (csv_root_folderpath == NULL)
		throw bad_alloc();
	strcpy(csv_root_folderpath, root_folderpath);
	return RET_SUCCESS;
}
	
const char* FinanceAnalyzerMgrBase::get_csv_root_folderpath()const{return csv_root_folderpath;}

unsigned short FinanceAnalyzerMgrBase::set_shm_root_folderpath(const char* root_folderpath)
{
	assert(root_folderpath != NULL && "root_folderpath should NOT be NULL");
	if (shm_root_folderpath != NULL)
		delete[] shm_root_folderpath;
	int root_folderpath_len = strlen(root_folderpath) + 1;
	shm_root_folderpath = new char[root_folderpath_len];
	if (shm_root_folderpath == NULL)
		throw bad_alloc();
	strcpy(shm_root_folderpath, root_folderpath);
	return RET_SUCCESS;
}
	
const char* FinanceAnalyzerMgrBase::get_shm_root_folderpath()const{return shm_root_folderpath;}

#ifdef DO_DEBUG
unsigned short FinanceAnalyzerMgrBase::test()
{
	PTIME_RANGE_CFG time_range_cfg = new TimeRangeCfg(2015, 8, 5, 2015, 11, 11);
	PQUERY_SET query_set = new QuerySet();
//	query_set->add_query(FinanceSource_StockTop3LegalPersonsNetBuyOrSell, -1);
//	query_set->add_query(FinanceSource_FutureTop10DealersAndLegalPersons, 2);
//	query_set->add_query(FinanceSource_FutureTop10DealersAndLegalPersons, 3);
	ADD_QUERY((*query_set), FinanceSource_StockTop3LegalPersonsNetBuyOrSell, -1);
	ADD_QUERY((*query_set), FinanceSource_FutureTop10DealersAndLegalPersons, 2);
	ADD_QUERY((*query_set), FinanceSource_FutureTop10DealersAndLegalPersons, 3);
	query_set->add_query_done();
	PRESULT_SET result_set = new ResultSet();

	unsigned short ret = query(time_range_cfg, query_set, result_set);
	if (CHECK_FAILURE(ret))
		return ret;

	ret = result_set->show_data();
	if (CHECK_FAILURE(ret))
		return ret;

	return RET_SUCCESS;
}
#endif
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Market Manager Class
IFinanceAnalyzerMgr* FinanceAnalyzerMarketMgr::create_instance()
{
	IFinanceAnalyzerMgr* finance_analyzer_mgr = new FinanceAnalyzerMarketMgr();
	return finance_analyzer_mgr;
}

FinanceAnalyzerMarketMgr::FinanceAnalyzerMarketMgr() :
	// market_data_collector(NULL),
	market_data_calculator(NULL)
{
	// market_data_collector = new MarketDataCollector();
	// if (market_data_collector == NULL)
	// 	throw bad_alloc();
	// data_collector = market_data_collector;
	market_data_calculator = new MarketDataCalculator();
	if (market_data_calculator == NULL)
		throw bad_alloc();
	data_calculator = market_data_calculator;
}

FinanceAnalyzerMarketMgr::~FinanceAnalyzerMarketMgr()
{
// Caution: Data Collector/ Data Calculator is released in the base class
	market_data_calculator = NULL;
	// market_data_collector = NULL;
}

unsigned short FinanceAnalyzerMarketMgr::initialize()
{
	WRITE_DEBUG("Initialize FinanceAnalyzerMarketMgr.....");
	unsigned short ret = FinanceAnalyzerMgrBase::initialize();
	// fprintf(stderr, "Initialize FinanceAnalyzerStockinitialize().....\n");
	return ret;
}

// unsigned short FinanceAnalyzerMarketMgr::search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)
// {
// 	WRITE_DEBUG("Initialize FinanceAnalyzerMarketMgr.....");
// 	assert(search_rule_set != NULL && "search_rule_set should NOT be NULL");
// 	assert(result_set_map != NULL && "result_set_map should NOT be NULL");
// 	// fprintf(stderr, "Initialize FinanceAnalyzerMarketMgr.....\n");
// 	void* reader_obj = NULL;
// 	switch (finance_data_type)
// 	{
// 		case FinanceData_SQL:
// 		{
// 			static DataSqlReader sql_reader_obj;
// 			sql_reader_obj.set_continue_when_non_exist(continue_when_non_exist);
// 			reader_obj = (void*)&sql_reader_obj;
// 		}
// 		break;
// 		case FinanceData_CSV:
// 		{
// 			static DataCsvReader csv_reader_obj;
// 			csv_reader_obj.set_root_folderpath(csv_root_folderpath);
// 			csv_reader_obj.set_continue_when_non_exist(continue_when_non_exist);
// 			reader_obj = (void*)&csv_reader_obj;
// 		}
// 		break;
// 		default:
// 		{
// 			WRITE_FORMAT_DEBUG("Unknown finance data type: %d", finance_data_type);
// 			throw runtime_error(string("Unknown finance data type"));
// 		}
// 		break;
// 	}
// 	unsigned short ret = DATA_READ_BY_OBJECT(finance_data_type, search_rule_set, reader_obj, result_set_map);
// 	return ret;
// }

unsigned short FinanceAnalyzerMarketMgr::get_stock_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& stock_support_resistance_string, const char* stock_stock_support_resistance_folderpath, bool show_detail, const char* stock_stock_support_resistance_time_filter, const char* stock_support_resistance_volume_filter)
{
	throw logic_error(string("Only supported in the Stock mode"));
}

/////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// The Stock Manager Class
IFinanceAnalyzerMgr* FinanceAnalyzerStockMgr::create_instance()
{
	IFinanceAnalyzerMgr* finance_analyzer_mgr = new FinanceAnalyzerStockMgr();
	return finance_analyzer_mgr;
}

FinanceAnalyzerStockMgr::FinanceAnalyzerStockMgr() :
	// stock_data_collector(NULL),
	stock_data_calculator(NULL)
{
	// stock_data_collector = new StockDataCollector();
	// if (stock_data_collector == NULL)
	// 	throw bad_alloc();
	// data_collector = stock_data_collector;
	stock_data_calculator = new StockDataCalculator();
	if (stock_data_calculator == NULL)
		throw bad_alloc();
	data_calculator = stock_data_calculator;
}


FinanceAnalyzerStockMgr::~FinanceAnalyzerStockMgr()
{
// Caution: Data Collector/ Data Calculator is released in the base class
	stock_data_calculator = NULL;
	// stock_data_collector = NULL;
}

unsigned short FinanceAnalyzerStockMgr::initialize()
{
	WRITE_DEBUG("Initialize FinanceAnalyzerStockMgr.....");
	unsigned short ret = FinanceAnalyzerMgrBase::initialize();
	// fprintf(stderr, "Initialize FinanceAnalyzerStockinitialize().....\n");
	return ret;
}

// unsigned short FinanceAnalyzerStockMgr::search(PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)
// {
// 	WRITE_DEBUG("Initialize FinanceAnalyzerStockMgr.....");
// 	// fprintf(stderr, "Initialize FinanceAnalyzerMarketMgr.....\n");
// 	return RET_SUCCESS;
// }

unsigned short FinanceAnalyzerStockMgr::get_stock_support_resistance_string(const std::string& company_code_number, float stock_close_price, std::string& stock_support_resistance_string, const char* stock_stock_support_resistance_folderpath, bool show_detail, const char* stock_stock_support_resistance_time_filter, const char* stock_support_resistance_volume_filter)
{
	unsigned short ret = RET_SUCCESS;
// Setup the filepath 
	if (stock_stock_support_resistance_folderpath != NULL)
	{
		ret = stock_data_calculator->set_stock_support_resistance_folderpath(string(stock_stock_support_resistance_folderpath));	
		if (CHECK_FAILURE(ret))
			return ret;
	}
// Get the stock support and resistance string 
	ret = stock_data_calculator->get_stock_support_resistance_string(company_code_number, stock_close_price, stock_support_resistance_string, show_detail, stock_stock_support_resistance_time_filter, stock_support_resistance_volume_filter);
	if (CHECK_FAILURE(ret))
		return ret;
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
