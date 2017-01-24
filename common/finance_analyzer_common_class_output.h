#ifndef FINANCE_ANALYZER_COMMON_CLASS_OUTPUT_H
#define FINANCE_ANALYZER_COMMON_CLASS_OUTPUT_H

#include <string>
#include <list>
#include "msg_dumper_wrapper.h"
#include "finance_analyzer_common_definition.h"
#include "finance_analyzer_common_class_base.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class

class IOutputStream
{
public:
	virtual ~IOutputStream(){}; // Caution: Required !!!
	virtual unsigned short output(const char* output_data)const=0;
};
typedef IOutputStream* POUTPUT_STREAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class OutputStreamBase : public IOutputStream
{
protected:
	DECLARE_MSG_DUMPER()
	POUTPUT_STREAM output_stream_obj_handle;

	virtual unsigned short output_internal(const char* output_data)const=0;

public:
	OutputStreamBase(POUTPUT_STREAM output_stream_obj=NULL);
	virtual ~OutputStreamBase();

	virtual unsigned short output(const char* output_data)const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class OutputMonitorStream : public OutputStreamBase
{
private:
	virtual unsigned short output_internal(const char* output_data)const;

public:
	OutputMonitorStream(POUTPUT_STREAM output_stream_obj=NULL);
	~OutputMonitorStream();
};
typedef OutputMonitorStream* POUTPUT_MONITOR_STREAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct OutputMailStreamCfg
{
	std::string title;
	std::list<std::string> address_list;
};
typedef OutputMailStreamCfg* POUTPUT_MAIL_STREAM_CFG;

class OutputMailStream : public OutputStreamBase
{
private:
	virtual unsigned short output_internal(const char* output_data)const;
	OutputMailStreamCfg mail_param;
	int fd;

public:
	OutputMailStream(const POUTPUT_MAIL_STREAM_CFG output_stream_param, POUTPUT_STREAM output_stream_obj=NULL);
	~OutputMailStream();
};
typedef OutputMailStream* POUTPUT_MAIL_STREAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct OutputFileStreamCfg
{
	std::string file_attribute;
	std::string file_path;
};
typedef OutputFileStreamCfg* POUTPUT_FILE_STREAM_CFG;

class OutputFileStream : public OutputStreamBase
{
private:
	virtual unsigned short output_internal(const char* output_data)const;
	OutputFileStreamCfg file_param;
	FILE* fp;

public:
	OutputFileStream(const POUTPUT_FILE_STREAM_CFG output_stream_param, POUTPUT_STREAM output_stream_obj=NULL);
	~OutputFileStream();
};
typedef OutputFileStream* POUTPUT_FILE_STREAM;

#endif
