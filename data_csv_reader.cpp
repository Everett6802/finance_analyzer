#include <assert.h>
#include "data_csv_reader.h"
// #include "database_time_range.h"


using namespace std;

DataCsvReaderParam::DataCsvReaderParam() :
	root_folderpath(NULL),
	continue_when_non_exist(true)
{
	static int default_root_folderpath_len = strlen(DEFAULT_CSV_ROOT_FOLDERPATH);
	root_folderpath = new char[default_root_folderpath_len + 1];
	if (root_folderpath == NULL)
		throw bad_alloc();
	memset(root_folderpath, 0x0, sizeof(char) * (default_root_folderpath_len + 1));
	memcpy(root_folderpath, DEFAULT_CSV_ROOT_FOLDERPATH, sizeof(char) * default_root_folderpath_len);
}

DataCsvReaderParam::~DataCsvReaderParam()
{
	if (root_folderpath != NULL)
	{
		delete[] root_folderpath;
		root_folderpath = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////

unsigned short DataCsvReader::read_from_filesystem(
		const PTIME_RANGE_PARAM time_range_param, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		const int company_group_number,  // For stock mode only, ignored in market mode
		DataCsvReader* csv_reader_obj, 
		FinanceAnalysisMode finance_analysis_mode,
		PRESULT_SET result_set
	)
{
	assert((finance_analysis_mode == FinanceAnalysis_Market || finance_analysis_mode == FinanceAnalysis_Stock) && "finance_analysis_mode is NOT FinanceAnalysis_Market/FinanceAnalysis_Stock");
	assert(result_set != NULL && "result_set should NOT be NULL");
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	static int const CSV_FILEPATH_BUF_SIZE = 256;
	static char csv_filepath_buf[CSV_FILEPATH_BUF_SIZE];
	if (finance_analysis_mode == FinanceAnalysis_Stock)
	{
		DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE()
		if (!company_profile->is_company_exist(company_code_number))
		{
			WRITE_FORMAT_ERROR("The company[%s] does NOT exist", company_code_number.c_str());
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}
	unsigned short ret = RET_SUCCESS;
	for (QuerySet::const_iterator iter = query_set->begin() ; iter != query_set->end() ; ++iter)
	{
		// const INT_DEQUE& query_field = (*query_set)[source_index];
		const INT_DEQUE& query_field = (*iter);
		assert(!query_field.empty() && "query_field should NOT be empty");
		int method_index = iter.get_first();
// Determine the CSV file path
		if (finance_analysis_mode == FinanceAnalysis_Market)
		{
			snprintf(csv_filepath_buf, CSV_FILEPATH_BUF_SIZE, "%s/%s/%s.csv", csv_reader_obj->data_csv_reader_param.root_folderpath, FINANCE_DATA_MARKET_NAME, FINANCE_CSV_FILE_NAME_LIST[method_index]);
		}
		else
		{
			snprintf(csv_filepath_buf, CSV_FILEPATH_BUF_SIZE, "%s/%s%02d/%s/%s.csv", csv_reader_obj->data_csv_reader_param.root_folderpath, FINANCE_DATA_STOCK_NAME, company_group_number, company_code_number.c_str(), FINANCE_CSV_FILE_NAME_LIST[method_index]);
		}
// Add to the result set
		ret = result_set->add_set(method_index, query_field);
		if (CHECK_FAILURE(ret))
			return ret;
// Read the CSV data from the file system
		ret = csv_reader_obj->read_data(
			method_index,
			csv_filepath_buf,
			(const PINT_DEQUE)&query_field, 
			time_range_param, 
			result_set
		);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return RET_SUCCESS;
}

unsigned short DataCsvReader::read_market(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, DataCsvReader* csv_reader_obj, PRESULT_SET_MAP result_set_map)
{
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	// DECLARE_AND_IMPLEMENT_STATIC_DATABASE_TIME_RANGE()
	static string company_code_number_dummy("xxxx");
	static int company_group_number_dummy = 99;
	STATIC_WRITE_DEBUG("Start to read the CSV data in the Market mode......");
	assert(csv_reader_obj != NULL && query_set != NULL && time_range_param != NULL && result_set_map != NULL);

	if (!query_set->is_add_query_done())
	{
		WRITE_ERROR("The setting of query data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	if (time_range_param->get_start_time() == NULL || time_range_param->get_end_time() == NULL)
	{
		WRITE_ERROR("The start/end time in time_range_param should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	unsigned short ret = RET_SUCCESS;
// // Check the boundary of each database
// 	SmartPointer<TimeRangeParam> sp_time_range_param(new TimeRangeParam(*time_range_param));
// 	WRITE_FORMAT_DEBUG("The original search time range: %s", sp_time_range_param->to_string());
// 	ret = database_time_range->restrict_time_range(query_set, sp_time_range_param.get_instance());
// 	if (CHECK_FAILURE(ret))
// 		return ret;
// 	WRITE_FORMAT_DEBUG("The new search time range: %s", sp_time_range_param->to_string());

	ResultSetDataUnit result_set_data_unit = result_set_map->get_data_unit();
	switch (result_set_data_unit)
	{
		case ResultSetDataUnit_MultipleMethod:
		{
// Query data from MySQL
			PRESULT_SET result_set = new ResultSet(FinanceData_CSV);
			if (result_set == NULL)
			{
				WRITE_ERROR("Fail to allocate memory: result_set");
				return RET_FAILURE_INSUFFICIENT_MEMORY;
			}
// Query the data from each table
			ret = read_from_filesystem(
				// sp_time_range_param.get_instance(), 
				time_range_param,
				query_set,
				company_code_number_dummy,  // For stock mode only, ignored in market mode
				company_group_number_dummy,  // For stock mode only, ignored in market mode
				csv_reader_obj, 
				FinanceAnalysis_Market,
				result_set
			);
			if (CHECK_FAILURE(ret))
				return ret;
			ret = result_set_map->register_result_set(NO_METHOD_MARKET_SOURCE_KEY_VALUE, result_set);
			if (CHECK_FAILURE(ret))
				return ret;
// Check the result
			result_set->switch_to_check_date_mode();
			ret = result_set->check_data();
			if (CHECK_FAILURE(ret))
				return ret;
		}
		break;
		case ResultSetDataUnit_SingleMethod:
		{
			QuerySet::const_iterator iter = query_set->begin();			
			while (iter != query_set->end())
			{
				int method_index = iter.get_first();
				PQUERY_SET query_sub_set = NULL;
				// const PINT_DEQUE field_index_deque = iter.get_second();
// Initialize the sub query set
				ret = query_set->get_query_sub_set(method_index, &query_sub_set);
				if (CHECK_FAILURE(ret))
					return ret;
				SmartPointer<QuerySet> sp_query_sub_set;
				sp_query_sub_set.set_new(query_sub_set);
				// sp_query_sub_set->add_query_done();
// Query data from MySQL
				PRESULT_SET result_set = new ResultSet(FinanceData_CSV);
				if (result_set == NULL)
				{
					WRITE_ERROR("Fail to allocate memory: result_set");
					return RET_FAILURE_INSUFFICIENT_MEMORY;
				}
// Query the data from each table
				ret = read_from_filesystem(
					// sp_time_range_param.get_instance(), 
					time_range_param,
					sp_query_sub_set.get_instance(),
					company_code_number_dummy,  // For stock mode only, ignored in market mode
					company_group_number_dummy,  // For stock mode only, ignored in market mode
					csv_reader_obj,
					FinanceAnalysis_Market, 
					result_set
				);
				if (CHECK_FAILURE(ret))
					return ret;
				int source_key = get_source_key(method_index);
				ret = result_set_map->register_result_set(source_key, result_set);
				if (CHECK_FAILURE(ret))
					return ret;
				++iter;
			}
		}
		break;
		default:
		{
			WRITE_FORMAT_ERROR("Unknown result set data unit: %d", result_set_data_unit);
			return RET_FAILURE_INVALID_ARGUMENT;
		}
		break;
	}

	return RET_SUCCESS;
}

unsigned short DataCsvReader::read_stock(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, const PCOMPANY_GROUP_SET company_group_set, DataCsvReader* csv_reader_obj, PRESULT_SET_MAP result_set_map)
{
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	// DECLARE_AND_IMPLEMENT_STATIC_DATABASE_TIME_RANGE()
	STATIC_WRITE_DEBUG("Start to read the CSV data in the Stock mode......");
	assert(csv_reader_obj != NULL && query_set != NULL && time_range_param != NULL && company_group_set != NULL && result_set_map != NULL);
	if (!query_set->is_add_query_done())
	{
		WRITE_ERROR("The setting of query data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}
	if (time_range_param->get_start_time() == NULL || time_range_param->get_end_time() == NULL)
	{
		WRITE_ERROR("The start/end time in time_range_param should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if (!company_group_set->is_add_company_done())
	{
		WRITE_ERROR("The setting of company group data is NOT complete");
		return RET_FAILURE_INCORRECT_OPERATION;
	}

	unsigned short ret = RET_SUCCESS;
	bool init_query_sub_set_array = false; // For ResultSetDataUnit_SingleMethod only
	SmartPointer<QuerySet> sp_query_sub_set_array[STOCK_METHOD_INDEX_LENGTH]; // For ResultSetDataUnit_SingleMethod only

	ResultSetDataUnit result_set_data_unit = result_set_map->get_data_unit();
	// CompanyGroupSet& company_group_set = query_set->get_company_group_set();
	for(CompanyGroupSet::const_iterator iter = company_group_set->begin() ; iter != company_group_set->end() ; ++iter)
	{
		int company_group_number = iter.get_first();
		const STRING_DEQUE& company_code_number_deque = *iter;
		for(STRING_DEQUE_CONST_ITER iter = company_code_number_deque.begin() ; iter != company_code_number_deque.end() ; ++iter)
		{
			string company_code_number = (string)*iter;
// // Check the boundary of each database
// 			SmartPointer<TimeRangeParam> sp_time_range_param(new TimeRangeParam(*time_range_param));
// 			WRITE_FORMAT_DEBUG("The original search time range: %s", sp_time_range_param->to_string());
// 			ret = database_time_range->restrict_time_range(query_set, sp_time_range_param.get_instance());
// 			if (CHECK_FAILURE(ret))
// 				return ret;
// 			WRITE_FORMAT_DEBUG("The new search time range: %s", sp_time_range_param->to_string());
// Query the data from each table
			switch (result_set_data_unit)
			{
				case ResultSetDataUnit_MultipleMethod:
				{
					PRESULT_SET result_set = new ResultSet(FinanceData_CSV);
					if (result_set == NULL)
					{
						WRITE_FORMAT_ERROR("Fail to allocate memory: result_set while reading %s table", company_code_number.c_str());
						return RET_FAILURE_INSUFFICIENT_MEMORY;
					}
					ret = read_from_filesystem(
						// sp_time_range_param.get_instance(), 
						time_range_param,
						query_set,
						company_code_number,  // For stock mode only, ignored in market mode
						company_group_number,  // For stock mode only, ignored in market mode
						csv_reader_obj, 
						FinanceAnalysis_Stock,
						result_set
					);
					if (CHECK_FAILURE(ret))
						return ret;
// Check the result
					result_set->switch_to_check_date_mode();
					ret = result_set->check_data();
					if (CHECK_FAILURE(ret))
						return ret;
					int source_key = get_source_key(company_group_number, company_code_number);
// Add the result into group
				 	ret = result_set_map->register_result_set(source_key, result_set);
					if (CHECK_FAILURE(ret))
						return ret;
				}
				break;
				case ResultSetDataUnit_SingleMethod:
				{
// Initialize the sub query set for the first time
					if (!init_query_sub_set_array)
					{
						QuerySet::const_iterator iter = query_set->begin();
						while (iter != query_set->end())
						{
							int method_index = iter.get_first();
							int method_revised_index = method_index - FinanceMethod_StockStart;
							// const PINT_DEQUE field_index_deque = iter.get_second();
							PQUERY_SET query_sub_set = NULL;
							ret = query_set->get_query_sub_set(method_index, &query_sub_set);
							if (CHECK_FAILURE(ret))
								return ret;
							sp_query_sub_set_array[method_revised_index].set_new(query_sub_set);
							// sp_query_sub_set_array[method_revised_index]->add_query_done();
						}
						init_query_sub_set_array = true;
					}
					QuerySet::const_iterator iter = query_set->begin();
// Query data from MySQL
					while (iter != query_set->end())
					{
						int method_index = iter.get_first();
						int method_revised_index = method_index - FinanceMethod_StockStart;
						PRESULT_SET result_set = new ResultSet(FinanceData_CSV);
						if (result_set == NULL)
						{
							WRITE_ERROR("Fail to allocate memory: result_set");
							return RET_FAILURE_INSUFFICIENT_MEMORY;
						}
// Query the data from each table
						ret = read_from_filesystem(
							// sp_time_range_param.get_instance(), 
							time_range_param,
							sp_query_sub_set_array[method_revised_index].get_instance(),
							company_code_number,  // For stock mode only, ignored in market mode
							company_group_number,  // For stock mode only, ignored in market mode
							csv_reader_obj, 
							FinanceAnalysis_Stock,
							result_set
						);
						if (CHECK_FAILURE(ret))
							return ret;
						int source_key = get_source_key(method_index, company_code_number, company_group_number);
						ret = result_set_map->register_result_set(source_key, result_set);
						if (CHECK_FAILURE(ret))
							return ret;
						++iter;
					}
				}
				break;
				default:
				{
					WRITE_FORMAT_ERROR("Unknown result set data unit: %d", result_set_data_unit);
					return RET_FAILURE_INVALID_ARGUMENT;
				}
				break;
			}
		}
	}

	return RET_SUCCESS;
}

unsigned short DataCsvReader::read_by_object(
	const PSEARCH_RULE_SET search_rule_set,
	void* reader_obj, 
	PRESULT_SET_MAP result_set_map
	)
{
	assert(reader_obj != NULL && search_rule_set != NULL && result_set_map != NULL && search_rule_set->get_query_rule() != NULL);
	STATIC_WRITE_DEBUG("Start to read the CSV data......");
	unsigned short ret = RET_SUCCESS;
	if (search_rule_set->get_query_rule()->get_data_type() != FinanceData_CSV)
	{
		STATIC_WRITE_FORMAT_ERROR("The data type of the QuerySet object should be %s, not %s", FINANCE_DATA_DESCRIPTION[FinanceData_CSV], FINANCE_DATA_DESCRIPTION[search_rule_set->get_query_rule()->get_data_type()]);		
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	if (result_set_map->get_data_type() != FinanceData_CSV)
	{
		STATIC_WRITE_FORMAT_ERROR("The data type of the ResultSetMap object should be %s, not %s", FINANCE_DATA_DESCRIPTION[FinanceData_CSV], FINANCE_DATA_DESCRIPTION[result_set_map->get_data_type()]);		
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	PDATA_CSV_READER csv_reader_obj = (PDATA_CSV_READER)reader_obj;
	if (search_rule_set->get_finance_mode() == FinanceAnalysis_Market)
		ret = read_market(search_rule_set->get_query_rule(), search_rule_set->get_time_rule(), csv_reader_obj, result_set_map);
	else if (search_rule_set->get_finance_mode() == FinanceAnalysis_Stock)
		ret = read_stock(search_rule_set->get_query_rule(), search_rule_set->get_time_rule(), search_rule_set->get_company_rule(), csv_reader_obj, result_set_map);
	else
	{
		ret = RET_FAILURE_INVALID_ARGUMENT;
		STATIC_WRITE_FORMAT_ERROR("query() fails, due to: %s", get_ret_description(ret));
	}
	return ret;
}

unsigned short DataCsvReader::read_by_param(
	const PSEARCH_RULE_SET search_rule_set,
	void* reader_param, 
	PRESULT_SET_MAP result_set_map
	)
{
	assert(reader_param != NULL && "reader_param should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	PDATA_CSV_READER_PARAM data_csv_reader_param = (PDATA_CSV_READER_PARAM)reader_param;
	DataCsvReader csv_reader_obj;
// Set parameters......
	if (data_csv_reader_param->root_folderpath != NULL)
	{
		ret = csv_reader_obj.set_root_folderpath(data_csv_reader_param->root_folderpath);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	ret = csv_reader_obj.set_continue_when_non_exist(data_csv_reader_param->continue_when_non_exist);
	if (CHECK_FAILURE(ret))
		return ret;
	return read_by_object(search_rule_set, &csv_reader_obj, result_set_map);
}

unsigned short DataCsvReader::read_by_default(
	const PSEARCH_RULE_SET search_rule_set, 
	PRESULT_SET_MAP result_set_map
	)
{
	DataCsvReader csv_reader_obj;
	return read_by_object(search_rule_set, &csv_reader_obj, result_set_map);	
}

DataCsvReader::DataCsvReader()
{
	IMPLEMENT_MSG_DUMPER()
	// int default_csv_root_folderpath_len = strlen(DEFAULT_CSV_ROOT_FOLDERPATH);
	// data_csv_reader_param.root_folderpath = new char[default_csv_root_folderpath_len + 1];
	// if (data_csv_reader_param.root_folderpath == NULL)
	// 	throw bad_alloc();
	// memset(data_csv_reader_param.root_folderpath, 0x0, sizeof(char) * default_csv_root_folderpath_len);
	// memcpy(data_csv_reader_param.root_folderpath, DEFAULT_CSV_ROOT_FOLDERPATH, default_csv_root_folderpath_len);
}

DataCsvReader::~DataCsvReader()
{
	RELEASE_MSG_DUMPER()
}

unsigned short DataCsvReader::read_data(
		int method_index,
		const char* csv_filepath,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_PARAM time_range_param,
		PRESULT_SET result_set
	)
{
	assert(query_field != NULL && !query_field->empty() && "query_field should NOT be NULL/Empty");
	assert(result_set != NULL && "result_set should NOT be NULL");
	static const char* DELIM = ",";
	static const int LINE_BUF_SIZE = 1024;
	static char line_buf[LINE_BUF_SIZE];
	static const int LINE_MAX_ELEMENT_COUT = 256;
	static char *line_element[LINE_MAX_ELEMENT_COUT];
	unsigned short ret = RET_SUCCESS;
// Read the data from CSV
	list<string> line_list;
	ret = read_file_lines_ex(line_list, csv_filepath, "r", time_range_param);
	if (CHECK_FAILURE(ret))
		return ret;
// Count the amount of the field including the date field
	int data_dimension = (int)query_field->size() + 1;
// Read the data in each row
	list<string>::iterator iter = line_list.begin();
	while (iter != line_list.end())
	{
// Parse each row
		string line = (string)*iter;
		int line_len = strlen(line.c_str());
		if (line_len >= LINE_BUF_SIZE)
		{
			WRITE_FORMAT_ERROR("The line length is over the threshold: %d", LINE_BUF_SIZE);
			return RET_FAILURE_INCORRECT_CONFIG;			
		}
		memset(line_buf, 0x0, sizeof(char) * LINE_BUF_SIZE);
		memcpy(line_buf, line.c_str(), sizeof(char) * line_len);
// Parse each element in the row and put into an array
		int line_element_count = 0;
		line_element[line_element_count++] = strtok(line_buf, DELIM);
		if (line_element[0] == NULL)
		{
			WRITE_FORMAT_ERROR("Incorrect format in csv file[%s]: %s", csv_filepath, line_buf);
			return RET_FAILURE_INCORRECT_CONFIG;
		}
		// printf("%s ", line_element[0]);
		while((line_element[line_element_count++] = strtok(NULL, DELIM)) != NULL)
		{
			if (line_element_count >= LINE_MAX_ELEMENT_COUT)
			{
				WRITE_FORMAT_ERROR("The line element count is over the threshold: %d", LINE_MAX_ELEMENT_COUT);
				return RET_FAILURE_INCORRECT_CONFIG;			
			}
			// printf("%s ", line_element[line_element_count - 1]);
		}
		// printf("\n");
// Keep track of the interested field in the line element array 
// Set the date
		ret = result_set->set_date(line_element[0]);
		if (CHECK_FAILURE(ret))
			return ret;
// Set the data in each field
		for(int index = 1 ; index < data_dimension ; index++)
		{
			int field_index = (*query_field)[index - 1];
			if (field_index < 0 || field_index >= line_element_count)
			{
				WRITE_FORMAT_ERROR("The field_index is out of range: [0, %d)", line_element_count);
				return RET_FAILURE_INCORRECT_CONFIG;			
			}
			ret = result_set->set_data(method_index, field_index, line_element[field_index]);
			if (CHECK_FAILURE(ret))
				return ret;
		}
		iter++;
	}
	return RET_SUCCESS;
}

unsigned short DataCsvReader::set_root_folderpath(const char* new_root_folderpath)
{
	if(new_root_folderpath == NULL)
	{
		WRITE_ERROR("new_root_folderpath should NOT be NULL");
		return RET_FAILURE_INVALID_ARGUMENT;
	}
	// assert(data_csv_reader_param != NULL && "data_csv_reader_param should NOT be NULL");
	assert(data_csv_reader_param.root_folderpath != NULL && "data_csv_reader_param.root_folderpath should NOT be NULL");
	delete[] data_csv_reader_param.root_folderpath;
	int new_root_folderpath_len = strlen(new_root_folderpath);
	data_csv_reader_param.root_folderpath = new char[new_root_folderpath_len + 1];
	if (data_csv_reader_param.root_folderpath == NULL)
		throw bad_alloc();
	// snprintf(data_csv_reader_param.root_folderpath, new_root_folderpath_len, "%s", new_root_folderpath);
	memcpy(data_csv_reader_param.root_folderpath, new_root_folderpath, sizeof(char) * (new_root_folderpath_len + 1));
	return RET_SUCCESS;
}

unsigned short DataCsvReader::set_continue_when_non_exist(bool enable)
{
	// assert(data_csv_reader_param != NULL && "data_csv_reader_param should NOT be NULL");
	data_csv_reader_param.continue_when_non_exist = enable;
	return RET_SUCCESS;
}