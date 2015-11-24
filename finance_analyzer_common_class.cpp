#include <assert.h>
#include <string>
#include <stdexcept>
#include <new>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


DECLARE_MSG_DUMPER_PARAM()

using namespace std;

TimeCfg::TimeCfg(const char* cur_time_str)
{
	static const char *DELIM = "-";
	if (cur_time_str == NULL)
		throw invalid_argument(string("cur_time_str should NOT be NULL"));
	snprintf(time_str, 16, "%s", cur_time_str);

	char time_tmp_str[16];
	memset(time_tmp_str, 0x0, sizeof(char) * 16);
	memcpy(time_tmp_str, time_str, sizeof(char) * strlen(time_str));
	char* pch;
	pch = strtok(time_tmp_str, DELIM);
	int count = 0;
	while (pch != NULL)
	{
		switch (count)
		{
		case 0:
			year = atoi(pch);
			break;
		case 1:
			month = atoi(pch);
			break;
		case 2:
			day = atoi(pch);
			break;
		}
//		printf ("%s, %d\n", pch, atoi(pch));
		pch = strtok(NULL, DELIM);
		count++;
	}
	if (count == 2)
		time_type = TIME_MONTH;
	else if (count == 3)
		time_type = TIME_DATE;
	else
	{
		char errmsg[64];
		snprintf(errmsg, 64, "Incorrect time format: %s, count: %d", time_str, count);
		throw invalid_argument(errmsg);
	}
}

TimeCfg::TimeCfg(int cur_year, int cur_month)
{
	year = cur_year;
	month = cur_month;
	snprintf(time_str, 16, "%04d-%02d", year, month);
	time_type = TIME_MONTH;
}

TimeCfg::TimeCfg(int cur_year, int cur_month, int cur_day)
{
	year = cur_year;
	month = cur_month;
	day = cur_day;
	snprintf(time_str, 16, "%04d-%02d-%02d", year, month, day);
	time_type = TIME_DATE;
}

int TimeCfg::get_year()const{return year;}
int TimeCfg::get_month()const{return month;}
int TimeCfg::get_day()const{return day;}
const char* TimeCfg::to_string()const{return time_str;}
bool TimeCfg::is_month_type()const{return (time_type == TIME_MONTH);}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TimeRangeCfg::TimeRangeCfg(const char* time_start_str, const char* time_end_str) :
	time_start_cfg(NULL),
	time_end_cfg(NULL),
	time_range_description(NULL),
	type_is_month(false)
{
	if (time_start_str != NULL)
	{
		time_start_cfg = new TimeCfg(time_start_str);
		if (time_start_cfg == NULL)
			throw bad_alloc();
	}
	if (time_end_str != NULL)
	{
		time_end_cfg = new TimeCfg(time_end_str);
		if (time_end_cfg == NULL)
			throw bad_alloc();
	}

	if (time_start_cfg != NULL && time_end_cfg != NULL)
	{
		if (time_start_cfg->is_month_type() != time_end_cfg->is_month_type())
		{
			char errmsg[64];
			snprintf(errmsg, 64, "The time format is NOT identical, start: %s, end: %s", time_start_cfg->to_string(), time_end_cfg->to_string());
			throw invalid_argument(errmsg);
		}
		type_is_month = time_start_cfg->is_month_type();
	}
	else if (time_start_cfg != NULL)
		type_is_month = time_start_cfg->is_month_type();
	else if (time_end_cfg != NULL)
		type_is_month = time_end_cfg->is_month_type();
	else
		throw invalid_argument("time_start_str and time_end_str should NOT be NULL simultaneously");
}

TimeRangeCfg::TimeRangeCfg(int year_start, int month_start, int year_end, int month_end)
{
	time_start_cfg = new TimeCfg(year_start, month_start);
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(year_end, month_end);
	if (time_end_cfg == NULL)
		throw bad_alloc();
	type_is_month = true;
}

