#ifndef SOURCE_DEF_H
#define SOURCE_DEF_H

#include <string>
#include "../common.h"


///////////////////////////////////////////////////////////////////////////

class ISource;

class ISourceParam
{
public:
	virtual ~ISourceParam(){}

	virtual FinanceDataType get_type()const=0;
	virtual unsigned short to_object(ISource** source_obj)=0;
};
typedef ISourceParam* PISOURCE_PARAM;

///////////////////////////////////////////////////////////////////////////

class ISource
{
public:
	virtual ~ISource(){}

	virtual FinanceDataType get_type()const=0;

	unsigned short set_continue_when_non_exist(bool enable);
	// unsigned short set_time_range_param(const TimeRangeParam* new_time_range_param);
};
typedef ISource* PISOURCE;

///////////////////////////////////////////////////////////////////////////

class IFsSource : public ISource
{
protected:
	virtual unsigned short read_line_data(std::list<std::string>& line_list, const char* filepath, const PTIME_RANGE_PARAM time_range_param)=0;
	virtual unsigned short write_line_data(const std::list<std::string>& line_list, const char* filepath, const PTIME_RANGE_PARAM time_range_param)=0;

public:
	unsigned short set_root_folderpath(const char* new_root_folderpath);
};
typedef IFsSource* PIFS_SOURCE;

#endif
