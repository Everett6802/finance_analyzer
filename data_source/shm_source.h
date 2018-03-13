#ifndef SHM_SOURCE_H
#define SHM_SOURCE_H

#include "../common.h"
#include "fs_source.h"


class ShmSourceParam : public FsSourceParam
{
public:
	ShmSourceParam();
	ShmSourceParam(bool new_continue_when_non_exist, const char* new_root_folderpath);
	virtual ~ShmSourceParam();

	virtual FinanceDataType get_type()const;
	virtual unsigned short to_object(ISource** source_obj);
};
typedef ShmSourceParam* PSHM_SOURCE_PARAM;

///////////////////////////////////////////////////////////////////////////

class ShmSource : public FsSource
{
public:
	static unsigned short read_by_object(const PSEARCH_RULE_SET search_rule_set, PISOURCE source_obj, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_param(const PSEARCH_RULE_SET search_rule_set, PISOURCE_PARAM source_param, PRESULT_SET_MAP result_set_map);
	static unsigned short read_by_default(const PSEARCH_RULE_SET search_rule_set, PRESULT_SET_MAP result_set_map);

private:
	virtual unsigned short read_line_data(std::list<std::string>& line_list, const char* filepath, const PTIME_RANGE_PARAM time_range_param);
	virtual unsigned short write_line_data(const std::list<std::string>& line_list, const char* filepath, const PTIME_RANGE_PARAM time_range_param);

public:
	ShmSource();
	~ShmSource();

	virtual FinanceDataType get_type()const;
};
typedef ShmSource* PSHM_SOURCE;

#endif
