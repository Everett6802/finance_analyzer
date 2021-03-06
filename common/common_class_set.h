#ifndef COMMON_CLASS_SET_H
#define COMMON_CLASS_SET_H

#include "msg_dumper_wrapper.h"
#include "common_definition.h"
#include "common_function.h"
#include "common_class_base.h"
#include "common_class_smart_pointer.h"
#include "common_class_company_profile.h"


class DataSetAccessParam;
class ResultSetAccessParam;
class FilterRuleThresholdBase;

typedef std::deque<DataSetAccessParam*> DataSetAccessParamDeque;
typedef DataSetAccessParamDeque* PDATA_SET_ACCESS_PARAM_DEQUE;
typedef std::deque<ResultSetAccessParam*> ResultSetAccessParamDeque;
typedef ResultSetAccessParamDeque* PRESULT_SET_ACCESS_PARAM_DEQUE;
typedef std::deque<FilterRuleThresholdBase*> FilterRuleThresholdDeque;
typedef FilterRuleThresholdDeque* PFILTER_RULE_THRESHOLD_DEQUE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class

#define ADD_QUERY(QuerySet, SourceIndex, FieldIndex)\
do{\
ret = QuerySet.add_query(SourceIndex, FieldIndex);\
if (CHECK_FAILURE(ret))\
	return ret;\
}while(0)

#define ADD_QUERY_EX(QuerySet, SourceIndex, FieldIndex, SourceTypeIndexSet)\
do{\
ADD_QUERY(QuerySet, SourceIndex, FieldIndex);\
SourceTypeIndexSet.insert(SourceIndex);\
}while(0)

// class QuerySet
// {
// typedef std::map<int, INT_DEQUE> INT_INT_DEQUE_MAP;
// typedef INT_INT_DEQUE_MAP* PINT_INT_DEQUE_MAP;
// protected:
// 	DECLARE_MSG_DUMPER()

// 	INT_DEQUE query_set[FinanceSourceSize];
// 	bool add_done;

// public:
// 	QuerySet();
// 	~QuerySet();

// 	unsigned short add_query(int method_index, int field_index=-1);
// 	unsigned short add_query_done();
// 	bool is_add_query_done()const;
// 	int get_size()const;

// 	class iterator
// 	{
// 	private:
// 		PINT_DEQUE ptr_;
// 		PINT_DEQUE ptr_end_;
// //		void get_next()
// //		{
// //			while(true)
// //			{
// //				ptr_++;
// //				if (ptr_ != ptr_end_ && !ptr_->empty()) break;
// //			}
// //		}

// 	public:
// 		iterator(const PINT_DEQUE ptr) : ptr_(ptr), ptr_end_(ptr + FinanceSourceSize) {}
// 		iterator operator++()
// 		{
// 			ptr_++;
// //			get_next();
// 			return *this;
// 		}
// 		iterator operator++(int junk)
// 		{
// 			iterator i = *this;
// 			ptr_++;
// //			get_next();
// 			return i;
// 		}
// 		const INT_DEQUE& operator*() { return *ptr_; }
// 		const PINT_DEQUE operator->() { return ptr_; }
// 	};

// 	iterator begin() { return iterator(query_set); }
// 	iterator end() { return iterator(query_set + FinanceSourceSize); }

// 	const INT_DEQUE& operator[](int index)const;
// };
class QuerySet
{
private:
	DECLARE_MSG_DUMPER()

public:
	class const_iterator
	{
	private:
		INT_INT_DEQUE_MAP_ITER iter;

	public:
		const_iterator(INT_INT_DEQUE_MAP_ITER iterator);
		const_iterator operator++();
		bool operator==(const const_iterator& another);
		bool operator!=(const const_iterator& another);
		const PINT_DEQUE operator->();
		const INT_DEQUE& operator*();
		int get_first()const;
		const PINT_DEQUE get_second()const;
	};

	static unsigned short create_instance_from_string(FinanceAnalysisMode cur_finance_analysis_mode, FinanceDataType cur_finance_data_type, const char* source_string, QuerySet& query_set);
	static unsigned short create_instance_from_string(FinanceAnalysisMode cur_finance_analysis_mode, FinanceDataType cur_finance_data_type, const char* source_string, QuerySet** query_set);

private:
	FinanceDataType finance_data_type;
	// const int** FINANCE_DATA_FIELD_TYPE_LIST;
	// const int* FINANCE_DATA_FIELD_AMOUNT_LIST;
	DECLARE_FINANCE_DATA_FIELD_TYPE_LIST()
	DECLARE_FINANCE_DATA_FIELD_AMOUNT_LIST()
	bool add_done;
	FinanceAnalysisMode finance_analysis_mode;
	mutable INT_INT_DEQUE_MAP source_field_query_map;
	mutable PINT_SET method_index_set;
	mutable std::string query_set_string;

