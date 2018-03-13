#include <assert.h>
#include "fs_source.h"
// #include "database_time_range.h"


using namespace std;

SourceBaseParam::SourceBaseParam() :
	// time_range_param(NULL),
	continue_when_non_exist(DEFAULT_CONTINUE_WHEN_NOT_EXIST)
{
	IMPLEMENT_MSG_DUMPER()
	// static int default_root_folderpath_len = strlen(DEFAULT_CSV_ROOT_FOLDERPATH);
	// root_folderpath = new char[default_root_folderpath_len + 1];
	// if (root_folderpath == NULL)
	// 	throw bad_alloc();
	// memset(root_folderpath, 0x0, sizeof(char) * (default_root_folderpath_len + 1));
	// memcpy(root_folderpath, DEFAULT_CSV_ROOT_FOLDERPATH, sizeof(char) * default_root_folderpath_len);
}

SourceBaseParam::~SourceBaseParam()
{
	// if (time_range_param != NULL)
	// {
	// 	delete time_range_param;
	// 	time_range_param = NULL;
	// }
	RELEASE_MSG_DUMPER()
}

// unsigned short SourceBaseParam::set_time_range_param(const TimeRangeParam* new_time_range_param)
// {
// 	if(new_time_range_param == NULL)
// 	{
// 		WRITE_ERROR("new_time_range_param should NOT be NULL");
// 		return RET_FAILURE_INVALID_ARGUMENT;
// 	}
// 	// assert(source_param != NULL && "source_param should NOT be NULL");
// 	if (time_range_param != NULL)
// 	{
// 		delete time_range_param;
// 		time_range_param = NULL;
// 	}
// 	time_range_param = new TimeRangeParam(new_time_range_param->get_start_time(), new_time_range_param->get_end_time());
// 	if (time_range_param == NULL)
// 		throw bad_alloc();
// 	unsigned short ret = time_range_param->add_time_done();
// 	return ret;
// }
