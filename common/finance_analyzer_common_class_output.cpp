#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_common_class_output.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputStreamBase::OutputStreamBase(POUTPUT_STREAM output_stream_obj) :
	output_stream_obj_handle(output_stream_obj)
{
	IMPLEMENT_MSG_DUMPER()
}

OutputStreamBase::~OutputStreamBase()
{
	// fprintf(stderr, "OutputStreamBase release\n");
	if (output_stream_obj_handle != NULL)
	{
		delete output_stream_obj_handle;
		output_stream_obj_handle = NULL;
	}
	RELEASE_MSG_DUMPER()
}

unsigned short OutputStreamBase::output(const char* output_data)const
{
	assert(output_data != NULL && "output_data should NOT be NULL");
	unsigned short ret = RET_SUCCESS;
	if (output_stream_obj_handle != NULL)
	{
		ret = output_stream_obj_handle->output(output_data);
		if (CHECK_FAILURE(ret))
			return ret;
	}
	return output_internal(output_data);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputMonitorStream::OutputMonitorStream(POUTPUT_STREAM output_stream_obj) :
	OutputStreamBase(output_stream_obj)
{

}

OutputMonitorStream::~OutputMonitorStream()
{
	// fprintf(stderr, "~OutputMonitorStream\n");
}

unsigned short OutputMonitorStream::output_internal(const char* output_data)const
{
	printf("%s", output_data);
	return RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputMailStream::OutputMailStream(const POUTPUT_MAIL_STREAM_CFG output_stream_param, POUTPUT_STREAM output_stream_obj) :
	OutputStreamBase(output_stream_obj)
{
	assert(output_stream_param != NULL && "output_stream_param should NOT be NULL");
	// memcpy(&mail_param, output_stream_param, sizeof(OutputMailStreamCfg));
	mail_param.title = output_stream_param->title;
	STRING_LIST::const_iterator iter = output_stream_param->address_list.begin();
	while (iter != output_stream_param->address_list.end())
	{
		string address = (string)*iter;
		mail_param.address_list.push_back(address);
		++iter;
	}
}

OutputMailStream::~OutputMailStream()
{
	// fprintf(stderr, "~OutputMailStream\n");
}

unsigned short OutputMailStream::output_internal(const char* output_data)const
{
	// printf("OutputMailStream\n");
	unsigned short ret = RET_SUCCESS;
	STRING_LIST::const_iterator iter = mail_param.address_list.begin();
	while (iter != mail_param.address_list.end())
	{
		string address = (string)*iter;
		ret = send_email(mail_param.title.c_str(), address.c_str(), output_data);
		if (CHECK_FAILURE(ret))
		{
			WRITE_FORMAT_ERROR("Fail to send mail to %s, due to: %s", address.c_str(), get_ret_description(ret));
			return ret;
		}
		iter++;
	}
	return RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutputFileStream::OutputFileStream(const POUTPUT_FILE_STREAM_CFG output_stream_param, POUTPUT_STREAM output_stream_obj) :
	OutputStreamBase(output_stream_obj),
	fp(NULL)
{
	assert(output_stream_param != NULL && "output_stream_param should NOT be NULL");
	memcpy(&file_param, output_stream_param, sizeof(OutputFileStreamCfg));
	fp = fopen(file_param.file_path.c_str(), file_param.file_attribute.c_str());
	if (fp == NULL)
	{
		static const int ERRMESG_SIZE = 256;
		char errmsg[ERRMESG_SIZE];
		snprintf(errmsg, ERRMESG_SIZE, "Fail to open the file: %s", file_param.file_path.c_str());
		throw runtime_error(errmsg);
	}
}

OutputFileStream::~OutputFileStream()
{
	// fprintf(stderr, "~OutputFileStream\n");
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
}

unsigned short OutputFileStream::output_internal(const char* output_data)const
{
	// printf("OutputFileStream\n");
	assert(fp != NULL && "fp shoule NOT be NULL");
	size_t rest_data_len = strlen(output_data);
	size_t written_data_len = 0;
	while(rest_data_len > 0)
	{
		written_data_len = fwrite(output_data, sizeof(char), rest_data_len, fp);
		if (written_data_len < 0)
		{
			WRITE_FORMAT_ERROR("Fail to write data to %s, due to: %s", file_param.file_path.c_str(), strerror(errno));
			return RET_FAILURE_SYSTEM_API;
		}
		rest_data_len -= written_data_len;
	}

	return RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
