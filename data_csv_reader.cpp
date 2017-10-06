#include <assert.h>
#include "data_csv_reader.h"
#include "database_time_range.h"


using namespace std;

DataCsvReaderParam::DataCsvReaderParam() : 
	root_folderpath(NULL),
	continue_when_non_exist(true)
{

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
		const PTIME_RANGE_CFG restricted_time_range_cfg, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		DataCsvReader* csv_reader_obj, 
		FinanceAnalysisMode finance_analysis_mode,
		PRESULT_SET result_set
	)
{
	assert(!(finance_analysis_mode == FinanceAnalysis_Market || finance_analysis_mode == FinanceAnalysis_Stock) && "finance_analysis_mode is NOT FinanceAnalysis_Market/FinanceAnalysis_Stock");
	assert(result_set != NULL && "result_set should NOT be NULL");
	DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER()
	DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE()
	if (finance_analysis_mode == FinanceAnalysis_Stock)
	{
		if (!company_profile->is_company_exist(company_code_number))
		{
			WRITE_FORMAT_ERROR("The company[%s] does NOT exist", company_code_number.c_str());
			return RET_FAILURE_INVALID_ARGUMENT;
		}
	}
	unsigned short ret = RET_SUCCESS;
	string table_name;
	for (QuerySet::const_iterator iter = query_set->begin() ; iter != query_set->end() ; ++iter)
	{
		// const INT_DEQUE& query_field = (*query_set)[source_index];
		const INT_DEQUE& query_field = (*iter);
		assert(!query_field.empty() && "query_field should NOT be empty");
		int method_index = iter.get_first();
// Add to the result set
		ret = result_set->add_set(method_index, query_field);
		if (CHECK_FAILURE(ret))
			return ret;
// // Generate the field command
// 		string field_cmd = string("");
// 		FinanceAnalyzerSqlReader::get_sql_field_command(method_index, query_field, field_cmd);
// // Search for each table
// 		table_name = string(FINANCE_TABLE_NAME_LIST[method_index]);
// 		if (finance_analysis_mode == FinanceAnalysis_Stock)
// 			table_name = company_code_number + table_name;
// 		ret = csv_reader_obj->select_data(
// 			method_index, 
// 			table_name, 
// 			field_cmd, 
// 			(const PINT_DEQUE)&query_field, 
// 			restricted_time_range_cfg, 
// 			result_set
// 		);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return RET_SUCCESS;
}

unsigned short DataCsvReader::read_market(const PQUERY_SET query_set, const PTIME_RANGE_CFG time_range_cfg, DataCsvReader* csv_reader_obj, PRESULT_SET_MAP result_set_map)
{
	return RET_SUCCESS;
}

unsigned short DataCsvReader::read_stock(const PQUERY_SET query_set, const PTIME_RANGE_CFG time_range_cfg, const PCOMPANY_GROUP_SET company_group_set, DataCsvReader* csv_reader_obj, PRESULT_SET_MAP result_set_map)
{
	return RET_SUCCESS;
}

unsigned short DataCsvReader::read_by_object(
	const PSEARCH_RULE_SET search_rule_set,
	void* reader_obj, 
	PRESULT_SET_MAP result_set_map
	)
{
	assert(reader_obj != NULL && search_rule_set != NULL && result_set_map != NULL);
	unsigned short ret = RET_SUCCESS;
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
	int default_csv_root_folderpath_len = strlen(DEFAULT_CSV_ROOT_FOLDERPATH);
	data_csv_reader_param.root_folderpath = new char[default_csv_root_folderpath_len + 1];
	if (data_csv_reader_param.root_folderpath == NULL)
		throw bad_alloc();
	memset(data_csv_reader_param.root_folderpath, 0x0, sizeof(char) * default_csv_root_folderpath_len);
	memcpy(data_csv_reader_param.root_folderpath, DEFAULT_CSV_ROOT_FOLDERPATH, default_csv_root_folderpath_len);
}

DataCsvReader::~DataCsvReader()
{
	RELEASE_MSG_DUMPER()
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
	snprintf(data_csv_reader_param.root_folderpath, new_root_folderpath_len, "%s", new_root_folderpath);
	return RET_SUCCESS;
}

unsigned short DataCsvReader::set_continue_when_non_exist(bool enable)
{
	// assert(data_csv_reader_param != NULL && "data_csv_reader_param should NOT be NULL");
	data_csv_reader_param.continue_when_non_exist = enable;
	return RET_SUCCESS;
}