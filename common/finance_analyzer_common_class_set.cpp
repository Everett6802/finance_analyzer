#include <assert.h>
#include <math.h>
#include <string>
#include <stdexcept>
#include <new>
#include <algorithm>
#include "finance_analyzer_common_class_set.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

QuerySet::const_iterator::const_iterator(INT_INT_DEQUE_MAP_ITER iterator) : iter(iterator){}

QuerySet::const_iterator QuerySet::const_iterator::operator++()
{
	++iter;
	return *this;
}

bool QuerySet::const_iterator::operator==(const const_iterator& another)
{
	if (this == &another)
		return true;
	return iter == another.iter;
}
		
bool QuerySet::const_iterator::operator!=(const const_iterator& another)
{
	return !(*this == another);
}

const PINT_DEQUE QuerySet::const_iterator::operator->()
{
	return (PINT_DEQUE)iter->second;
}

const INT_DEQUE& QuerySet::const_iterator::operator*()
{
	return *((PINT_DEQUE)iter->second);
}

int QuerySet::const_iterator::get_first()const
{
	return (int)iter->first;
}

const PINT_DEQUE QuerySet::const_iterator::get_second()const
{
	return (PINT_DEQUE)iter->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QuerySet::QuerySet() :
	add_done(false)
{
	IMPLEMENT_MSG_DUMPER()
}

QuerySet::~QuerySet()
{
	INT_INT_DEQUE_MAP_ITER iter = source_field_query_map.begin();
	while (iter != source_field_query_map.end())
	{
		PINT_DEQUE field_deque = (PINT_DEQUE)iter->second;
		iter++;
		if (field_deque != NULL)
		{
			delete field_deque;
			field_deque = NULL;
		}
	}
	RELEASE_MSG_DUMPER()
}

unsigned short QuerySet::add_query(int source_type_index, int field_index)
{
	if (add_done)
	{
		WRITE_ERROR("Fail to add another data");
		return RET_FAILURE_INCORRECT_OPERATION;
	}

// Check if the index is out of range
	if(!check_source_type_index_in_range(source_type_index))
	{
		WRITE_ERROR("source_type_index is out of range in QuerySet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	assert(source_field_query_map[source_type_index] != NULL && "source_field_query_map[source_type_index] should NOT be NULL");

	if(!check_field_index_in_range(source_type_index, field_index))
	{

		WRITE_ERROR("field_index is out of range in QuerySet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
// Check the index is duplicate
	INT_DEQUE::iterator it = find(source_field_query_map[source_type_index]->begin(), source_field_query_map[source_type_index]->end(), 10);
	if (it != source_field_query_map[source_type_index]->end())
	{
		WRITE_FORMAT_WARN("Duplicate index: %d in %s", field_index, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index]);
		return RET_WARN_INDEX_DUPLICATE;
	}
// If all fields are selected, it's no need to add extra index
	if (!source_field_query_map[source_type_index]->empty() && (*source_field_query_map[source_type_index])[0] == -1)
	{
		WRITE_FORMAT_WARN("Ignore index: %d in %s, since all fields are selected", field_index, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index]);
		return RET_WARN_INDEX_IGNORE;
	}

// Add the index
	if (field_index == -1)
		source_field_query_map[source_type_index]->clear();
	source_field_query_map[source_type_index]->push_back(field_index);
	return RET_SUCCESS;
}

unsigned short QuerySet::add_query_list(int source_type_index, const PINT_DEQUE field_index_deque)
{
	assert(field_index_deque != NULL && "field_index_deque should NOT be NULL");
	if (add_done)
	{
		WRITE_ERROR("Fail to add another data");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
// Check if the index is out of range
	if(!check_source_type_index_in_range(source_type_index))
	{
		WRITE_ERROR("source_type_index is out of range in QuerySet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	assert(source_field_query_map[source_type_index] != NULL && "source_field_query_map[source_type_index] should NOT be NULL");

	INT_DEQUE::iterator iter = field_index_deque->begin();
	while (iter != field_index_deque->end())
	{
		int field_index = *iter;
		if(!check_field_index_in_range(source_type_index, field_index))
		{
// If field_index == -1, it means select all field in the table
			if (field_index != -1)
			{
				WRITE_ERROR("field_index is out of range in QuerySet");
				return RET_FAILURE_INVALID_ARGUMENT;
			}
		}
// Check the index is duplicate
		INT_DEQUE::iterator it = find(source_field_query_map[source_type_index]->begin(), source_field_query_map[source_type_index]->end(), 10);
		if (it != source_field_query_map[source_type_index]->end())
		{
			WRITE_FORMAT_WARN("Duplicate index: %d in %s", field_index, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index]);
			continue;
		}
// If all fields are selected, it's no need to add extra index
		if (!source_field_query_map[source_type_index]->empty() && (*source_field_query_map[source_type_index])[0] == -1)
		{
			WRITE_FORMAT_WARN("Ignore index: %d in %s, since all fields are selected", field_index, FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index]);
			break;
		}
// Add the index
		if (field_index == -1)
			source_field_query_map[source_type_index]->clear();
		source_field_query_map[source_type_index]->push_back(field_index);
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short QuerySet::add_query_done()
{
	if (add_done)
	{
		WRITE_ERROR("Fail to add another data");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	const_iterator iter = source_field_query_map.begin();
	while (iter != source_field_query_map.end())
	{
		// assert(iter.get_second() != NULL && "iter.get_second() should NOT be NULL");
//  Caution: The iterator is const, can NOT be altered
		// const INT_DEQUE& field_queue = *iter;
		// if (field_queue[0] == -1)
		// {
		// 	field_queue.clear();
		// 	for (int field_index = 1 ; field_index < FINANCE_DATABASE_FIELD_AMOUNT_LIST[i] ; field_index++) // Caution: Don't include the "date" field
		// 		field_queue.push_back(field_index);
		// }
		int source_type_index = iter.get_first();
		assert(source_field_query_map[source_type_index] != NULL && "source_field_query_map[source_type_index] should NOT be NULL");
		PINT_DEQUE field_deque = source_field_query_map[source_type_index];
		if ((*field_deque)[0] == -1)
		{
			field_deque->clear();
			for (int field_index = 1 ; field_index < FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_type_index] ; field_index++) // Caution: Don't include the "date" field
				field_deque->push_back(field_index);
		}
		++iter;
	}
// 	for (int i = 0 ; i < FinanceSourceSize ; i++)
// 	{
// 		assert(source_field_query_map[i] != NULL && "source_field_query_map[source_type_index] should NOT be NULL");
// 		if (source_field_query_map[i]->empty())
// 			continue;
// //		WRITE_FORMAT_DEBUG("Transform the query data[source_type_index: %d]", i);
// 		if ((*source_field_query_map[i])[0] == -1)
// 		{
// 			source_field_query_map[i]->clear();
// 			for (int field_index = 1 ; field_index < FINANCE_DATABASE_FIELD_AMOUNT_LIST[i] ; field_index++) // Caution: Don't include the "date" field
// 				source_field_query_map[i]->push_back(field_index);
// 		}
// 	}
	add_done = true;
	return RET_SUCCESS;
}

int QuerySet::get_size()const{return source_field_query_map.size();}

bool QuerySet::is_add_query_done()const{return add_done;}

unsigned short QuerySet::get_query_sub_set(int source_type_index, QuerySet** query_sub_set)const
{
	assert(query_sub_set != NULL && "query_sub_set should NOT be NULL");
// Check if the index is out of range
	if(!check_source_type_index_in_range(source_type_index))
	{
		WRITE_ERROR("source_type_index is out of range in QuerySet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
// Can't call map::operator[] in const member function, since std doesn't provide the const version of operator[]
	// const PINT_DEQUE field_index_deque = (const PINT_DEQUE)source_field_query_map[source_type_index];
	INT_INT_DEQUE_MAP_CONST_ITER iter = source_field_query_map.find(source_type_index);
	assert(iter != source_field_query_map.end() && "source_field_query_map[source_type_index] should NOT be NULL");
	const PINT_DEQUE field_index_deque = iter->second;
	assert(field_index_deque != NULL && "source_field_query_map[source_type_index] should NOT be NULL");

	PQUERY_SET query_sub_set_tmp = new QuerySet();
	if (query_sub_set_tmp == NULL)
	{
		WRITE_ERROR("Fail to allocate memory: query_sub_set_tmp");
		return RET_FAILURE_INSUFFICIENT_MEMORY;		
	}
	unsigned short ret = query_sub_set_tmp->add_query_list(source_type_index, field_index_deque);
	if (CHECK_FAILURE(ret))
	{
		delete query_sub_set_tmp;
		query_sub_set_tmp = NULL;
		return ret;
	}
	*query_sub_set = query_sub_set_tmp;
	return ret;
}

const INT_DEQUE& QuerySet::operator[](int source_type_index)
{
	if (source_field_query_map.find(source_type_index) == source_field_query_map.end())
	{
		static char errmsg[64];
		snprintf(errmsg, 64, "The index[%d] is NOT found", source_type_index);
		WRITE_ERROR(errmsg);
		throw invalid_argument(errmsg);
	}
	assert(source_field_query_map[source_type_index] != NULL && "source_field_query_map[source_type_index] should NOT be NULL");
	return *source_field_query_map[source_type_index];
}

QuerySet::const_iterator QuerySet::begin() 
{
	return const_iterator(source_field_query_map.begin());
}

QuerySet::const_iterator QuerySet::end() 
{
	return const_iterator(source_field_query_map.end());
}

///////////////////////////////////////////////////////////////////////////////////////

CompanyGroupSet::const_iterator::const_iterator(INT_STRING_DEQUE_MAP_ITER iterator) : iter(iterator){}

CompanyGroupSet::const_iterator CompanyGroupSet::const_iterator::operator++()
{
	++iter;
	return *this;
}

bool CompanyGroupSet::const_iterator::operator==(const const_iterator& another)
{
	if (this == &another)
		return true;
	return iter == another.iter;
}
		
bool CompanyGroupSet::const_iterator::operator!=(const const_iterator& another)
{
	return !(*this == another);
}

const PSTRING_DEQUE CompanyGroupSet::const_iterator::operator->()
{
	return (PSTRING_DEQUE)iter->second;
}

const STRING_DEQUE& CompanyGroupSet::const_iterator::operator*()
{
	return *((PSTRING_DEQUE)iter->second);
}

int CompanyGroupSet::const_iterator::get_first()const
{
	return (int)iter->first;
}

const PSTRING_DEQUE CompanyGroupSet::const_iterator::get_second()const
{
	return (PSTRING_DEQUE)iter->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PINT_STRING_DEQUE_MAP CompanyGroupSet::whole_company_number_in_group_map = NULL;

void CompanyGroupSet::init_whole_company_number_in_group_map()
{
	if (whole_company_number_in_group_map == NULL)
	{
		DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE();
		whole_company_number_in_group_map = new INT_STRING_DEQUE_MAP();
		if (whole_company_number_in_group_map == NULL)
			throw bad_alloc();
		int company_group_size = company_profile->get_company_group_size();
		for (int i = 0 ; i < company_group_size ; i++)
		{
			PSTRING_DEQUE company_number_deque = new STRING_DEQUE();
			if (company_number_deque == NULL)
				throw bad_alloc();
			FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->group_begin(i);
			while (iter != company_profile->group_end(i))
			{
				company_number_deque->push_back((*iter)[COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER]);
				++iter;
			}
			(*whole_company_number_in_group_map)[i] = company_number_deque;
		}	
		RELEASE_COMPANY_PROFILE()
	}
}

int CompanyGroupSet::get_company_group_size()
{
	static int COMPANY_GROUP_SIZE = -1;
	if (COMPANY_GROUP_SIZE == -1)
	{
		DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE();
		COMPANY_GROUP_SIZE = company_profile->get_company_group_size();
		RELEASE_COMPANY_PROFILE()		
	}
	return COMPANY_GROUP_SIZE;
}

CompanyGroupSet::CompanyGroupSet() :
	company_number_in_group_map(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	// IMPLEMENT_COMPANY_PROFILE()
}

CompanyGroupSet::~CompanyGroupSet()
{
	if (company_number_in_group_map != NULL && company_number_in_group_map != whole_company_number_in_group_map)
	{
		INT_STRING_DEQUE_MAP_ITER iter = company_number_in_group_map->begin();

		while (iter != company_number_in_group_map->end())
		{
			PSTRING_DEQUE company_number_deque = (PSTRING_DEQUE)iter->second;
			iter++;
			if (company_number_deque != NULL)
			{
				delete company_number_deque;
				company_number_deque = NULL;
			}
		}
		delete company_number_in_group_map;
		company_number_in_group_map = NULL;
	}
	// RELEASE_COMPANY_PROFILE()
	RELEASE_MSG_DUMPER()
}

unsigned short CompanyGroupSet::add_company_list(int company_group_number, const PSTRING_DEQUE company_code_number_in_group_deque)
{
	if (company_number_in_group_map == NULL)
	{
		company_number_in_group_map = new INT_STRING_DEQUE_MAP();
		if (company_number_in_group_map == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: company_number_in_group_map");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
	}
	STRING_DEQUE_ITER iter;
	if (company_number_in_group_map->find(company_group_number) == company_number_in_group_map->end())
	{
		PSTRING_DEQUE company_number_deque = new STRING_DEQUE();
		if (company_number_deque == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: company_number_deque");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
		(*company_number_in_group_map)[company_group_number] = company_number_deque;		
	}
	else
	{
		if ((*company_number_in_group_map)[company_group_number] == NULL)
		{
			WRITE_FORMAT_ERROR("The company group[%d] has already been set to NULL", company_group_number);
			return RET_FAILURE_INCORRECT_OPERATION;	
		}
	}
	iter = company_code_number_in_group_deque->begin();
	PSTRING_DEQUE company_code_number_in_group_deque_in_map = (*company_number_in_group_map)[company_group_number];
	assert(company_code_number_in_group_deque_in_map != NULL && "company_code_number_in_group_deque_in_map should NOT be NULL");
	while (iter != company_code_number_in_group_deque->end())
	{
		string company_code_number = (string)*iter;
		STRING_DEQUE_ITER iter = company_code_number_in_group_deque_in_map->begin();
		if (find(company_code_number_in_group_deque_in_map->begin(), company_code_number_in_group_deque_in_map->end(), company_code_number) != company_code_number_in_group_deque_in_map->end())
		// if ((*company_number_in_group_map)[company_group_number]->find(company_code_number) != (*company_number_in_group_map)[company_group_number]->end())
		{
			WRITE_FORMAT_WARN("The company code number[%s] has already been added to the group[%d]", company_code_number.c_str(), company_group_number);
			continue;
		}
		(*company_number_in_group_map)[company_group_number]->push_back(company_code_number);
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short CompanyGroupSet::add_company(int company_group_number, std::string company_code_number)
{
	STRING_DEQUE company_code_number_in_group_deque;
	company_code_number_in_group_deque.push_back(company_code_number);
	return add_company_list(company_group_number, &company_code_number_in_group_deque);
}

unsigned short CompanyGroupSet::add_company_group(int company_group_number)
{
	if (company_number_in_group_map == NULL)
	{
		company_number_in_group_map = new INT_STRING_DEQUE_MAP();
		if (company_number_in_group_map == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: company_number_in_group_map");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
	}
	STRING_DEQUE_ITER iter;
	if (company_number_in_group_map->find(company_group_number) != company_number_in_group_map->end())
	{
		if ((*company_number_in_group_map)[company_group_number] != NULL)
		{
			PSTRING_DEQUE company_code_number_in_group_deque = (*company_number_in_group_map)[company_group_number];
			delete company_code_number_in_group_deque;
			WRITE_FORMAT_WARN("Select all company group[%d], ignore the original settings......", company_group_number);
		}
	}
	(*company_number_in_group_map)[company_group_number] = NULL;
	return RET_SUCCESS;
}

const PSTRING_DEQUE CompanyGroupSet::get_company_number_in_group_list(int company_group_index)const
{
	static const int ERRMSG_SIZE = 128;
	static char errmsg[ERRMSG_SIZE];
// // Check if index is Out of range
// 	assert((company_group_index >= 0 && company_group_index < CompanyGroupSet::get_company_group_size()) && "company_group_index should NOT be out-of-range");
// 	{
// 		snprintf(errmsg, ERRMSG_SIZE, "The company group index[%d] is NOT in range [0, %d)", company_group_index, CompanyGroupSet::get_company_group_size());
// 		throw out_of_range(errmsg);
// 	}
	if (company_number_in_group_map == NULL)
	{
		if (whole_company_number_in_group_map == NULL)
			init_whole_company_number_in_group_map();
		company_number_in_group_map = whole_company_number_in_group_map;
	}
// Check if the company group index is added into data structure
	if (company_number_in_group_map->find(company_group_index) == company_number_in_group_map->end())
	{
		snprintf(errmsg, ERRMSG_SIZE, "The company group index[%d] is NOT found in data structure", company_group_index);
		throw invalid_argument(errmsg);
	}
		
	PSTRING_DEQUE company_number_deque = (*company_number_in_group_map)[company_group_index];
	if (company_number_deque == NULL)
	{
		if (whole_company_number_in_group_map == NULL)
			init_whole_company_number_in_group_map();
		company_number_deque = (*whole_company_number_in_group_map)[company_group_index];
	}
	return company_number_deque;
}

CompanyGroupSet::const_iterator CompanyGroupSet::begin() 
{
	if (company_number_in_group_map == NULL)
	{
		if (whole_company_number_in_group_map == NULL)
			init_whole_company_number_in_group_map();
		company_number_in_group_map = whole_company_number_in_group_map;
	}
	return const_iterator(company_number_in_group_map->begin());
}

CompanyGroupSet::const_iterator CompanyGroupSet::end() 
{
	if (company_number_in_group_map == NULL)
	{
		if (whole_company_number_in_group_map == NULL)
			init_whole_company_number_in_group_map();
		company_number_in_group_map = whole_company_number_in_group_map;
	}
	return const_iterator(company_number_in_group_map->end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StockQuerySet::StockQuerySet()
{
}

StockQuerySet::~StockQuerySet()
{
}

unsigned short StockQuerySet::add_company_list(int company_group_number, const PSTRING_DEQUE company_code_number_in_group_deque)
{
	return company_group_set.add_company_list(company_group_number, company_code_number_in_group_deque);
}

unsigned short StockQuerySet::add_company(int company_group_number, std::string company_code_number)
{
	return company_group_set.add_company(company_group_number, company_code_number);
}

unsigned short StockQuerySet::add_company_group(int company_group_number)
{
	return company_group_set.add_company_group(company_group_number);
}

const CompanyGroupSet& StockQuerySet::get_company_group_set()const
{
	return company_group_set;
}

CompanyGroupSet& StockQuerySet::get_company_group_set()
{
	return company_group_set;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataSetAccessParam::DataSetAccessParam(FinanceFieldType new_finance_field_type, int new_array_index, int new_start_index, int new_end_index)
{
	finance_field_type = new_finance_field_type;
	array_index = new_array_index;
	start_index = new_start_index;
	end_index = new_end_index;
}

void DataSetAccessParam::set_finance_field_type(FinanceFieldType new_finance_field_type){finance_field_type = new_finance_field_type;}
FinanceFieldType DataSetAccessParam::get_finance_field_type()const{return finance_field_type;}

void DataSetAccessParam::set_array_index(int new_array_index){array_index = new_array_index;}
int DataSetAccessParam::get_array_index()const{return array_index;}

void DataSetAccessParam::set_start_index(int new_start_index){start_index = new_start_index;}
int DataSetAccessParam::get_start_index()const{return start_index;}

void DataSetAccessParam::set_end_index(int new_end_index){end_index = new_end_index;}
int DataSetAccessParam::get_end_index()const{return end_index;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataSet::DataSet() :
	// check_date_data_mode(false),
	// data_set_mapping_size(0),
	date_data_size(0),
	date_data_cur_pos(0),
	int_data_set_size(0),
	long_data_set_size(0),
	float_data_set_size(0)
{
	IMPLEMENT_MSG_DUMPER()
}

DataSet::~DataSet()
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

unsigned short DataSet::add_int_set()
{
	PFINANCE_INT_DATA_ARRAY new_array = new FinanceIntDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceIntDataArray object");
	new_array->set_type(FinanceField_INT);
	int_data_set.push_back(new_array);
	int_data_set_size = int_data_set.size();

	return RET_SUCCESS;
}

unsigned short DataSet::add_long_set()
{
	PFINANCE_LONG_DATA_ARRAY new_array = new FinanceLongDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceLongDataArray object");
	new_array->set_type(FinanceField_LONG);
	long_data_set.push_back(new_array);
	long_data_set_size = long_data_set.size();

	return RET_SUCCESS;
}

unsigned short DataSet::add_float_set()
{
	PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
	new_array->set_type(FinanceField_FLOAT);
	float_data_set.push_back(new_array);
	float_data_set_size = float_data_set.size();

	return RET_SUCCESS;
}

#define DEFINE_DEEPCOPY_DATA_ARRAY_FUNC(n, m)\
unsigned short DataSet::deepcopy_##n##_data_array(int set_index, const PFINANCE_##m##_DATA_ARRAY src_##n##_data_array)\
{\
	assert(src_##n##_data_array != NULL && "data_array should NOT be NULL");\
	if (set_index < 0 || set_index >= n##_data_set_size)\
	{\
		WRITE_FORMAT_ERROR("The index[%d] is NOT in range[0, %d)", set_index, n##_data_set_size);\
		return RET_FAILURE_INVALID_ARGUMENT;\
	}\
	n##_data_set[set_index]->set_data_array(src_##n##_data_array->get_data_array(), src_##n##_data_array->get_size());\
	return RET_SUCCESS;\
}

DEFINE_DEEPCOPY_DATA_ARRAY_FUNC(int, INT)
DEFINE_DEEPCOPY_DATA_ARRAY_FUNC(long, LONG)
DEFINE_DEEPCOPY_DATA_ARRAY_FUNC(float, FLOAT)

// unsigned short DataSet::deepcopy_int_data_array(int set_index, const PFINANCE_INT_DATA_ARRAY src_int_data_array)
// {
// 	assert(src_int_data_array != NULL && "int_data_array should NOT be NULL");
// 	if (set_index < 0 || set_index >= int_data_set_size)
// 	{
// 		WRITE_FORMAT_ERROR("The index[%d] is NOT in range[0, %d)", set_index, int_data_set_size);
// 		return RET_FAILURE_INVALID_ARGUMENT;
// 	}
// 	int_data_set[set_index]->set_data_array(src_int_data_array->get_data_array(), src_int_data_array->get_size());
// 	return RET_SUCCESS;
// }

// unsigned short DataSet::deepcopy_long_data_array(int set_index, const PFINANCE_LONG_DATA_ARRAY src_long_data_array)
// {
// 	assert(src_long_data_array != NULL && "long_data_array should NOT be NULL");
// 	if (set_index < 0 || set_index >= long_data_set_size)
// 	{
// 		WRITE_FORMAT_ERROR("The index[%d] is NOT in range[0, %d)", set_index, long_data_set_size);
// 		return RET_FAILURE_INVALID_ARGUMENT;
// 	}
// 	long_data_set[set_index]->set_data_array(src_long_data_array->get_data_array(), src_long_data_array->get_size());
// 	return RET_SUCCESS;
// }

// unsigned short DataSet::deepcopy_float_data_array(int set_index, const PFINANCE_FLOAT_DATA_ARRAY src_float_data_array)
// {
// 	assert(src_float_data_array != NULL && "float_data_array should NOT be NULL");
// 	if (set_index < 0 || set_index >= float_data_set_size)
// 	{
// 		WRITE_FORMAT_ERROR("The index[%d] is NOT in range[0, %d)", set_index, float_data_set_size);
// 		return RET_FAILURE_INVALID_ARGUMENT;
// 	}
// 	float_data_set[set_index]->set_data_array(src_float_data_array->get_data_array(), src_float_data_array->get_size());
// 	return RET_SUCCESS;
// }

unsigned short DataSet::deepcopy_data_array(int set_index, const PFINANCE_DATA_ARRAY_BASE src_data_array)
{
	assert(src_data_array != NULL && "src_data_array should NOT be NULL");

	unsigned short ret = RET_SUCCESS;
	switch(src_data_array->get_type())
	{
		case FinanceField_INT:
		{
			ret = deepcopy_int_data_array(set_index, (const PFINANCE_INT_DATA_ARRAY)src_data_array);
		}
		break;
		case FinanceField_LONG:
		{
			ret = deepcopy_long_data_array(set_index, (const PFINANCE_LONG_DATA_ARRAY)src_data_array);
		}
		break;
		case FinanceField_FLOAT:
		{
			ret = deepcopy_float_data_array(set_index, (const PFINANCE_FLOAT_DATA_ARRAY)src_data_array);
		}
		break;
		default:
			WRITE_FORMAT_ERROR("The unsupported field type: %d", src_data_array->get_type());
			ret = RET_FAILURE_INVALID_ARGUMENT;
	}
	return ret;
}

#define DEFINE_GET_DATA_ARRAY_FUNC(n, m)\
const PFINANCE_##m##_DATA_ARRAY DataSet::get_##n##_data_array(int set_index)const\
{\
	static const int BUF_SIZE = 64;\
	static char errmsg[BUF_SIZE];\
	if (set_index >= n##_data_set_size || set_index < 0)\
	{\
		snprintf(errmsg, BUF_SIZE, "The index[%d] is Not in the range[0, %d)", set_index, n##_data_set_size);\
		WRITE_ERROR(errmsg);\
		throw out_of_range(errmsg);\
	}\
	return n##_data_set[set_index];\
}

DEFINE_GET_DATA_ARRAY_FUNC(int, INT)
DEFINE_GET_DATA_ARRAY_FUNC(long, LONG)
DEFINE_GET_DATA_ARRAY_FUNC(float, FLOAT)

#define DEFINE_GET_DATA_ARRAY_ELEMENT_FUNC(n, m)\
n DataSet::get_##n##_data_array_element(int set_index, int index)const\
{\
	PFINANCE_##m##_DATA_ARRAY data_array = get_##n##_data_array(set_index);\
	return (*data_array)[index];\
}

DEFINE_GET_DATA_ARRAY_ELEMENT_FUNC(int, INT)
DEFINE_GET_DATA_ARRAY_ELEMENT_FUNC(long, LONG)
DEFINE_GET_DATA_ARRAY_ELEMENT_FUNC(float, FLOAT)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResultSetAccessParam::ResultSetAccessParam(FinanceSourceType new_finance_source_type, int new_finance_field_no, ArrayElementCalculationType new_calculation_type, int new_start_index, int new_end_index)
{
	finance_source_type = new_finance_source_type;
	finance_field_no = new_finance_field_no;
	calculation_type = new_calculation_type;
	start_index = new_start_index;
	end_index = new_end_index;
}

void ResultSetAccessParam::set_finance_source_type(FinanceSourceType new_finance_source_type){finance_source_type = new_finance_source_type;}
FinanceSourceType ResultSetAccessParam::get_finance_source_type()const{return finance_source_type;}

void ResultSetAccessParam::set_finance_field_no(int new_finance_field_no){finance_field_no = new_finance_field_no;}
int ResultSetAccessParam::get_finance_field_no()const{return finance_field_no;}

void ResultSetAccessParam::set_calculation_type(ArrayElementCalculationType new_calculation_type){calculation_type = new_calculation_type;}
ArrayElementCalculationType ResultSetAccessParam::get_calculation_type()const{return calculation_type;}

void ResultSetAccessParam::set_start_index(int new_start_index){start_index = new_start_index;}
int ResultSetAccessParam::get_start_index()const{return start_index;}

void ResultSetAccessParam::set_end_index(int new_end_index){end_index = new_end_index;}
int ResultSetAccessParam::get_end_index()const{return end_index;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned short ResultSet::get_combined_index(int x, int y)
{
	return (unsigned short)(((x & 0xFF) << 8) | (y & 0xFF));
}

unsigned long ResultSet::get_combined_index_ex(int x, int y, ArrayElementCalculationType calculation_type)
{
	if (calculation_type == ArrayElementCalculation_None)
		return get_combined_index(x, y);
	return (unsigned long)(((calculation_type & 0xFFFF) << 16) | (get_combined_index(x, y) & 0xFFFF));
}

unsigned short ResultSet::get_upper_subindex(unsigned short x)
{
	return (unsigned short)((x >> 8) & 0xFF);
}

unsigned short ResultSet::get_lower_subindex(unsigned short x)
{
	return (unsigned short)(x & 0xFF);
}

unsigned short ResultSet::get_calculation_subindex(unsigned long x)
{
	return (unsigned short)((x >> 16) & 0xFF);
}

void ResultSet::generate_data_for_simulation(ResultSet& result_set)
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15"};
	static const char* field1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}; // Long
	static const char* field2[] = {"-1", "2", "-3", "4", "-5", "6", "-7", "8", "-9", "10"}; // Long;
	static const char* field3[] = {"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10"}; // Int
	static const char* field4[] = {"10.1", "20.1", "30.1", "40.1", "50.1", "60.1", "70.1", "80.1", "90.1", "100.1"}; // Float
	static const char* field5[] = {"1.1", "21.2", "122.3", "199.4", "200.5", "300.6", "420.7", "435.8", "599.4", "600.0"}; // Float;
	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

	char data[32];
	for (int i = 1 ; i < 6 ; i++)
		result_set.add_set(FinanceSource_StockExchangeAndVolume, i);
	for (int i = 0 ; i < DATA_SIZE ; i++)
	{
		snprintf(data, 32, "%s", date[i]);
		result_set.set_date(data);
		snprintf(data, 32, "%s", field1[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 1, data);
		snprintf(data, 32, "%s", field2[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 2, data);
		snprintf(data, 32, "%s", field3[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 3, data);
		snprintf(data, 32, "%s", field4[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 4, data);
		snprintf(data, 32, "%s", field5[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 5, data);
	}
}

void ResultSet::generate_filtered_data_for_simulation(ResultSet& result_set, FinanceBoolDataArray& filter_data_array)
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15", "2016-01-18", "2016-01-19", "2016-01-20", "2016-01-21", "2016-01-22", "2016-01-25", "2016-01-26", "2016-01-27", "2016-01-28", "2016-01-29"};
	static const char* field1[] = {"1", "0", "2", "0", "3", "0", "4", "0", "5", "0", "6", "0", "7", "0", "8", "0", "9", "0", "10", "0"}; // Long
	static const char* field2[] = {"-1", "0", "2", "0", "-3", "0", "4", "0", "-5", "0", "6", "0", "-7", "0", "8", "0", "-9", "0", "10", "0"}; // Long; Dummy
	static const char* field3[] = {"-1", "0", "-2", "0", "-3", "0", "-4", "0", "-5", "0", "-6", "0", "-7", "0", "-8", "0", "-9", "0", "-10", "0"}; // Int
	static const char* field4[] = {"10.1", "0.0", "20.1", "0.0", "30.1", "0.0", "40.1", "0.0", "50.1", "0.0", "60.1", "0.0", "70.1", "0.0", "80.1", "0.0", "90.1", "0.0", "100.1", "0.0"}; // Float
	static const char* field5[] = {"1.1", "0.0", "21.2", "0.0", "122.3", "0.0", "199.4", "0.0", "200.5", "0.0", "300.6", "0.0", "420.7", "0.0", "435.8", "0.0", "599.4", "0.0", "600.0", "0.0"}; // Float; Dummy
	static const bool filter[] = {true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false};
	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

	char data[32];
	for (int i = 1 ; i < 6 ; i++)
		result_set.add_set(FinanceSource_StockExchangeAndVolume, i);
	for (int i = 0 ; i < DATA_SIZE ; i++)
	{
		snprintf(data, 32, "%s", date[i]);
		result_set.set_date(data);
		snprintf(data, 32, "%s", field1[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 1, data);
		snprintf(data, 32, "%s", field2[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 2, data);
		snprintf(data, 32, "%s", field3[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 3, data);
		snprintf(data, 32, "%s", field4[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 4, data);
		snprintf(data, 32, "%s", field5[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 5, data);

		filter_data_array.add(filter[i]);
	}
}

ResultSet::ResultSet() :
	check_date_data_mode(false),
	data_set_mapping_size(0)
{
	// IMPLEMENT_MSG_DUMPER()
}

ResultSet::~ResultSet()
{
	// RELEASE_MSG_DUMPER()
}

unsigned short ResultSet::add_set(int source_type_index, int field_index)
{
	if(!check_source_type_index_in_range(source_type_index))
	{
		WRITE_ERROR("source_type_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(!check_field_index_in_range(source_type_index, field_index))
	{
		WRITE_ERROR("field_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}

	deque<int> field_list;
//	if (field_index != -1)
		field_list.push_back(field_index);
//	else
//	{
//		for(int i = 1 ; i < FINANCE_DATABASE_FIELD_AMOUNT_LIST[source_type_index] ; i++)
//			field_list.push_back(i);
//	}
	deque<int>::iterator iter = field_list.begin();
	while(iter != field_list.end())
	{
		field_index = *iter;
		unsigned short key = get_combined_index(source_type_index, field_index);
		if (data_set_mapping.find(key) != data_set_mapping.end())
		{
			WRITE_FORMAT_ERROR("The key[%d] from (%d, %d) is duplicate", key, source_type_index, field_index);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		unsigned short value;
//		int next_index = -1;
		switch(FINANCE_DATABASE_FIELD_TYPE_LIST[source_type_index][field_index])
		{
		case FinanceField_INT:
		{
//			PFINANCE_INT_DATA_ARRAY new_finance_int_data = new FinanceIntDataArray();
//			if (new_finance_int_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_int_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_INT, int_data_set.size());
			add_int_set();
		}
			break;
		case FinanceField_LONG:
		{
//			PFINANCE_LONG_DATA_ARRAY new_finance_long_data = new FinanceLongDataArray();
//			if (new_finance_long_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_long_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_LONG, long_data_set.size());
			add_long_set();
		}
			break;
		case FinanceField_FLOAT:
		{
//			PFINANCE_FLOAT_DATA_ARRAY new_finance_float_data = new FinanceFloatDataArray();
//			if (new_finance_float_data == NULL)
//			{
//				WRITE_ERROR("Fail to allocate memory: new_finance_float_data");
//				return RET_FAILURE_INSUFFICIENT_MEMORY;
//			}
			value = get_combined_index(FinanceField_FLOAT, float_data_set.size());
			add_float_set();
		}
			break;
		case FinanceField_DATE:
			WRITE_ERROR("The DATE field type is NOT supported");
			return RET_FAILURE_INVALID_ARGUMENT;
		default:
			WRITE_FORMAT_ERROR("The unsupported field type: %d", FINANCE_DATABASE_FIELD_TYPE_LIST[source_type_index][field_index]);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		data_set_mapping[key] = value;
		data_set_mapping_size++;
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::add_set(int source_type_index, const INT_DEQUE& field_set)
{
	unsigned short ret = RET_SUCCESS;
	int field_set_size = field_set.size();
	for (int index = 0 ; index < field_set_size ; index++)
	{
		ret = add_set(source_type_index, field_set[index]);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return ret;
}

unsigned short ResultSet::add_calculation_set_dummy(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex)
{
	assert("No need to do the element calculation");
	return RET_SUCCESS;
}

unsigned short ResultSet::add_calculation_set_diff(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	unsigned short value;
	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY new_array = new FinanceIntDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceIntDataArray object");
		new_array->set_type(FinanceField_INT);

		value = get_combined_index(FinanceField_INT, int_data_set.size());

		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_diff_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		int_data_set.push_back(new_array);
		int_data_set_size = int_data_set.size();
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY new_array = new FinanceLongDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceLongDataArray object");
		new_array->set_type(FinanceField_LONG);

		value = get_combined_index(FinanceField_LONG, long_data_set.size());

		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_diff_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		long_data_set.push_back(new_array);
		long_data_set_size = long_data_set.size();
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
		new_array->set_type(FinanceField_FLOAT);

		value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_diff_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		float_data_set.push_back(new_array);
		float_data_set_size = float_data_set.size();
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_rate(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
	new_array->set_type(FinanceField_FLOAT);

	unsigned short value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_rate_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_rate_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_rate_array(*new_array, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	float_data_set.push_back(new_array);
	float_data_set_size = float_data_set.size();

	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_sum(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	unsigned short value;

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY new_array = new FinanceIntDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceIntDataArray object");
		new_array->set_type(FinanceField_INT);

		value = get_combined_index(FinanceField_INT, int_data_set.size());

		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_sum_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		int_data_set.push_back(new_array);
		int_data_set_size = int_data_set.size();
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY new_array = new FinanceLongDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceLongDataArray object");
		new_array->set_type(FinanceField_LONG);

		value = get_combined_index(FinanceField_LONG, long_data_set.size());

		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_sum_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		long_data_set.push_back(new_array);
		long_data_set_size = long_data_set.size();
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
		assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
		new_array->set_type(FinanceField_FLOAT);

		value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_sum_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
		float_data_set.push_back(new_array);
		float_data_set_size = float_data_set.size();
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_sum5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 5);}
unsigned short ResultSet::add_calculation_set_sum10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 10);}
unsigned short ResultSet::add_calculation_set_sum20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 20);}
unsigned short ResultSet::add_calculation_set_sum60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 60);}

unsigned short ResultSet::add_calculation_set_avg(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
	new_array->set_type(FinanceField_FLOAT);

	unsigned short value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	float_data_set.push_back(new_array);
	float_data_set_size = float_data_set.size();

	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_avg5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 5);}
unsigned short ResultSet::add_calculation_set_avg10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 10);}
unsigned short ResultSet::add_calculation_set_avg20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 20);}
unsigned short ResultSet::add_calculation_set_avg60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 60);}

unsigned short ResultSet::add_calculation_set_weighted_avg(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n)
{
	assert(data_array_base != NULL && "data_array_base should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	PFINANCE_FLOAT_DATA_ARRAY new_array = new FinanceFloatDataArray();
	assert(new_array != NULL && "Fail to allocate the FinanceFloatDataArray object");
	new_array->set_type(FinanceField_FLOAT);

	unsigned short value = get_combined_index(FinanceField_FLOAT, float_data_set.size());

	switch (data_array_base->get_type())
	{
	case FinanceField_INT:
	{
		PFINANCE_INT_DATA_ARRAY data_array = (PFINANCE_INT_DATA_ARRAY)data_array_base;
		ret = data_array->get_weighted_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_LONG:
	{
		PFINANCE_LONG_DATA_ARRAY data_array = (PFINANCE_LONG_DATA_ARRAY)data_array_base;
		ret = data_array->get_weighted_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	case FinanceField_FLOAT:
	{
		PFINANCE_FLOAT_DATA_ARRAY data_array = (PFINANCE_FLOAT_DATA_ARRAY)data_array_base;
		ret = data_array->get_weighted_avg_array(*new_array, n, 0);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	break;
	default:
		WRITE_FORMAT_ERROR("The unsupported field type: %d", data_array_base->get_type());
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	float_data_set.push_back(new_array);
	float_data_set_size = float_data_set.size();

	data_calculation_set_mapping[key_ex] = value;
	return ret;
}

unsigned short ResultSet::add_calculation_set_weighted_avg5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 5);}
unsigned short ResultSet::add_calculation_set_weighted_avg10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 10);}
unsigned short ResultSet::add_calculation_set_weighted_avg20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 20);}
unsigned short ResultSet::add_calculation_set_weighted_avg60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex){return add_calculation_set_avg(data_array_base, key_ex, 60);}

unsigned short ResultSet::add_calculation_set(int source_type_index, int field_index, ArrayElementCalculationType calculation_type)
{
	typedef unsigned short (ResultSet::*add_calculation_set_func_ptr)(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	static add_calculation_set_func_ptr add_calculation_set_func_array[] =
	{
		&ResultSet::add_calculation_set_dummy,
		&ResultSet::add_calculation_set_diff,
		&ResultSet::add_calculation_set_rate,
		&ResultSet::add_calculation_set_sum5,
		&ResultSet::add_calculation_set_sum10,
		&ResultSet::add_calculation_set_sum20,
		&ResultSet::add_calculation_set_sum60,
		&ResultSet::add_calculation_set_avg5,
		&ResultSet::add_calculation_set_avg10,
		&ResultSet::add_calculation_set_avg20,
		&ResultSet::add_calculation_set_avg60,
		&ResultSet::add_calculation_set_weighted_avg5,
		&ResultSet::add_calculation_set_weighted_avg10,
		&ResultSet::add_calculation_set_weighted_avg20,
		&ResultSet::add_calculation_set_weighted_avg60
	};

	if (calculation_type == ArrayElementCalculation_None)
	{
		WRITE_ERROR("Incorrect calculation type: ArrayElementCalculation_None");
		return RET_FAILURE_INVALID_ARGUMENT;
//		return add_set(source_type_index, field_index);
	}
// Check the index boundary
	if(!check_source_type_index_in_range(source_type_index))
	{
		WRITE_ERROR("source_type_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(!check_field_index_in_range(source_type_index, field_index))
	{
		WRITE_ERROR("field_index is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if(!check_calculation_type_in_range(calculation_type))
	{
		WRITE_ERROR("calculation_type is out of range in ResultSet");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
// Make sure the array should NOT exist
	unsigned long key_ex = get_combined_index_ex(source_type_index, field_index, calculation_type);
	if (data_calculation_set_mapping.find(key_ex) != data_calculation_set_mapping.end())
	{
		WRITE_FORMAT_ERROR("The key[%ld] from (%d, %d, %d) is duplicate", key_ex, source_type_index, field_index, calculation_type);
		return RET_FAILURE_INVALID_ARGUMENT;
	}

// Find the original array
	PFINANCE_DATA_ARRAY_BASE data_array_base = get_array(source_type_index, field_index);
	if (data_array_base == NULL)
	{
		WRITE_FORMAT_ERROR("The array from (%d, %d) should EXIST", source_type_index, field_index);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	return (this->*(add_calculation_set_func_array[calculation_type]))(data_array_base, key_ex);
}

unsigned short ResultSet::set_date(const char* element_value)
{
	if (check_date_data_mode)
	{
		if (strcmp((const char*)date_data[date_data_cur_pos], element_value) != 0)
		{
			WRITE_FORMAT_ERROR("The date(%s, %s) is NOT equal", date_data[date_data_cur_pos], element_value);
			return RET_FAILURE_INCORRECT_OPERATION;
		}
		date_data_cur_pos++;
	}
	else
	{
		date_data.add(element_value, strlen(element_value) + 1);
		date_data_size++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::find_data_pos(int source_type_index, int field_index, unsigned short& field_type_index, unsigned short& field_type_pos)const
{
	unsigned short key = get_combined_index(source_type_index, field_index);
//	if (data_set_mapping.find(key) == data_set_mapping.end())
//	{
//		WRITE_FORMAT_ERROR("The key[%d] from (%d, %d) is NOT FOUND", key, source_type_index, field_index);
//		return RET_FAILURE_INVALID_ARGUMENT;
//	}
//	unsigned short value = data_set_mapping[key];
	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.find(key);
	if (iter == data_set_mapping.end())
	{
		char errmsg[64];
		snprintf(errmsg, 64, "The key[%d] from (%d, %d) is NOT FOUND", key, source_type_index, field_index);
		WRITE_ERROR(errmsg);
//		throw invalid_argument(errmsg);
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short value = iter->second;

	field_type_index = get_upper_subindex(value);
	field_type_pos = get_lower_subindex(value);

	return RET_SUCCESS;
}

unsigned short ResultSet::set_data(int source_type_index, int field_index, const char* data_string)
{
	if (data_string == NULL)
	{
		WRITE_ERROR("data_string should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short ret = RET_SUCCESS;
	unsigned short field_type_index, field_type_pos;
	ret = find_data_pos(source_type_index, field_index, field_type_index, field_type_pos);
	if (CHECK_FAILURE(ret))
		return ret;

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
		ret = RET_FAILURE_INVALID_ARGUMENT;
		break;
	}
	return RET_SUCCESS;
}

#define DEFINE_GET_ARRAY_FUNC_BY_POS(n, m)\
const PFINANCE_##m##_DATA_ARRAY ResultSet::get_##n##_array_by_position(unsigned short field_type_index, unsigned short field_type_pos)const\
{\
	static const int BUF_SIZE = 64;\
	static char errmsg[BUF_SIZE];\
	if (field_type_index != FinanceField_##m)\
	{\
		snprintf(errmsg, BUF_SIZE, "The field type[%d] is NOT int", field_type_index);\
		WRITE_ERROR(errmsg);\
		throw invalid_argument(errmsg);\
	}\
	return get_##n##_data_array(field_type_pos);\
}

DEFINE_GET_ARRAY_FUNC_BY_POS(int, INT)
DEFINE_GET_ARRAY_FUNC_BY_POS(long, LONG)
DEFINE_GET_ARRAY_FUNC_BY_POS(float, FLOAT)

#define DEFINE_GET_ARRAY_FUNC(n, m)\
const PFINANCE_##m##_DATA_ARRAY ResultSet::get_##n##_array(int source_type_index, int field_index)const\
{\
	static const int BUF_SIZE = 64;\
	static char errmsg[BUF_SIZE];\
	unsigned short ret = RET_SUCCESS;\
	unsigned short field_type_index, field_type_pos;\
	ret = find_data_pos(source_type_index, field_index, field_type_index, field_type_pos);\
	if (CHECK_FAILURE(ret))\
	{\
		snprintf(errmsg, BUF_SIZE, "Fail to fail data position from (%d, %d)", source_type_index, field_index);\
		WRITE_ERROR(errmsg);\
		throw invalid_argument(errmsg);\
	}\
	return get_##n##_array_by_position(field_type_index, field_type_pos);\
}

DEFINE_GET_ARRAY_FUNC(int, INT)
DEFINE_GET_ARRAY_FUNC(long, LONG)
DEFINE_GET_ARRAY_FUNC(float, FLOAT)

const PFINANCE_DATA_ARRAY_BASE ResultSet::get_array(int source_type_index, int field_index)const
{
	int field_type = FINANCE_DATABASE_FIELD_TYPE_LIST[source_type_index][field_index];
	switch (field_type)
	{
	case FinanceField_INT:
		return get_int_array(source_type_index, field_index);
	case FinanceField_LONG:
		return get_long_array(source_type_index, field_index);
	case FinanceField_FLOAT:
		return get_float_array(source_type_index, field_index);
	default:
	{
		char errmsg[32];
		snprintf(errmsg, 32, "Unknown field type: %d", field_type);
		WRITE_ERROR(errmsg);
		throw invalid_argument(errmsg);
	}
	break;
	}
	return NULL	;
}

const PFINANCE_DATA_ARRAY_BASE ResultSet::get_array(int source_type_index, int field_index, ArrayElementCalculationType calculation_type)
{
	static const int BUF_SIZE = 32;
	static char errmsg[BUF_SIZE];

	if (calculation_type == ArrayElementCalculation_None)
		return get_array(source_type_index, field_index);

	unsigned short ret = RET_SUCCESS;
	unsigned long key_ex = get_combined_index_ex(source_type_index, field_index, calculation_type);
// If the array should NOT exist, create the array
	if (data_calculation_set_mapping.find(key_ex) == data_calculation_set_mapping.end())
	{
		ret = add_calculation_set(source_type_index, field_index, calculation_type);
		if (CHECK_FAILURE(ret))
		{
			snprintf(errmsg, BUF_SIZE, "Fail to set array from (%d, %d, %d), due to: %s", source_type_index, field_index, calculation_type, get_ret_description(ret));
			WRITE_ERROR(errmsg);
			throw runtime_error(string(errmsg));
		}
	}
// Access the calculation array
	unsigned short value = data_calculation_set_mapping[key_ex];
	unsigned short field_type_index = get_upper_subindex(value);
	unsigned short field_type_pos = get_lower_subindex(value);
	switch (field_type_index)
	{
	case FinanceField_INT:
		return int_data_set[field_type_pos];
	case FinanceField_LONG:
		return long_data_set[field_type_pos];
	case FinanceField_FLOAT:
		return float_data_set[field_type_pos];
	default:
	{
		snprintf(errmsg, BUF_SIZE, "Unknown field type: %d", field_index);
		WRITE_ERROR(errmsg);
		throw invalid_argument(errmsg);
	}
	break;
	}
	return NULL	;
}

const PFINANCE_CHAR_DATA_PTR_ARRAY ResultSet::get_date_array()const
{
	return (const PFINANCE_CHAR_DATA_PTR_ARRAY)&date_data;
}

#define DEFINE_GET_ARRAY_ELEMENT_FUNC(n, m)\
n ResultSet::get_##n##_array_element(int source_type_index, int field_index, int index)const\
{\
	PFINANCE_##m##_DATA_ARRAY data_array = get_##n##_array(source_type_index, field_index);\
	return (*data_array)[index];\
}
DEFINE_GET_ARRAY_ELEMENT_FUNC(int, INT)
DEFINE_GET_ARRAY_ELEMENT_FUNC(long, LONG)
DEFINE_GET_ARRAY_ELEMENT_FUNC(float, FLOAT)

const char* ResultSet::get_date_array_element(int index)const
{
	return date_data[index];
}

int ResultSet::get_date_array_element_index(const char* date_str)const
{
	assert(date_str != NULL && "date_str should NOT be NULL");
	int cur_value = TimeCfg::get_int_value(date_str);
	int start_index = 0;
	int end_index = get_data_size() - 1;
	while (start_index <= end_index)
	{
		int mid_index = (end_index + start_index) / 2;
		int mid_value = TimeCfg::get_int_value(date_data[mid_index]);
		WRITE_FORMAT_DEBUG("***Index*** Start: %d, Mid: %d, End: %d", start_index, mid_index, end_index);
		WRITE_FORMAT_DEBUG("***Date*** Start: %s, Mid: %s, End: %s", date_data[start_index], date_data[mid_index], date_data[end_index]);
		if (cur_value == mid_value)
		{
			WRITE_FORMAT_DEBUG("Found in Index: %d", mid_index);
			return mid_index;
		}
		else if (cur_value > mid_value)
			start_index = mid_index + 1;
		else
			end_index = mid_index - 1;
	}
	WRITE_FORMAT_ERROR("Fail to find the date[%s] index", date_str);
	return -1;
}

void ResultSet::switch_to_check_date_mode()
{
	check_date_data_mode = true;
	date_data_cur_pos = 0;
}

unsigned short ResultSet::check_data()const
{
	assert(date_data_size == date_data.get_size() && "The date_data_size is NOT identical");
//	int date_data_size_tmp = date_data.get_size();
	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.begin();

	unsigned short key;
	unsigned short value;
	unsigned short field_type_index;
	unsigned short field_type_pos;
	int data_size;
	while (iter != data_set_mapping.end())
	{
		value = iter->second;
		field_type_index = get_upper_subindex(value);
		field_type_pos = get_lower_subindex(value);
		switch(field_type_index)
		{
		case FinanceField_INT:
			data_size = int_data_set[field_type_pos]->get_size();
			break;
		case FinanceField_LONG:
			data_size = long_data_set[field_type_pos]->get_size();
			break;
		case FinanceField_FLOAT:
			data_size = float_data_set[field_type_pos]->get_size();
			break;
		default:
			WRITE_FORMAT_ERROR("Unsupported field_type_index: %d", field_type_index);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
// Check all the data size are equal
		if (date_data_size != data_size)
		{
			key = iter->first;
			unsigned short source_type_index = get_upper_subindex(key);
			unsigned short field_index = get_lower_subindex(key);
			WRITE_FORMAT_ERROR("Incorrect data size in %d, %d, expected: %d, actual: %d", source_type_index, field_index, date_data_size, data_size);
			return RET_FAILURE_INCORRECT_OPERATION;
		}
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short ResultSet::show_data()const
{
	static int STAR_LEN = 120;
	if (!SHOW_CONSOLE)
	{
		WRITE_WARN("Disabled; No data will be shown on STDOUT/STDERR");
		return RET_SUCCESS;
	}
	unsigned short key;
	unsigned short value;
	unsigned short source_type_index;
	unsigned short field_index;
	unsigned short field_type_index;
	unsigned short field_type_pos;

	assert(date_data_size == date_data.get_size() && "The date_data_size is NOT identical");

// Show the database:field info
	for(int i = 0 ; i < STAR_LEN ; i++)
		putchar('*');
	printf("\n| %s |", MYSQL_DATE_FILED_NAME);

	map<unsigned short, unsigned short>::const_iterator iter = data_set_mapping.begin();
	while (iter != data_set_mapping.end())
	{
		key = iter->first;
		source_type_index = get_upper_subindex(key);
		field_index = get_lower_subindex(key);
		printf(" %s:%s%d |", FINANCE_DATABASE_DESCRIPTION_LIST[source_type_index], MYSQL_FILED_NAME_BASE, field_index);
		iter++;
	}
	printf("\n");
	for(int i = 0 ; i < STAR_LEN ; i++)
		putchar('*');
	printf("\n\n");

// Show the data info
//	int date_data_size_tmp = date_data.get_size();
	for(int i = 0 ; i < date_data_size ; i++)
	{
		printf("| %s |", date_data[i]);
		iter = data_set_mapping.begin();
		while (iter != data_set_mapping.end())
		{
			value = iter->second;
			field_type_index = get_upper_subindex(value);
			field_type_pos = get_lower_subindex(value);
			switch(field_type_index)
			{
			case FinanceField_INT:
				printf(" %10d |", (*int_data_set[field_type_pos])[i]);
				break;
			case FinanceField_LONG:
				printf(" %12ld |", (*long_data_set[field_type_pos])[i]);
				break;
			case FinanceField_FLOAT:
				printf(" %8.1f |", (*float_data_set[field_type_pos])[i]);
				break;
			default:
				WRITE_FORMAT_ERROR("Unsupported field_type_index: %d", field_type_index);
				return RET_FAILURE_INVALID_ARGUMENT;
			}
			iter++;
		}
		printf("\n");
	}
	return RET_SUCCESS;
}

int ResultSet::get_data_dimension()const{return data_set_mapping_size;}
int ResultSet::get_data_size()const{return date_data_size;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResultSetMap::ResultSetMap(ResultSetDataUnit data_unit) : result_set_data_unit(data_unit)
{

}

ResultSetMap::~ResultSetMap()
{
	RESULT_SET_MAP::iterator iter = result_set_map.begin();
	while (iter != result_set_map.end())
	{
		PRESULT_SET result_set = (PRESULT_SET)iter->second;
		assert(result_set != NULL && "result_set should NOT be NULL");
		iter++;
		delete result_set;
	}
}

ResultSetDataUnit ResultSetMap::get_data_unit()const
{
	return result_set_data_unit;
}

unsigned short ResultSetMap::register_result_set(int source_key, const PRESULT_SET result_set)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	RESULT_SET_MAP::const_iterator iter = result_set_map.find(source_key);
	if (iter != result_set_map.end()) 
	{
		static const int ERRMSG_SIZE = 256;
		static char errmsg[ERRMSG_SIZE];
		snprintf(errmsg, ERRMSG_SIZE, "The result set of source key[%d] already exist", source_key);
		throw invalid_argument(string(errmsg));
	}
	result_set_map[source_key] = result_set;
	return RET_SUCCESS;
}

const PRESULT_SET ResultSetMap::lookup_result_set(int source_key)const
{
	RESULT_SET_MAP::const_iterator iter = result_set_map.find(source_key);
	if (iter == result_set_map.end()) 
	{
		static const int ERRMSG_SIZE = 256;
		static char errmsg[ERRMSG_SIZE];
		snprintf(errmsg, ERRMSG_SIZE, "Fail to find the result set of source key: %d", source_key);
		throw invalid_argument(string(errmsg));
	}

	return ((PRESULT_SET)iter->second);
}