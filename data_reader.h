#ifndef DATA_READER_H
#define DATA_READER_H

#include <map>
#include <string>
#include "common.h"
#include "data_sql_reader.h"
#include "data_csv_reader.h"


#define DECLARE_DATA_READER() PDATA_READER data_reader;
#define IMPLEMENT_DATA_READER() data_reader = DataReader::get_instance();
#define RELEASE_DATA_READER() SAFE_RELEASE(data_reader)

#define DECLARE_AND_IMPLEMENT_STATIC_DATA_READER()\
static PDATA_READER data_reader = DataReader::get_instance();

#define DATA_READ_BY_OBJECT(data_reader_type, search_rule_set, reader_obj, result_set_map)\
data_reader->read_by_object(data_reader_type, search_rule_set, reader_obj, result_set_map);

#define DATA_READ_BY_PARAM(data_reader_type, search_rule_set, reader_param, result_set_map)\
data_reader->read_by_param(data_reader_type, search_rule_set, reader_param, result_set_map);

#define DATA_READ_BY_DEFAULT(data_reader_type, search_rule_set, result_set_map)\
data_reader->read_by_default(data_reader_type, search_rule_set, result_set_map);

enum DataReaderType{DataReader_SQL, DataReader_CSV, DataReaderSize, DataReader_None};
extern char* DataReaderDescription[];

class DataReader
{
	DECLARE_MSG_DUMPER()
private:
	static DataReader* instance;

public:
	static DataReader* get_instance();

private:
	typedef unsigned short (*READ_BY_OBJECT_FUNC_PTR)(const PSEARCH_RULE_SET search_rule_set, void* sql_reader_obj, PRESULT_SET_MAP result_set_map);
	typedef unsigned short (*READ_BY_PARAM_FUNC_PTR)(const PSEARCH_RULE_SET search_rule_set, void* sql_reader_param, PRESULT_SET_MAP result_set_map);
	typedef unsigned short (*READ_BY_DEFAULT_FUNC_PTR)(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);
	typedef std::map<int, READ_BY_OBJECT_FUNC_PTR> READ_BY_OBJECT_MAP;
	typedef std::map<int, READ_BY_PARAM_FUNC_PTR> READ_BY_PARAM_MAP;
	typedef std::map<int, READ_BY_DEFAULT_FUNC_PTR> READ_BY_DEFAULT_MAP;
	READ_BY_OBJECT_MAP read_by_object_map;
	READ_BY_PARAM_MAP read_by_param_map;
	READ_BY_DEFAULT_MAP read_by_default_map;
	volatile int ref_cnt;

	DataReader();
	DataReader(const DataReader&);
	const DataReader& operator=(const DataReader&);
	~DataReader();

	unsigned short initialize();
	unsigned short deinitialize();

public:
	int add_ref();
	int release();

	unsigned short read_by_object(DataReaderType data_reader_type, const PSEARCH_RULE_SET search_rule_set, void* reader_obj, PRESULT_SET_MAP result_set_map);
	unsigned short read_by_param(DataReaderType data_reader_type, const PSEARCH_RULE_SET search_rule_set, void* reader_param, PRESULT_SET_MAP result_set_map);
	unsigned short read_by_default(DataReaderType data_reader_type, const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);
};
typedef DataReader* PDATA_READER;

#endif

