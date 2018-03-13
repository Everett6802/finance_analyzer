#ifndef FS_SOURCE_H
#define FS_SOURCE_H

#include <string>
#include "../common.h"
#include "source_param.h"


///////////////////////////////////////////////////////////////////////////

class FsSource;

class FsSourceParam : public SourceBaseParam
{
protected:
	unsigned short set_param(IFsSource* source_obj);

public:
	char* root_folderpath;

	// FsSourceParam();
	// // FsSourceParam(bool new_continue_when_non_exist, const char* new_root_folderpath);
	virtual ~FsSourceParam();
};
typedef FsSourceParam* PFS_SOURCE_PARAM;

///////////////////////////////////////////////////////////////////////////

class FsSourceHelper
{
private:
	static unsigned short format_market_filepath(char* csv_filepath_buf, int csv_filepath_buf_size, const char* root_folderpath, int method_index);
	static unsigned short format_stock_filepath(char* csv_filepath_buf, int csv_filepath_buf_size, const char* root_folderpath, int method_index, int company_group_number, const char* company_code_number);

	static unsigned short read_from_filesystem(
		const PTIME_RANGE_PARAM time_range_param, 
		const PQUERY_SET query_set,
		const std::string& company_code_number,  // For stock mode only, ignored in market mode
		const int company_group_number,  // For stock mode only, ignored in market mode
		FsSource* fs_source_obj, 
		FinanceAnalysisMode finance_analysis_mode,
		PRESULT_SET result_set
	);
	static unsigned short read_market(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, FsSource* fs_source_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_stock(const PQUERY_SET query_set, const PTIME_RANGE_PARAM time_range_param, const PCOMPANY_GROUP_SET company_group_set, FsSource* fs_source_obj, PRESULT_SET_MAP result_set_map);

public:
	static unsigned short read_by_object(const PSEARCH_RULE_SET search_rule_set, PISOURCE source_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_param(const PSEARCH_RULE_SET search_rule_set, PISOURCE_PARAM source_param, PRESULT_SET_MAP result_set_map);

private:
};
typedef FsSourceHelper* PFS_SOURCE_HELPER;

///////////////////////////////////////////////////////////////////////////

class FsSource : public IFsSource
{
	friend class FsSourceHelper;
protected:
	DECLARE_MSG_DUMPER()

	// FsSourceParam source_param;
	PFS_SOURCE_PARAM source_param;

	unsigned short read_data(
		int method_index,
		const char* csv_filepath,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_PARAM time_range_param,
		PRESULT_SET result_set
	);

	unsigned short write_data(
		int method_index,
		const char* csv_filepath,
		const PINT_DEQUE query_field,
		const PTIME_RANGE_PARAM time_range_param,
		const PRESULT_SET result_set
	);

protected:
	FsSource();
	FsSource(PFS_SOURCE_PARAM new_source_param);
	~FsSource();

public:
	unsigned short set_root_folderpath(const char* new_root_folderpath);
	unsigned short set_continue_when_non_exist(bool enable);
	// unsigned short set_time_range_param(const TimeRangeParam* new_time_range_param);
};
typedef FsSource* PFS_SOURCE;

#endif
