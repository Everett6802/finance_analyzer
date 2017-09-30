#include "data_reader.h"


using namespace std;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

#define GET_READ_FUNC_PTR(ClassName, FuncName) &ClassName::FuncName
#define GET_READ_BY_OBJECT_FUNC_PTR(ClassName) GET_READ_FUNC_PTR(ClassName, read_by_object)
#define GET_READ_BY_PARAM_FUNC_PTR(ClassName) GET_READ_FUNC_PTR(ClassName, read_by_param)
#define GET_READ_BY_DEFAULT_FUNC_PTR(ClassName) GET_READ_FUNC_PTR(ClassName, read_by_default)

char* DataReaderDescription[] = {"SQL", "CSV"};

DataReader* DataReader::instance = NULL;

DataReader* DataReader::get_instance()
{
	if (instance == NULL)
	{
		pthread_mutex_lock(&mtx);
		if (instance == NULL)
		{
			instance = new DataReader();
			if (instance == NULL)
				throw bad_alloc();
			unsigned short ret = instance->initialize();
			if (CHECK_FAILURE(ret))
			{
				char errmsg[256];
				snprintf(errmsg, 256, "Fail to initialize the DataReader object , due to: %s", get_ret_description(ret));
				throw runtime_error(string(errmsg));
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	instance->add_ref();
	return instance;
}

DataReader::DataReader()
{
	IMPLEMENT_MSG_DUMPER()
	ref_cnt = 0;
}

DataReader::~DataReader()
{
	deinitialize();
	RELEASE_MSG_DUMPER()
}

unsigned short DataReader::initialize()
{
	unsigned short ret = RET_SUCCESS;
	read_by_object_map.insert(make_pair(DataReader_SQL, GET_READ_BY_OBJECT_FUNC_PTR(DataSqlReader)));
	read_by_object_map.insert(make_pair(DataReader_CSV, GET_READ_BY_OBJECT_FUNC_PTR(DataCsvReader)));
	read_by_param_map.insert(make_pair(DataReader_SQL, GET_READ_BY_PARAM_FUNC_PTR(DataSqlReader)));
	read_by_param_map.insert(make_pair(DataReader_CSV, GET_READ_BY_PARAM_FUNC_PTR(DataCsvReader)));
	read_by_default_map.insert(make_pair(DataReader_SQL, GET_READ_BY_DEFAULT_FUNC_PTR(DataSqlReader)));
	read_by_default_map.insert(make_pair(DataReader_CSV, GET_READ_BY_DEFAULT_FUNC_PTR(DataCsvReader)));
	return ret;
}

unsigned short DataReader::deinitialize()
{
	unsigned short ret = RET_SUCCESS;
	if (instance != NULL)
		instance = NULL;
	return ret;
}

int DataReader::add_ref()
{
	__sync_fetch_and_add(&ref_cnt, 1);
	return ref_cnt;
}

int DataReader::release()
{
	__sync_fetch_and_sub(&ref_cnt, 1);
	if (ref_cnt == 0)
	{
		delete this;
		return 0;
	}
	return ref_cnt;
}

unsigned short DataReader::read_by_object(DataReaderType data_reader_type, const PSEARCH_RULE_SET search_rule_set, void* reader_obj, PRESULT_SET_MAP result_set_map)
{
	return (*(read_by_object_map[data_reader_type]))(search_rule_set, reader_obj, result_set_map);
}

unsigned short DataReader::read_by_param(DataReaderType data_reader_type, const PSEARCH_RULE_SET search_rule_set, void* reader_param, PRESULT_SET_MAP result_set_map)
{
	return (*(read_by_param_map[data_reader_type]))(search_rule_set, reader_param, result_set_map);
}

unsigned short DataReader::read_by_default(DataReaderType data_reader_type, const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map)
{
	return (*(read_by_default_map[data_reader_type]))(search_rule_set, result_set_map);
}