	unsigned short init_source_field_query_map_element(int method_index);
	unsigned short init_method_index_set();

public:
	QuerySet(FinanceDataType cur_finance_data_type/*=FinanceData_SQL*/);
	QuerySet(const QuerySet& another_query_set);
	~QuerySet();

	const std::string& to_string();

	const_iterator begin();
	const_iterator end();

	FinanceDataType get_data_type()const;
	unsigned short add_query(int method_index, int field_index=-1);
	unsigned short add_query_list(int method_index, const PINT_DEQUE field_index_deque);
	unsigned short add_query_done(FinanceAnalysisMode cur_finance_analysis_mode);
	bool is_add_query_done()const;
	int get_size()const;
	unsigned short get_query_sub_set(int method_index, QuerySet** query_sub_set)const;
	const PINT_SET get_method_index_set();

	const INT_DEQUE& operator[](int method_index);
};
typedef QuerySet* PQUERY_SET;
// typedef QuerySet MarketQuerySet;
// typedef MarketQuerySet* PMARKET_QUERY_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CompanyGroupSet
{
private:
	DECLARE_MSG_DUMPER()
	DECLARE_COMPANY_PROFILE()

private:
	static PINT_STRING_DEQUE_MAP whole_company_number_in_group_map;

	static void init_whole_company_number_in_group_map();
	static int get_company_group_size();

public: 
	static unsigned short create_instance_from_string(const char* source_string, CompanyGroupSet& company_group_set);
	static unsigned short create_instance_from_string(const char* source_string, CompanyGroupSet** company_group_set);

public:
	class const_iterator
	{
	private:
		INT_STRING_DEQUE_MAP_ITER iter;

	public:
		const_iterator(INT_STRING_DEQUE_MAP_ITER iterator);
		const_iterator operator++();
		bool operator==(const const_iterator& another);
		bool operator!=(const const_iterator& another);
		const PSTRING_DEQUE operator->();
		const STRING_DEQUE& operator*();
		int get_first()const;
		const PSTRING_DEQUE get_second()const;
	};

private:
	bool add_done;
	mutable PINT_STRING_DEQUE_MAP company_number_in_group_map;
	mutable std::string company_group_set_string;
	unsigned short init_company_number_in_group_map_elememnt(int company_group_number);
	PSTRING_DEQUE get_company_number_in_group_map_elememnt(int company_group_number);

public:
	const std::string& to_string();

	const_iterator begin();
	const_iterator end();

	CompanyGroupSet();
	CompanyGroupSet(const CompanyGroupSet& another_company_group_set);
	~CompanyGroupSet();

	unsigned short add_company_list_in_group(int company_group_number, const PSTRING_DEQUE company_code_number_in_group_deque);
	unsigned short add_company(int company_group_number, std::string company_code_number);
	unsigned short add_company(std::string company_code_number);
	unsigned short add_company_group(int company_group_number);
	unsigned short add_company_done();
	bool is_add_company_done()const;
	const PSTRING_DEQUE get_company_number_in_group_list(int company_group_index)const;
};
typedef CompanyGroupSet* PCOMPANY_GROUP_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class StockQuerySet : public QuerySet
// {
// public: 
// 	static unsigned short create_instance_from_string(const char* query_source_string, const char* company_source_string, StockQuerySet& stock_query_set);
// 	static unsigned short create_instance_from_string(const char* query_source_string, const char* company_source_string, StockQuerySet** stock_query_set);

// protected:
// 	bool add_done;
// 	CompanyGroupSet company_group_set;

// public:
// 	const std::string& to_string();

// 	StockQuerySet();
// 	~StockQuerySet();

