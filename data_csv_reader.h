#ifndef DATA_CSV_READER_H
#define DATA_CSV_READER_H

#include "common.h"


class DataCsvReaderParam
{
public:
	char* root_folderpath;
	bool continue_when_non_exist;

	DataCsvReaderParam();
	~DataCsvReaderParam();
};
typedef DataCsvReaderParam* PDATA_CSV_READER_PARAM;

///////////////////////////////////////////////////////////////////////////

class DataCsvReader
{
	DECLARE_MSG_DUMPER()
	static unsigned short read_from_filesystem(
		const PTIME_RANGE_PARAM time_range_param, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		const int company_group_number,  // For stock mode only, ignored in market mode
		DataCsvReader* csv_reader_obj, 
		FinanceAnalysisMode finance_analysis_mode,
		PRESULT_SET result_set
	);
	static unsigned short read_market(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, DataCsvReader* csv_reader_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_stock(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, const PCOMPANY_GROUP_SET company_group_set, DataCsvReader* csv_reader_obj, PRESULT_SET_MAP result_set_map);

public:
	static unsigned short read_by_object(const PSEARCH_RULE_SET search_rule_set, void* reader_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_param(const PSEARCH_RULE_SET search_rule_set, void* reader_param, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_default(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

private:
	DataCsvReaderParam data_csv_reader_param;

	DataCsvReader();
	~DataCsvReader();

	unsigned short read_data(
		int method_index,
		const char* csv_filepath,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_PARAM time_range_param,
		PRESULT_SET result_set
	);

	unsigned short set_root_folderpath(const char* new_root_folderpath);
	unsigned short set_continue_when_non_exist(bool enable);
};
typedef DataCsvReader* PDATA_CSV_READER;

#endif
