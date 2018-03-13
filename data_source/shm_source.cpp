#include <assert.h>
#include "shm_source.h"
// #include "database_time_range.h"


using namespace std;

ShmSourceParam::ShmSourceParam()
{
	static int default_shm_root_finance_folderpath_len = strlen(DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH);
	root_folderpath = new char[default_shm_root_finance_folderpath_len + 1];
	if (root_folderpath == NULL)
		throw bad_alloc();
	memset(root_folderpath, 0x0, sizeof(char) * (default_shm_root_finance_folderpath_len + 1));
	memcpy(root_folderpath, DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH, sizeof(char) * default_shm_root_finance_folderpath_len);
}

ShmSourceParam::ShmSourceParam(bool new_continue_when_non_exist, const char* new_root_folderpath)
{
	continue_when_non_exist = new_continue_when_non_exist;
	if (new_root_folderpath != NULL)
	{
		int root_folderpath_len = strlen(new_root_folderpath);
		root_folderpath = new char[root_folderpath_len + 1];
		if (root_folderpath == NULL)
			throw bad_alloc();
		strcpy(root_folderpath, new_root_folderpath);
	}
	else
	{
		static int default_shm_root_finance_folderpath_len = strlen(DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH);
		root_folderpath = new char[default_shm_root_finance_folderpath_len + 1];
		if (root_folderpath == NULL)
			throw bad_alloc();
		memset(root_folderpath, 0x0, sizeof(char) * (default_shm_root_finance_folderpath_len + 1));
		memcpy(root_folderpath, DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH, sizeof(char) * default_shm_root_finance_folderpath_len);
	}
}

ShmSourceParam::~ShmSourceParam()
{
}

FinanceDataType ShmSourceParam::get_type()const
{
	return FinanceData_SHM;
}

unsigned short ShmSourceParam::to_object(ISource** source_obj)
{
	assert(source_obj != NULL && "source_obj should NOT be NULL");
	PSHM_SOURCE shm_source_obj = new ShmSource();
	if (shm_source_obj == NULL)
		throw bad_alloc();
	unsigned short ret = set_param((IFsSource*)shm_source_obj);
	if (CHECK_FAILURE(ret))
		goto OUT;
	*source_obj = shm_source_obj;
	return RET_SUCCESS;
OUT:
	if (shm_source_obj != NULL)
	{
		delete shm_source_obj;
		shm_source_obj = NULL;
	}
	return ret;

}

// ///////////////////////////////////////////////////////////////////////////

unsigned short ShmSource::read_by_object(
	const PSEARCH_RULE_SET search_rule_set,
	PISOURCE source_obj, 
	PRESULT_SET_MAP result_set_map
	)
{
	return FsSourceHelper::read_by_object(search_rule_set, source_obj, result_set_map);
}

unsigned short ShmSource::read_by_param(
	const PSEARCH_RULE_SET search_rule_set,
	PISOURCE_PARAM source_param, 
	PRESULT_SET_MAP result_set_map
	)
{
	return FsSourceHelper::read_by_param(search_rule_set, source_param, result_set_map);
}

unsigned short ShmSource::read_by_default(
	const PSEARCH_RULE_SET search_rule_set, 
	PRESULT_SET_MAP result_set_map
	)
{
	ShmSource shm_source_obj;
	return FsSourceHelper::read_by_object(search_rule_set, &shm_source_obj, result_set_map);	
}

ShmSource::ShmSource() : FsSource(new ShmSourceParam())
{
// Implemented in the base class
	// IMPLEMENT_MSG_DUMPER()
	// source_param = new ShmSourceParam();
	// if (source_param == NULL)
	// 	throw bad_alloc();
}

ShmSource::~ShmSource()
{
// Implemented in the base class
	// if (source_param != NULL)
	// {
	// 	delete source_param;
	// 	source_param = NULL;
	// }
	// RELEASE_MSG_DUMPER()
}

FinanceDataType ShmSource::get_type()const
{
	return FinanceData_SHM;
}

unsigned short ShmSource::read_line_data(std::list<std::string>& line_list, const char* filepath, const PTIME_RANGE_PARAM time_range_param)
{
	return read_shm_file_lines_ex(line_list, filepath, "r", time_range_param);
}

unsigned short ShmSource::write_line_data(const std::list<std::string>& line_list, const char* filepath, const PTIME_RANGE_PARAM time_range_param)
{
	return write_shm_file_lines_ex(line_list, filepath, "w", time_range_param);
}