// 	unsigned short add_company_list_in_group(int company_group_number, const PSTRING_DEQUE company_code_number_in_group_deque);
// 	unsigned short add_company(int company_group_number, std::string company_code_number);
// 	unsigned short add_company(std::string company_code_number);
// 	unsigned short add_company_group(int company_group_number);
// 	unsigned short add_query_done();
// 	bool is_add_query_done()const;
// 	const CompanyGroupSet& get_company_group_set()const;
// 	CompanyGroupSet& get_company_group_set();
// };
// typedef StockQuerySet* PSTOCK_QUERY_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SearchRuleSet
{
private:
	DECLARE_MSG_DUMPER()

public: 
	static unsigned short create_instance_from_string(FinanceAnalysisMode cur_finance_analysis_mode, FinanceDataType cur_finance_data_type, const char* query_source_string, const char* time_source_string, const char* company_source_string, SearchRuleSet& search_rule_set);
	static unsigned short create_instance_from_string(FinanceAnalysisMode cur_finance_analysis_mode, FinanceDataType cur_finance_data_type, const char* query_source_string, const char* time_source_string, const char* company_source_string, SearchRuleSet** search_rule_set);

private:
	bool add_done;
	FinanceAnalysisMode finance_analysis_mode;
	FinanceDataType finance_data_type;
	PQUERY_SET query_set;
	// PTIME_RANGE_CFG time_range_cfg;
	PTIME_RANGE_PARAM time_range_param;
	PCOMPANY_GROUP_SET company_group_set;
	mutable std::string search_rule_set_string;

	unsigned short set_finance_mode(FinanceAnalysisMode cur_finance_analysis_mode);
	unsigned short set_finance_data_type(FinanceDataType cur_finance_data_type);

public:
	SearchRuleSet(FinanceAnalysisMode cur_finance_analysis_mode, FinanceDataType cur_finance_data_type/*=FinanceData_SQL*/);
	SearchRuleSet();

	~SearchRuleSet();

	const std::string& to_string();
	unsigned short add_query_rule(const PQUERY_SET new_query_set);
	unsigned short add_query_rule(const char* query_source_string);
	unsigned short add_time_rule(const PTIME_RANGE_PARAM new_time_range_param);
	unsigned short add_time_rule(const char* time_source_string);
	unsigned short add_company_rule(const PCOMPANY_GROUP_SET new_company_group_set);
	unsigned short add_company_rule(const char* company_source_string);
	unsigned short add_rule_done();
	bool is_add_rule_done()const;
	FinanceAnalysisMode get_finance_mode()const;
	FinanceDataType get_data_type()const;
	const PQUERY_SET get_query_rule()const;
	const PTIME_RANGE_PARAM get_time_rule()const;
	const PCOMPANY_GROUP_SET get_company_rule()const;
};
typedef SearchRuleSet* PSEARCH_RULE_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DataSetAccessParam
{
private:
	FinanceFieldType finance_field_type;
	int array_index;
	int start_index;
	int end_index;

public:
	DataSetAccessParam(FinanceFieldType new_finance_field_type, int new_array_index, int new_start_index=0, int new_end_index=-1);

	void set_finance_field_type(FinanceFieldType new_finance_field_type);
	FinanceFieldType get_finance_field_type()const;

	void set_array_index(int new_array_index);
	int get_array_index()const;

	void set_start_index(int new_start_index);
	int get_start_index()const;

	void set_end_index(int new_end_index);
	int get_end_index()const;
};
typedef DataSetAccessParam* PDATA_SET_ACCESS_PARAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DataSet
{
protected:
	DECLARE_MSG_DUMPER()

	std::deque<PFINANCE_INT_DATA_ARRAY> int_data_set;
	std::deque<PFINANCE_LONG_DATA_ARRAY> long_data_set;
	std::deque<PFINANCE_FLOAT_DATA_ARRAY> float_data_set;
	int date_data_size;
	int date_data_cur_pos;
	int int_data_set_size;
	int long_data_set_size;
	int float_data_set_size;

public:
	DataSet();
	~DataSet();

	unsigned short add_int_set();
	unsigned short add_long_set();
	unsigned short add_float_set();

#define DECLARE_DEEPCOPY_DATA_ARRAY_FUNC(n, m) unsigned short deepcopy_##n##_data_array(int set_index, const PFINANCE_##m##_DATA_ARRAY src_##n##_data_array);
	DECLARE_DEEPCOPY_DATA_ARRAY_FUNC(int, INT)
	DECLARE_DEEPCOPY_DATA_ARRAY_FUNC(long, LONG)
	DECLARE_DEEPCOPY_DATA_ARRAY_FUNC(float, FLOAT)
	// unsigned short deepcopy_int_data_array(int set_index, const FinanceIntDataArray* src_int_data_array);
	// unsigned short deepcopy_long_data_array(int set_index, const FinanceLongDataArray* src_long_data_array);
	// unsigned short deepcopy_float_data_array(int set_index, const FinanceFloatDataArray* src_float_data_array);
	unsigned short deepcopy_data_array(int set_index, const PFINANCE_DATA_ARRAY_BASE src_data_array);

#define DECLARE_GET_DATA_ARRAY_FUNC(n, m) const PFINANCE_##m##_DATA_ARRAY get_##n##_data_array(int set_index)const;
	DECLARE_GET_DATA_ARRAY_FUNC(int, INT)
	DECLARE_GET_DATA_ARRAY_FUNC(long, LONG)
	DECLARE_GET_DATA_ARRAY_FUNC(float, FLOAT)
#define DECLARE_GET_DATA_ARRAY_ELEMENT_FUNC(n) n get_##n##_data_array_element(int set_index, int index)const;
	DECLARE_GET_DATA_ARRAY_ELEMENT_FUNC(int)
	DECLARE_GET_DATA_ARRAY_ELEMENT_FUNC(long)
	DECLARE_GET_DATA_ARRAY_ELEMENT_FUNC(float)
};
typedef DataSet* PDATA_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ResultSetAccessParam
{
private:
	FinanceMethodType finance_method;
	int finance_field_no;
	ArrayElementCalculationType calculation_type;
	int start_index;
	int end_index;

public:
	ResultSetAccessParam(FinanceMethodType new_finance_method, int new_finance_field_no, ArrayElementCalculationType new_calculation_type=ArrayElementCalculation_None, int new_start_index=0, int new_end_index=-1);

	void set_finance_method(FinanceMethodType new_finance_method);
	FinanceMethodType get_finance_method()const;

	void set_finance_field_no(int new_finance_field_no);
	int get_finance_field_no()const;

	void set_calculation_type(ArrayElementCalculationType new_calculation_type);
	ArrayElementCalculationType get_calculation_type()const;

	void set_start_index(int new_start_index);
	int get_start_index()const;

	void set_end_index(int new_end_index);
	int get_end_index()const;
};
typedef ResultSetAccessParam* PRESULT_SET_ACCESS_PARAM;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<unsigned short, unsigned short> SHORT_MAP;
typedef SHORT_MAP* PSHORT_MAP;
typedef SHORT_MAP::iterator SHORT_MAP_ITER;
typedef SHORT_MAP_ITER* PSHORT_MAP_ITER;
class ResultSet : public DataSet
{
	// DECLARE_MSG_DUMPER()
public:
	static unsigned short get_combined_index(int x, int y);
	static unsigned long get_combined_index_ex(int x, int y, ArrayElementCalculationType calculation_type=ArrayElementCalculation_None);
	static unsigned short get_upper_subindex(unsigned short x);
	static unsigned short get_lower_subindex(unsigned short x);
	static unsigned short get_calculation_subindex(unsigned long x);
	static void get_metadata_string(const ResultSet& result_set, std::string &result_set_metadata_string);
	static void get_string(const ResultSet& result_set, std::string &result_set_string);
	static void generate_data_for_simulation(ResultSet& result_set);
	static void generate_filtered_data_for_simulation(ResultSet& result_set, FinanceBoolDataArray& filter_data_array);

private:
	FinanceDataType finance_data_type;
	// const int** FINANCE_DATA_FIELD_TYPE_LIST;
	// const int* FINANCE_DATA_FIELD_AMOUNT_LIST;
	DECLARE_FINANCE_DATA_FIELD_TYPE_LIST()
	DECLARE_FINANCE_DATA_FIELD_AMOUNT_LIST()
	mutable std::string result_set_metadata_string;
	mutable std::string result_set_string;
	std::map<unsigned short, unsigned short> data_set_mapping;
	std::map<unsigned long, unsigned long> data_calculation_set_mapping;
	FinanceCharDataPtrArray date_data;
	mutable bool check_date_data_mode;
	int data_set_mapping_size;

