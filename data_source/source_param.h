#ifndef SOURCE_PARAM_H
#define SOURCE_PARAM_H

#include <string>
#include "../common.h"
#include "source_def.h"


///////////////////////////////////////////////////////////////////////////

class SourceBaseParam : public ISourceParam
{
protected:
	DECLARE_MSG_DUMPER()

public:
	bool continue_when_non_exist;
	// TimeRangeParam* time_range_param;

	SourceBaseParam();
	virtual ~SourceBaseParam();

	// unsigned short set_time_range_param(const TimeRangeParam* new_time_range_param);
};

#endif
