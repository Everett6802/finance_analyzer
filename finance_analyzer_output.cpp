#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_output.h"


using namespace std;

const char DEFAULT_OUTPUT_DATA_SPLIT_SYMBOL = ' ';
const bool DEFAULT_OUTPUT_DATA_SHOW_TITLE = false;

OutputResultParam::OutputResultParam(char output_split_symbol, bool output_show_title) :
	split_symbol(output_split_symbol),
	show_title(output_show_title)
{
}

void OutputResultParam::set_split_symbol(char output_split_symbol){split_symbol = output_split_symbol;}
char OutputResultParam::get_split_symbol()const{return split_symbol;}

void OutputResultParam::set_show_title(char output_show_title){show_title = output_show_title;}
char OutputResultParam::get_show_title()const{return show_title;}

unsigned short output_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const PFINANCE_BOOL_DATA_ARRAY filter_array, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(access_param_deque != NULL && "access_param_deque should NOT be NULL");
//	assert(output_result_param != NULL && "output_result_param should NOT be NULL");
	assert(output_filename != NULL && "output_filename should NOT be NULL");

	static const int FILE_PATH_SIZE = 256;
//	static char folder_path[FILE_PATH_SIZE];
	static const int DATA_ELEMENT_SIZE = 64;
	static char data_element[DATA_ELEMENT_SIZE];
	char output_filepath[FILE_PATH_SIZE];

	char split_symbol = (output_result_param != NULL) ? output_result_param->get_split_symbol() : DEFAULT_OUTPUT_DATA_SPLIT_SYMBOL;
	bool show_title = (output_result_param != NULL) ? output_result_param->get_show_title() : DEFAULT_OUTPUT_DATA_SHOW_TITLE;
	unsigned short ret = create_folder_in_project_if_not_exist(RESULT_FOLDER_NAME);
	if (CHECK_FAILURE(ret))
		return ret;

	char current_working_directory[FILE_PATH_SIZE];
	getcwd(current_working_directory, FILE_PATH_SIZE);
	snprintf(output_filepath, FILE_PATH_SIZE, "%s/%s/%s", current_working_directory, RESULT_FOLDER_NAME, output_filename);
	FILE* fp = fopen(output_filepath, "w");
	if (fp == NULL)
	{
		fprintf(stderr, "Fail to open file: %s, due to: %s", output_filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}

	int data_size = result_set->get_data_size();
	int access_param_deque_size = access_param_deque->size();
// Add title if required
	if (show_title)
	{
		string title_buf = "";
		for (int j = 0 ; j < access_param_deque_size ; j++)
		{
			int finance_source_type = (int)(*access_param_deque)[j]->get_finance_source_type();
			int finance_field_no = (*access_param_deque)[j]->get_finance_field_no();
			switch(FINANCE_DATABASE_FIELD_TYPE_LIST[finance_source_type][finance_field_no])
			{
			case FinanceField_INT:
			case FinanceField_LONG:
			case FinanceField_FLOAT:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%s%d%c", MYSQL_FILED_NAME_BASE, finance_field_no, split_symbol);
			}
			break;
			case FinanceField_DATE:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%s%c", MYSQL_DATE_FILED_NAME, split_symbol);
			}
			break;
			default:
				assert("Unsupported field type");
				ret = RET_FAILURE_INVALID_ARGUMENT;
				goto OUT;
			}
			title_buf += data_element;
		}
		title_buf[title_buf.length() - 1] = '\n';
		fputs(title_buf.c_str(), fp);
	}
// Output each data
	for (int i = 0 ; i < data_size ; i++)
	{
		if (filter_array != NULL && !(*filter_array)[i])
			continue;
		string data_buf = "";
		for (int j = 0 ; j < access_param_deque_size ; j++)
		{
			int finance_source_type = (int)(*access_param_deque)[j]->get_finance_source_type();
			int finance_field_no = (*access_param_deque)[j]->get_finance_field_no();
			switch(FINANCE_DATABASE_FIELD_TYPE_LIST[finance_source_type][finance_field_no])
			{
			case FinanceField_INT:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%d%c", result_set->get_int_array_element(finance_source_type, finance_field_no, i), split_symbol);
			}
			break;
			case FinanceField_LONG:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%ld%c", result_set->get_long_array_element(finance_source_type, finance_field_no, i), split_symbol);
			}
			break;
			case FinanceField_FLOAT:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%.2f%c", result_set->get_float_array_element(finance_source_type, finance_field_no, i), split_symbol);
			}
			break;
			case FinanceField_DATE:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%s%c", result_set->get_date_array_element(i), split_symbol);
			}
			break;
			default:
				assert("Unsupported field type");
				ret = RET_FAILURE_INVALID_ARGUMENT;
				goto OUT;
			}
			data_buf += data_element;
		}
		data_buf[data_buf.length() - 1] = '\n';
		fputs(data_buf.c_str(), fp);
	}
OUT:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}

	return ret;
}

unsigned short output_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	return output_result(result_set, access_param_deque, NULL, output_result_param, output_filename);
}

unsigned short output_2d_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM access_param1, const PRESULT_SET_ACCESS_PARAM access_param2, const PFINANCE_BOOL_DATA_ARRAY filter_array, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(access_param1 != NULL && "access_param1 should NOT be NULL");
	assert(access_param2 != NULL && "access_param2 should NOT be NULL");
	assert(result_set->get_data_dimension() >= 2 && "result_set data dimension SHOULD be larger than 2");

	ResultSetAccessParamDeque access_param_deque;
	access_param_deque.push_back(access_param1);
	access_param_deque.push_back(access_param2);
	unsigned short ret =  output_result(result_set, &access_param_deque, filter_array, output_result_param, output_filename);
	access_param_deque[0] = NULL;
	access_param_deque[1] = NULL;
	return ret;
}

unsigned short output_2d_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM access_param1, const PRESULT_SET_ACCESS_PARAM access_param2, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	return output_2d_result(result_set, access_param1, access_param2, NULL, output_result_param, output_filename);
}

unsigned short output_csv_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const PFINANCE_BOOL_DATA_ARRAY filter_array, const char* output_filename)
{
	OutputResultParam output_result_param;
	output_result_param.set_split_symbol(',');
	output_result_param.set_show_title(true);
	return  output_result(result_set, access_param_deque, filter_array, &output_result_param, output_filename);
}

unsigned short output_csv_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const char* output_filename)
{
	return output_csv_result(result_set, access_param_deque, output_filename);
}