	unsigned short find_data_pos(int method_index, int field_index, unsigned short& field_type_index, unsigned short& field_type_pos)const;
	unsigned short add_calculation_set_dummy(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_diff(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_rate(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_sum(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n);
	unsigned short add_calculation_set_sum5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_sum10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_sum20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_sum60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_avg(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n);
	unsigned short add_calculation_set_avg5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_avg10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_avg20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_avg60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_weighted_avg(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex, int n);
	unsigned short add_calculation_set_weighted_avg5(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_weighted_avg10(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_weighted_avg20(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set_weighted_avg60(const PFINANCE_DATA_ARRAY_BASE data_array_base, int key_ex);
	unsigned short add_calculation_set(int method_index, int field_index, ArrayElementCalculationType calculation_type);

public:
	ResultSet(FinanceDataType cur_finance_data_type=FinanceData_SQL);
	~ResultSet();
	FinanceDataType get_data_type()const;
	const std::string& to_metadata_string();
	const std::string& to_string();

	class iterator
	{
	private:
		SHORT_MAP_ITER iter;

	public:
		iterator(SHORT_MAP_ITER iterator) : iter(iterator){}
		iterator operator++()
		{
			iter++;
			return *this;
		}
		iterator operator++(int junk)
		{
			iterator i = *this;
			iter++;
			return i;
		}
		bool operator==(const iterator& another)
		{
			if (this == &another)
				return true;
			return iter == another.iter;
		}
		bool operator!=(const iterator& another){return !(*this == another);}
		const SHORT_MAP_ITER operator->(){return iter;}
	};

	iterator begin() {return iterator(data_set_mapping.begin());}
	iterator end() {return iterator(data_set_mapping.end());}

	unsigned short add_set(int method_index, int field_index);
	unsigned short add_set(int method_index, const INT_DEQUE& field_set);
	unsigned short set_date(const char* element_value);
	unsigned short set_data(int method_index, int field_index, const char* data_string);

	void switch_to_check_date_mode();
	unsigned short check_data()const;
	unsigned short show_data_mapping()const;
	unsigned short show_data()const;

	int get_data_dimension()const;
	int get_data_size()const;

#define DECLARE_GET_ARRAY_FUNC_BY_POS(n, m) const PFINANCE_##m##_DATA_ARRAY get_##n##_array_by_position(unsigned short field_type_index, unsigned short field_type_pos)const;
	DECLARE_GET_ARRAY_FUNC_BY_POS(int, INT)
	DECLARE_GET_ARRAY_FUNC_BY_POS(long, LONG)
	DECLARE_GET_ARRAY_FUNC_BY_POS(float, FLOAT)
#define DECLARE_GET_ARRAY_FUNC(n, m) const PFINANCE_##m##_DATA_ARRAY get_##n##_array(int method_index, int field_index)const;
	DECLARE_GET_ARRAY_FUNC(int, INT)
	DECLARE_GET_ARRAY_FUNC(long, LONG)
	DECLARE_GET_ARRAY_FUNC(float, FLOAT)
	const PFINANCE_DATA_ARRAY_BASE get_array(int method_index, int field_index)const;
	const PFINANCE_DATA_ARRAY_BASE get_array(int method_index, int field_index, ArrayElementCalculationType calculation_type);
	const PFINANCE_CHAR_DATA_PTR_ARRAY get_date_array()const;
#define DECLARE_GET_ARRAY_ELEMENT_FUNC(n) n get_##n##_array_element(int method_index, int field_index, int index)const;
	DECLARE_GET_ARRAY_ELEMENT_FUNC(int)
	DECLARE_GET_ARRAY_ELEMENT_FUNC(long)
	DECLARE_GET_ARRAY_ELEMENT_FUNC(float)
	const char* get_date_array_element(int index)const;
	int get_date_array_element_index(const char* date_str)const;
};
typedef ResultSet* PRESULT_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ResultSetMap
{
	typedef std::map<int, PRESULT_SET> INT_RESULT_SET_MAP;
	typedef INT_RESULT_SET_MAP* PINT_RESULT_SET_MAP;
	typedef INT_RESULT_SET_MAP::iterator INT_RESULT_SET_MAP_ITER;

private:
	FinanceDataType finance_data_type;
	mutable std::string result_set_map_metadata_string;
	mutable std::string result_set_map_string;
	INT_RESULT_SET_MAP result_set_map;
	ResultSetDataUnit result_set_data_unit;

public:
	ResultSetMap(FinanceDataType data_type/*=FinanceData_SQL*/, ResultSetDataUnit data_unit=ResultSetDataUnit_SingleMethod);
	~ResultSetMap();
	FinanceDataType get_data_type()const;
	const std::string& to_metadata_string();
	const std::string& to_string();
	ResultSetDataUnit get_data_unit()const;
	unsigned short register_result_set(int source_key, const PRESULT_SET result_set);
	const PRESULT_SET lookup_result_set(int source_key)const;
};
typedef ResultSetMap* PRESULT_SET_MAP;

#endif
