#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <map>
#include <string>
#include "../common.h"
#include "sql_source.h"
#include "csv_source.h"
#include "shm_source.h"


#define DECLARE_DATA_READER() PDATA_READER data_reader;
#define IMPLEMENT_DATA_READER() data_reader = DataSource::get_instance();
#define RELEASE_DATA_READER() SAFE_RELEASE(data_reader)

#define DECLARE_AND_IMPLEMENT_STATIC_DATA_READER()\
static PDATA_READER data_reader = DataSource::get_instance();

#define DATA_READ_BY_OBJECT(finance_data_type, search_rule_set, source_obj, result_set_map)\
data_reader->read_by_object(finance_data_type, search_rule_set, source_obj, result_set_map);

#define DATA_READ_BY_PARAM(finance_data_type, search_rule_set, source_param, result_set_map)\
data_reader->read_by_param(finance_data_type, search_rule_set, source_param, result_set_map);

#define DATA_READ_BY_DEFAULT(finance_data_type, search_rule_set, result_set_map)\
data_reader->read_by_default(finance_data_type, search_rule_set, result_set_map);

class DataSource
{
	DECLARE_MSG_DUMPER()
private:
	static DataSource* instance;

public:
	static DataSource* get_instance();

private:
	typedef unsigned short (*READ_BY_OBJECT_FUNC_PTR)(const PSEARCH_RULE_SET search_rule_set, PISOURCE source_obj, PRESULT_SET_MAP result_set_map);
	typedef unsigned short (*READ_BY_PARAM_FUNC_PTR)(const PSEARCH_RULE_SET search_rule_set, PISOURCE_PARAM source_param, PRESULT_SET_MAP result_set_map);
	typedef unsigned short (*READ_BY_DEFAULT_FUNC_PTR)(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);
	typedef std::map<int, READ_BY_OBJECT_FUNC_PTR> READ_BY_OBJECT_MAP;
	typedef std::map<int, READ_BY_PARAM_FUNC_PTR> READ_BY_PARAM_MAP;
	typedef std::map<int, READ_BY_DEFAULT_FUNC_PTR> READ_BY_DEFAULT_MAP;
	READ_BY_OBJECT_MAP read_by_object_map;
	READ_BY_PARAM_MAP read_by_param_map;
	READ_BY_DEFAULT_MAP read_by_default_map;
	volatile int ref_cnt;

	DataSource();
	DataSource(const DataSource&);
	const DataSource& operator=(const DataSource&);
	~DataSource();

	unsigned short initialize();
	unsigned short deinitialize();

public:
	int add_ref();
	int release();

	unsigned short read_by_object(FinanceDataType finance_data_type, const PSEARCH_RULE_SET search_rule_set, PISOURCE source_obj, PRESULT_SET_MAP result_set_map);
	unsigned short read_by_param(FinanceDataType finance_data_type, const PSEARCH_RULE_SET search_rule_set, PISOURCE_PARAM source_param, PRESULT_SET_MAP result_set_map);
	unsigned short read_by_default(FinanceDataType finance_data_type, const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);
};
typedef DataSource* PDATA_READER;

#endif