TimeRangeCfg::TimeRangeCfg(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end)
{
	time_start_cfg = new TimeCfg(year_start, month_start, day_start);
	if (time_start_cfg == NULL)
		throw bad_alloc();
	time_end_cfg = new TimeCfg(year_end, month_end, day_end);
	if (time_end_cfg == NULL)
		throw bad_alloc();
	type_is_month = false;
}

TimeRangeCfg::~TimeRangeCfg()
{
	if (time_start_cfg != NULL)
	{
		delete time_start_cfg;
		time_start_cfg = NULL;
	}
	if (time_end_cfg != NULL)
	{
		delete time_end_cfg;
		time_end_cfg = NULL;
	}
	if (time_range_description != NULL)
	{
		delete time_range_description;
		time_range_description = NULL;
	}
}

const char* TimeRangeCfg::to_string()
{
	if (time_range_description != NULL)
	{
		time_range_description = new char[32];
		if (time_range_description == NULL)
			throw bad_alloc();
		if (time_start_cfg != NULL && time_end_cfg != NULL)
			snprintf(time_range_description, 32, "%s:%s", time_start_cfg->to_string(), time_end_cfg->to_string());
		else if (time_start_cfg != NULL)
			snprintf(time_range_description, 32, "%s", time_start_cfg->to_string());
		else if (time_end_cfg != NULL)
			snprintf(time_range_description, 32, "%s", time_end_cfg->to_string());
	}
	return time_range_description;
}

