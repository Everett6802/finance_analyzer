#ifndef FINANCE_ANALYZER_COMMON_FUNCTION_H
#define FINANCE_ANALYZER_COMMON_FUNCTION_H

#include <stdlib.h>
#include "finance_analyzer_common_definition.h"


bool is_market_mode();
bool is_stock_mode();
int get_source_key(int source_type_index=-1);
int get_source_key(int company_group_number, const std::string& company_code_number, int source_type_index=-1);
int get_source_type(int source_key);
std::string get_company_code_number(int source_key);
int get_company_group_number(int source_key);
const char* get_ret_description(unsigned short ret);
const char* get_database_field_description(int source_type_index, int field_index);
bool check_source_type_index_in_range(int source_type_index);
bool check_field_index_in_range(int source_type_index, int field_index);
bool check_calculation_type_in_range(int calculation_type);
bool check_file_exist(const char* filepath);
bool check_config_file_exist(const char* filepath);
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

#endif
