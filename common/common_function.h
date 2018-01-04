#ifndef COMMON_FUNCTION_H
#define COMMON_FUNCTION_H

#include <stdlib.h>
#include <string>
#include <list>
#include "common_definition.h"
#include "common_class_time.h"


FinanceAnalysisMode get_finance_analysis_mode_from_file();
// bool is_market_mode();
// bool is_stock_mode();
int get_source_key(int method_index=-1/*, FinanceAnalysisMode finance_analysis_mode=FinanceAnalysis_None*/);
int get_source_key(int company_group_number, const std::string& company_code_number, int method_index=-1/*, FinanceAnalysisMode finance_analysis_mode=FinanceAnalysis_None*/);
int get_method(int source_key);
std::string get_company_code_number(int source_key);
int get_company_group_number(int source_key);
const char* get_ret_description(unsigned short ret);
// const int** get_finance_sql_data_field_type_list();
// const int** get_finance_csv_data_field_type_list();
const int** get_finance_data_field_type_list(FinanceDataType finance_data_type=FinanceData_SQL);
// const int* get_finance_sql_data_field_amount_list();
// const int* get_finance_csv_data_field_amount_list();
const int* get_finance_data_field_amount_list(FinanceDataType finance_data_type=FinanceData_SQL);
// const char* get_sql_field_description(int method_index, int field_index);
// const char* get_csv_field_description(int method_index, int field_index);
const char* get_field_description(int method_index, int field_index/*, FinanceAnalysisMode finance_analysis_mode=FinanceAnalysis_None*/, FinanceDataType finance_data_type=FinanceData_SQL);
bool check_method_index_in_range(int method_index, FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
void get_method_index_range(int& method_index_start, int& method_index_end, FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
int get_method_size(FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
// bool check_sql_field_index_in_range(int method_index, int field_index);
// bool check_csv_field_index_in_range(int method_index, int field_index);
bool check_field_index_in_range(int method_index, int field_index, FinanceDataType finance_data_type=FinanceData_SQL);
bool check_calculation_type_in_range(int calculation_type);
bool check_file_exist(const char* filepath);
bool check_config_file_exist(const char* config_filename);
unsigned short get_file_line_count(unsigned int &line_count, const char* filepath);
unsigned short read_file_lines_ex(std::list<std::string>& line_list, const char* filepath, const char* file_read_attribute, PTIME_RANGE_PARAM time_range_param=NULL, char data_seperate_character=',');
unsigned short read_config_file_lines_ex(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_file_read_attribute, const char* config_folderpath=NULL);
unsigned short read_config_file_lines(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_folderpath=NULL);
unsigned short write_file_lines_ex(const std::list<std::string>& line_list, const char* filepath, const char* file_write_attribute);
unsigned short write_config_file_lines_ex(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_file_write_attribute, const char* config_folderpath=NULL);
unsigned short write_config_file_lines(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_folderpath=NULL);
unsigned short get_config_file_timestamp(std::string& timestamp_string, const char* config_filename, const char* config_folderpath=NULL);
bool check_config_file_timestamp_equal(const char* config_filename1, const char* config_filename2, const char* config_folderpath1=NULL, const char* config_folderpath2=NULL);
unsigned short create_folder_if_not_exist(const char* path, int mode=0755);
unsigned short create_folders_if_not_exist(const char* path, const char* ignore_path_prefix="/dev/shm", int mode=0755);
unsigned short create_folder_in_project_if_not_exist(const char* foldername_in_project, int mode=0755);
unsigned short direct_string_to_output_stream(const char* data, const char* filepath=NULL);
unsigned short send_email(const char* title, const char* address, const char* content);
int get_end_index_ex(int end_index, int data_size);
bool check_start_index_in_range(int index, int range_start, int range_end);
bool check_end_index_in_range(int index, int range_start, int range_end);
int get_array_index(int value, const int array[], int array_size);
int get_array_sum(const int array[], int array_size);
unsigned short read_text_file(STRING_LIST& text_result_list, const char* text_filepath);
unsigned short read_text_file(std::string& text_result_string, const char* text_filepath);
void daemonize();
void get_int_deque_from_range_string(const char* int_range_string, INT_DEQUE& int_deque);
void get_int_deque_from_partial_string(char* int_range_string, int int_range_string_len, INT_DEQUE& int_deque);
unsigned short copy_file(const char* src_filepath, const char* dst_filepath);
unsigned short copy_config_file(const char* config_filename, const char* src_config_folderpath);
unsigned short get_process_count(const char* process_name, int& proccss_count);
unsigned short get_process_username(const char* process_name, std::string& process_username);
unsigned short get_absolute_filepath_from_username(const char* relative_filepath, char** absolute_filepath);
unsigned short check_string_is_digit(const char* time_string, int time_string_len=-1);
void traverse_directory(const char* root_filepath, unsigned short (*function_handler)(const char* filepath, void* param), void* function_handler_param);
// Only for test
unsigned short count_filesize_sum(const char* filepath, void* param);
void traverse_directory(const char* root_filepath, int indent=0);
unsigned short gm_add_data(const char* shm_filepath, const void* data, int data_size);
unsigned short gm_delete_data(const char* shm_filepath);
unsigned short gm_read_data(const char* shm_filepath, void** data, int& data_size);
unsigned short gm_write_data(const char* shm_filepath, void* data, int data_size);
unsigned short gm_get_data_ptr(const char* shm_filepath, void** data, int& data_size);
unsigned short gm_put_data_ptr(void* data, int data_size);
#endif