const PTIME_CFG TimeRangeCfg::get_start_time(){return time_start_cfg;}
const PTIME_CFG TimeRangeCfg::get_end_time(){return time_end_cfg;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
int FinanceDataArrayBase<T>::DEF_ARRAY_SIZE = 512;

template <typename T>
FinanceDataArrayBase<T>::FinanceDataArrayBase() :
	array_size(DEF_ARRAY_SIZE),
	array_pos(0)
{
	IMPLEMENT_MSG_DUMPER()
	array_data = (T*)calloc(array_size, sizeof(T));
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
FinanceDataArrayBase<T>::~FinanceDataArrayBase()
{
	if (array_data != NULL)
	{
		free(array_data);
		array_data = NULL;
	}
	RELEASE_MSG_DUMPER()
}

template <typename T>
void FinanceDataArrayBase<T>::alloc_new()
{
	T* array_data_old = array_data;
	array_size <<= 1;
	array_data = (T*)realloc(array_data_old, sizeof(T) * array_size);
	if (array_data == NULL)
		throw bad_alloc();
}

template <typename T>
bool FinanceDataArrayBase<T>::is_empty()const{return (array_pos == 0);}

template <typename T>
int FinanceDataArrayBase<T>::get_size()const{return array_pos;}

template <typename T>
int FinanceDataArrayBase<T>::get_array_size()const{return array_size;}

template <typename T>
const T* FinanceDataArrayBase<T>::get_data_array()const{return array_data;}

template <typename T>
void FinanceDataArrayBase<T>::add(T data)
{
	if (array_pos + 1 >= array_size)
		alloc_new();

	array_data[array_pos++] = data;
}

template <typename T>
const T FinanceDataArrayBase<T>::operator[](int index)const
{
	assert(array_data != NULL && "array_data == NULL");
	if(index >= 0 && index < array_pos)
	{
		char errmsg[32];
		snprintf(errmsg, 32, "index[%d] is out of range: (0, %d)", index, array_pos);
		WRITE_ERROR(errmsg);
		throw out_of_range(errmsg);
	}
	return array_data[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
FinanceDataPtrArrayBase<T>::~FinanceDataPtrArrayBase()
{
	if (FinanceDataArrayBase<T>::array_data != NULL)
	{
		for (int i = 0 ; i < FinanceDataArrayBase<T>::array_pos ; i++)
		{
			free(FinanceDataArrayBase<T>::array_data[i]);
			FinanceDataArrayBase<T>::array_data[i] = NULL;
		}
	}
}

template <typename T>
void FinanceDataPtrArrayBase<T>::add(T data, size_t data_size)
{
	if (FinanceDataArrayBase<T>::array_pos + 1 >= FinanceDataArrayBase<T>::array_size)
		FinanceDataArrayBase<T>::alloc_new();

	T data_new = (T)calloc(data_size, sizeof(char));
	if (data_new == NULL)
		throw bad_alloc();
	memcpy((void*)data_new, (void*)data, sizeof(char) * data_size);
	FinanceDataArrayBase<T>::array_data[FinanceDataArrayBase<T>::array_pos++] = data_new;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QuerySet::QuerySet()
{
	IMPLEMENT_MSG_DUMPER()
}

QuerySet::~QuerySet()
{
	RELEASE_MSG_DUMPER()
}

unsigned short QuerySet::add_query(int source_index, int field_index)
{
	if(source_index < 0 && source_index >= FinanceSourceSize)
	{
		WRITE_ERROR("source_index is out of range in QuerySet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(field_index < 0 && field_index >= FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index])
	{
// If field_index == -1, it means select all field in the table
		if (field_index != -1)
		{
			WRITE_ERROR("field_index is out of range in QuerySet");
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}

	query_set[source_index].push_back(field_index);
	return RET_SUCCESS;
}

const DEQUE_INT& QuerySet::operator[](int index)const
{
	if (index < 0 || index >= FinanceSourceSize)
	{
		char errmsg[64];
		snprintf(errmsg, 64, "The index[%d] is out of range(0, %d)", index , FinanceSourceSize - 1);
		WRITE_ERROR(errmsg);
		throw out_of_range(errmsg);
	}
	return query_set[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned short ResultSet::get_combined_index(int x, int y)
{
	return (unsigned short)(((x & 0xFF) << 8) | (y & 0xFF));
}

unsigned short ResultSet::get_upper_subindex(unsigned short x)
{
	return (unsigned short)((x >> 8) & 0xFF);
}

unsigned short ResultSet::get_lower_subindex(unsigned short x)
{
	return (unsigned short)(x & 0xFF);
}

ResultSet::ResultSet() :
	check_date_data_mode(false),
	date_data_size(0),
	date_date_pos(0),
	int_data_set_size(0),
	long_data_set_size(0),
	float_data_set_size(0)
{
	IMPLEMENT_MSG_DUMPER()
}

ResultSet::~ResultSet()
{
	int data_size = 0;
	if (!int_data_set.empty())
	{
		data_size = int_data_set.size();
		for(int i = 0 ; i < data_size ; i++)
		{
			delete int_data_set[i];
			int_data_set[i] = NULL;
		}
		int_data_set.clear();
	}
	if (!long_data_set.empty())
	{
		data_size = long_data_set.size();
		for(int i = 0 ; i < data_size ; i++)
		{
			delete long_data_set[i];
			long_data_set[i] = NULL;
		}
		long_data_set.clear();
	}
	if (!float_data_set.empty())
	{
		data_size = float_data_set.size();
		for(int i = 0 ; i < data_size ; i++)
		{
			delete float_data_set[i];
			float_data_set[i] = NULL;
		}
		float_data_set.clear();
	}

	RELEASE_MSG_DUMPER()
}

unsigned short ResultSet::add_set(int source_index, int field_index)
{
	if(source_index < 0 && source_index >= FinanceSourceSize)
	{
		WRITE_ERROR("source_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(field_index < 0 && field_index >= FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index])
	{
// If field_index == -1, it means select all field in the table
		if (field_index != -1)
		{
			WRITE_ERROR("field_index is out of range in ResultSet");
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}

	deque<int> field_type_list;
	if (field_index == -1)
	{
		for(int i = 1 ; i < FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_index] ; i++)
			field_type_list.push_back(FINANCE_DATABASE_FIELD_TYPE_LIST[source_index][i]);
	}

	deque<int>::iterator iter = field_type_list.begin();
	while(iter != field_type_list.end())
	{
		field_index = *iter;
		unsigned short key = get_combined_index(source_index, field_index);
		if (data_set_mapping.find(key) != data_set_mapping.end())
		{
			WRITE_FORMAT_ERROR("The key[%d] from (%d, %d) is duplicate", key, source_index, field_index);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		unsigned short value;
//		int next_index = -1;
		switch(FINANCE_DATABASE_FIELD_TYPE_LIST[source_index][field_index])
		{
		case FinanceField_INT:
//			PFINANCE_INT_DATA_ARRAY new_finance_int_data = new FinanceIntDataArray();
//			if (new_finance_int_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_int_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_INT, int_data_set.size());
			int_data_set.push_back(new FinanceIntDataArray());
			int_data_set_size = int_data_set.size();
			break;
		case FinanceField_LONG:
//			PFINANCE_LONG_DATA_ARRAY new_finance_long_data = new FinanceLongDataArray();
//			if (new_finance_long_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_long_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_LONG, long_data_set.size());
			long_data_set.push_back(new FinanceLongDataArray());
			long_data_set_size = long_data_set.size();
			break;
		case FinanceField_FLOAT:
//			PFINANCE_FLOAT_DATA_ARRAY new_finance_float_data = new FinanceFloatDataArray();
//			if (new_finance_float_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_float_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_FLOAT, float_data_set.size());
			float_data_set.push_back(new FinanceFloatDataArray());
			float_data_set_size = float_data_set.size();
			break;
		case FinanceField_DATE:
			WRITE_ERROR("The DATE field type is NOT supported");
			return RET_FAILURE_INVALID_ARGUMENT;
		default:
			WRITE_FORMAT_ERROR("The unsupported field type: %d", FINANCE_DATABASE_FIELD_TYPE_LIST[source_index][field_index]);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		data_set_mapping[key] = value;
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::add_set(int source_index, const DEQUE_INT& field_set)
{
	unsigned short ret = RET_SUCCESS;
	int field_set_size = field_set.size();
	for (int index = 0 ; index < field_set_size ; index++)
	{
		ret = add_set(source_index, field_set[index]);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return ret;
}

unsigned short ResultSet::set_date(char* element_value)
{
	if (check_date_data_mode)
	{
		if (strcmp(date_data[date_date_pos], element_value) != 0)
		{
			WRITE_FORMAT_ERROR("The date(%s, %s) is NOT equal", date_data[date_date_pos], element_value);
			return RET_FAILURE_INCORRECT_OPERATION;
		}
		date_date_pos++;
	}
	else
		date_data.add(element_value, strlen(element_value) + 1);
	return RET_SUCCESS;
}

unsigned short ResultSet::set_data(int source_index, int field_index, char* data_string)
{
	if (data_string == NULL)
	{
		WRITE_ERROR("data_string should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}

	unsigned short key = get_combined_index(source_index, field_index);
	if (data_set_mapping.find(key) == data_set_mapping.end())
	{
		WRITE_FORMAT_ERROR("The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short value = data_set_mapping[key];
	unsigned short field_type_index = get_upper_subindex(value);
	unsigned short field_type_pos = get_lower_subindex(value);
	switch(field_type_index)
	{
	case FinanceField_INT:
		int_data_set[field_type_pos]->add(atoi(data_string));
		break;
	case FinanceField_LONG:
		long_data_set[field_type_pos]->add(atol(data_string));
		break;
	case FinanceField_FLOAT:
		float_data_set[field_type_pos]->add(atof(data_string));
		break;
	default:
		WRITE_FORMAT_ERROR("Unsupported field_type_index: %d", field_type_index);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	return RET_SUCCESS;
}

#define DEFINE_GET_ARRAY_FUNC(n, m)\
const PFINANCE_##m##_DATA_ARRAY ResultSet::get_##n##_array(int source_index, int field_index)const\
{\
	unsigned short key = get_combined_index(source_index, field_index);\
	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.find(key);\
	if (iter == data_set_mapping.end())\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);\
		WRITE_ERROR(errmsg);\
		throw invalid_argument(errmsg);\
	}\
	unsigned short value = iter->second;\
	unsigned short field_type_index = get_upper_subindex(value);\
	if (field_type_index != FinanceField_##m)\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "The field type[%d] is NOT int", field_type_index);\
		WRITE_ERROR(errmsg);\
		throw invalid_argument(errmsg);\
	}\
	unsigned short field_type_pos = get_lower_subindex(value);\
	if (field_type_pos >= int_data_set_size || field_type_pos < 0)\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "The field pos[%d] is out of range", field_type_pos);\
		WRITE_ERROR(errmsg);\
		throw out_of_range(errmsg);\
	}\
	return n##_data_set[field_type_pos];\
}
DEFINE_GET_ARRAY_FUNC(int, INT)
DEFINE_GET_ARRAY_FUNC(long, LONG)
DEFINE_GET_ARRAY_FUNC(float, FLOAT)

#define DEFINE_GET_ARRAY_ELEMENT_FUNC(n, m)\
n ResultSet::get_##n##_array_element(int source_index, int field_index, int index)const\
{\
	PFINANCE_##m##_DATA_ARRAY data_array = get_##n##_array(source_index, field_index);\
	return (*data_array)[index];\
}
DEFINE_GET_ARRAY_ELEMENT_FUNC(int, INT)
DEFINE_GET_ARRAY_ELEMENT_FUNC(long, LONG)
DEFINE_GET_ARRAY_ELEMENT_FUNC(float, FLOAT)

void ResultSet::switch_to_check_date_mode()
{
	check_date_data_mode = true;
	date_date_pos = 0;
}


//int ResultSet::get_int_element(int source_index, int field_index, int index)const
//{
//	unsigned short key = get_combined_index(source_index, field_index);
//	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.find(key);
//	if (iter == data_set_mapping.end())
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);
//		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
//	}
//
//	unsigned short value = iter->second; //data_set_mapping[key];
//	unsigned short field_type_index = get_upper_subindex(value);
//	if (field_type_index != FinanceField_INT)
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The field type[%d] is NOT int", field_type_index);
//		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
//	}
//	unsigned short field_type_pos = get_lower_subindex(value);
//	if (field_type_pos >= int_data_set_size || field_type_pos < 0)
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The field pos[%d] is out of range", field_type_pos);
//		WRITE_ERROR(errmsg);
//		throw out_of_range(errmsg);
//	}
//	return (*int_data_set[field_type_pos])[index];
//}
//
//long ResultSet::get_long_element(int source_index, int field_index, int index)const
//{
//	unsigned short key = get_combined_index(source_index, field_index);
//	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.find(key);
//	if (iter == data_set_mapping.end())
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);
//		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
//	}
//
//	unsigned short value = iter->second; //data_set_mapping[key];
//	unsigned short field_type_index = get_upper_subindex(value);
//	if (field_type_index != FinanceField_LONG)
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The field type[%d] is NOT long", field_type_index);
//		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
//	}
//	unsigned short field_type_pos = get_lower_subindex(value);
//	if (field_type_pos >= int_data_set_size || field_type_pos < 0)
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The field pos[%d] is out of range", field_type_pos);
//		WRITE_ERROR(errmsg);
//		throw out_of_range(errmsg);
//	}
//	return (*long_data_set[field_type_pos])[index];
//}
//
//float ResultSet::get_float_element(int source_index, int field_index, int index)const
//{
//	unsigned short key = get_combined_index(source_index, field_index);
//	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.find(key);
//	if (iter == data_set_mapping.end())
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The key[%d] from (%d, %d) is NOT FOUND", key, source_index, field_index);
//		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
//	}
//
//	unsigned short value = iter->second; //data_set_mapping[key];
//	unsigned short field_type_index = get_upper_subindex(value);
//	if (field_type_index != FinanceField_INT)
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The field type[%d] is NOT float", field_type_index);
//		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
//	}
//	unsigned short field_type_pos = get_lower_subindex(value);
//	if (field_type_pos >= int_data_set_size || field_type_pos < 0)
//	{
//		char errmsg[64];
//		snprintf(errmsg, 64, "The field pos[%d] is out of range", field_type_pos);
//		WRITE_ERROR(errmsg);
//		throw out_of_range(errmsg);
//	}
//	return (*float_data_set[field_type_pos])[index];
//}
