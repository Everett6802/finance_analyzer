#ifndef FINANCE_ANALYZER_COMMON_FUNCTION_H
#define FINANCE_ANALYZER_COMMON_FUNCTION_H

#include <stdlib.h>
#include <string>
#include <list>
#include "finance_analyzer_common_definition.h"

FinanceAnalysisMode get_finance_analysis_mode_from_file();
// bool is_market_mode();
// bool is_stock_mode();
int get_source_key(int source_type_index=-1/*, FinanceAnalysisMode finance_analysis_mode=FinanceAnalysis_None*/);
int get_source_key(int company_group_number, const std::string& company_code_number, int source_type_index=-1/*, FinanceAnalysisMode finance_analysis_mode=FinanceAnalysis_None*/);
int get_source_type(int source_key);
std::string get_company_code_number(int source_key);
int get_company_group_number(int source_key);
const char* get_ret_description(unsigned short ret);
const char* get_database_field_description(int source_type_index, int field_index, FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
bool check_source_type_index_in_range(int source_type_index, FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
void get_source_type_index_range(int& source_type_index_start, int& source_type_index_end, FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
int get_source_type_size(FinanceAnalysisMode finance_analysis_mode/*=FinanceAnalysis_None*/);
bool check_field_index_in_range(int source_type_index, int field_index);
bool check_calculation_type_in_range(int calculation_type);
bool check_file_exist(const char* filepath);
bool check_config_file_exist(const char* config_filename);
unsigned short read_file_lines_ex(std::list<std::string>& line_list, const char* filepath, const char* file_read_attribute);
unsigned short read_config_file_lines_ex(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_file_read_attribute, const char* config_folderpath=NULL);
unsigned short read_config_file_lines(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_folderpath=NULL);
unsigned short write_file_lines_ex(const std::list<std::string>& line_list, const char* filepath, const char* file_write_attribute);
unsigned short write_config_file_lines_ex(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_file_write_attribute, const char* config_folderpath=NULL);
unsigned short write_config_file_lines(std::list<std::string>& conf_line_list, const char* config_filename, const char* config_folderpath=NULL);
unsigned short get_config_file_timestamp(std::string& timestamp_string, const char* config_filename, const char* config_folderpath=NULL);
bool check_config_file_timestamp_equal(const char* config_filename1, const char* config_filename2, const char* config_folderpath1=NULL, const char* config_folderpath2=NULL);
unsigned short create_folder_if_not_exist(const char* path, int mode=0755);
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

#endif
