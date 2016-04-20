#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_output.h"
#include "finance_analyzer_algorithm.h"


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

unsigned short output_result(const DataSet* data_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PDATA_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	assert(data_set != NULL && "data_set should NOT be NULL");
	assert(access_param_deque != NULL && "access_param_deque should NOT be NULL");
//	assert(output_result_param != NULL && "output_result_param should NOT be NULL");
	assert(output_filename != NULL && "output_filename should NOT be NULL");

	static const int FILE_PATH_SIZE = 256;
//	static char folder_path[FILE_PATH_SIZE];
	static const int DATA_ELEMENT_SIZE = 64;
	static char data_element[DATA_ELEMENT_SIZE];
	char output_filepath[FILE_PATH_SIZE];

	char split_symbol = (output_result_param != NULL) ? output_result_param->get_split_symbol() : DEFAULT_OUTPUT_DATA_SPLIT_SYMBOL;
	// bool show_title = (output_result_param != NULL) ? output_result_param->get_show_title() : DEFAULT_OUTPUT_DATA_SHOW_TITLE;
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

// Check data size in each array
	int data_array_size = -1;
	int access_param_deque_size = access_param_deque->size();
	for (int j = 1 ; j < access_param_deque_size ; j++)
	{
		int finance_field_type = (int)(*access_param_deque)[j]->get_finance_field_type();
		int array_index = (*access_param_deque)[j]->get_array_index();
		switch(finance_field_type)
		{
		case FinanceField_INT:
			{
				PFINANCE_INT_DATA_ARRAY data_array = data_set->get_int_data_array(array_index);
				assert(data_array != NULL && "Fail to find the INT data array");
				if (data_array_size == -1)
					data_array_size = data_array->get_size();
				else
				{
					int another_data_array_size = data_array->get_size();
					if (another_data_array_size != data_array_size)
					{
						ret = RET_FAILURE_NOT_EQUAL;
						goto OUT;
					}
				}
			}
			break;
			case FinanceField_LONG:
			{
				PFINANCE_LONG_DATA_ARRAY data_array = data_set->get_long_data_array(array_index);
				assert(data_array != NULL && "Fail to find the LONG data array");
				if (data_array_size == -1)
					data_array_size = data_array->get_size();
				else
				{
					int another_data_array_size = data_array->get_size();
					if (another_data_array_size != data_array_size)
					{
						ret = RET_FAILURE_NOT_EQUAL;
						goto OUT;
					}			
				}
			}
			break;
			case FinanceField_FLOAT:
			{
				PFINANCE_FLOAT_DATA_ARRAY data_array = data_set->get_float_data_array(array_index);
				assert(data_array != NULL && "Fail to find the INT data array");
				if (data_array_size == -1)
					data_array_size = data_array->get_size();
				else
				{
					int another_data_array_size = data_array->get_size();
					if (another_data_array_size != data_array_size)
					{
						ret = RET_FAILURE_NOT_EQUAL;
						goto OUT;
					}			
				}
			}
			break;
			default:
				assert("Unsupported field type");
				ret = RET_FAILURE_INVALID_ARGUMENT;
				goto OUT;
			}
	}

// Output each data
	for (int i = 0 ; i < data_array_size ; i++)
	{
		if (filter_array != NULL && !(*filter_array)[i])
			continue;
		string data_buf = "";
		for (int j = 0 ; j < access_param_deque_size ; j++)
		{
			int finance_field_type = (int)(*access_param_deque)[j]->get_finance_field_type();
			int array_index = (*access_param_deque)[j]->get_array_index();
			switch(finance_field_type)
			{
			case FinanceField_INT:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%d%c", data_set->get_int_data_array_element(array_index, i), split_symbol);
			}
			break;
			case FinanceField_LONG:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%ld%c", data_set->get_long_data_array_element(array_index, i), split_symbol);
			}
			break;
			case FinanceField_FLOAT:
			{
				snprintf(data_element, DATA_ELEMENT_SIZE, "%.2f%c", data_set->get_float_data_array_element(array_index, i), split_symbol);
			}
			break;
			// case FinanceField_DATE:
			// {
			// 	snprintf(data_element, DATA_ELEMENT_SIZE, "%s%c", result_set->get_date_array_element(i), split_symbol);
			// }
			// break;
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

unsigned short output_result(const DataSet* data_set, const PDATA_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	return output_result(data_set, NULL, access_param_deque, output_result_param, output_filename);
}

unsigned short output_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
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
	return output_result(result_set, NULL, access_param_deque, output_result_param, output_filename);
}

unsigned short output_2d_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM access_param1, const PRESULT_SET_ACCESS_PARAM access_param2, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(access_param1 != NULL && "access_param1 should NOT be NULL");
	assert(access_param2 != NULL && "access_param2 should NOT be NULL");
	assert(result_set->get_data_dimension() >= 2 && "result_set data dimension SHOULD be larger than 2");

	ResultSetAccessParamDeque access_param_deque;
	access_param_deque.push_back(access_param1);
	access_param_deque.push_back(access_param2);
	unsigned short ret =  output_result(result_set, filter_array, &access_param_deque, output_result_param, output_filename);
	access_param_deque[0] = NULL;
	access_param_deque[1] = NULL;
	return ret;
}

unsigned short output_2d_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM access_param1, const PRESULT_SET_ACCESS_PARAM access_param2, const POUTPUT_RESULT_PARAM output_result_param, const char* output_filename)
{
	return output_2d_result(result_set, NULL, access_param1, access_param2, output_result_param, output_filename);
}

unsigned short output_csv_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const char* output_filename)
{
	OutputResultParam output_result_param;
	output_result_param.set_split_symbol(',');
	output_result_param.set_show_title(true);
	return  output_result(result_set, filter_array, access_param_deque, &output_result_param, output_filename);
}

unsigned short output_csv_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM_DEQUE access_param_deque, const char* output_filename)
{
	return output_csv_result(result_set, NULL, access_param_deque, output_filename);
}

unsigned short output_histogram_result(const ResultSet* result_set, const PFINANCE_BOOL_DATA_ARRAY filter_array, const PRESULT_SET_ACCESS_PARAM access_param, int interval_amount, const char* output_filename)
{
	assert(access_param != NULL && "access_param should NOT be NULL");
	int finance_source_type = (int)access_param->get_finance_source_type();
	int finance_field_no = access_param->get_finance_field_no();
	FinanceIntDataArray data_histogram_statistics;
	unsigned short ret = get_histogram(result_set->get_array(finance_source_type, finance_field_no), interval_amount, data_histogram_statistics);
	if (CHECK_FAILURE(ret))
		return ret;
// 	ResultSet histogram_result_set;
// 	histogram_result_set.add_int_set();
// 	histogram_result_set.deepcopy_int_data_array(0, &data_histogram_statistics);
// 	ResultSetAccessParamDeque histogram_access_param_deque;
// 	histogram_access_param_deque.push_back(new ResultSetAccessParam());
// 	return output_result(histogram_result_set, histogram_access_param_deque, NULL, output_filename);
}

unsigned short output_histogram_result(const ResultSet* result_set, const PRESULT_SET_ACCESS_PARAM access_param, int interval_amount, const char* output_filename)
{
	return output_histogram_result(result_set, NULL, access_param, interval_amount, output_filename);
}
