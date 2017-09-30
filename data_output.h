#ifndef DATA_OUTPUT_H
#define DATA_OUTPUT_H

#include "common.h"


extern const char DEFAULT_OUTPUT_DATA_SPLIT_SYMBOL;
extern const bool DEFAULT_OUTPUT_DATA_SHOW_TITLE;

class OutputResultParam
{
private:
	char split_symbol;
	bool show_title;
public:
	OutputResultParam(char output_split_symbol=DEFAULT_OUTPUT_DATA_SPLIT_SYMBOL, bool output_show_title=DEFAULT_OUTPUT_DATA_SHOW_TITLE);

	void set_split_symbol(char output_split_symbol);
	char get_split_symbol()const;

	void set_show_title(char output_show_title);
	char get_show_title()const;
};
typedef OutputResultParam* POUTPUT_RESULT_PARAM;

unsigned short output_result(const DataSet* data_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PDATA_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param=NULL, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);
unsigned short output_result(const DataSet* data_set, const PDATA_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param=NULL, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);

unsigned short output_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param=NULL, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);
unsigned short output_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param=NULL, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);

unsigned short output_2d_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM access_param1, const PRESULT_SET_ACCESS_PARAM access_param2, const POUTPUT_RESULT_PARAM output_result_param=NULL, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);
unsigned short output_2d_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM access_param1, const PRESULT_SET_ACCESS_PARAM access_param2, const POUTPUT_RESULT_PARAM output_result_param=NULL, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);

unsigned short output_csv_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);
unsigned short output_csv_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);

unsigned short output_histogram_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM access_param, int interval_amount, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);
unsigned short output_histogram_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM access_param, int interval_amount, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);

unsigned short read_output_result(STRING_LIST& output_result_list, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);
unsigned short read_output_result(std::string& output_result_string, const char* output_filename=DEFAULT_OUTPUT_FILENAME_FOR_PLOT);

#endif
