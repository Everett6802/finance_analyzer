#ifndef DATA_FILTER_H
#define DATA_FILTER_H

#include "common.h"


#define FILTER_RULT_IN_RANGE_START_INDEX 100
#define IS_RULE_RANGE_TYPE(X) (X >= FILTER_RULT_IN_RANGE_START_INDEX ? true : false)

enum FilterRuleType
{
	FilterRule_Equal,
	FilterRule_NotEqual,
	FilterRule_GreaterThan,
	FilterRule_GreaterEqualThan,
	FilterRule_LessThan,
	FilterRule_LessEqualThan,
	FilterRule_InRange_LCRC = FILTER_RULT_IN_RANGE_START_INDEX, // Close left side, Close right side
	FilterRule_InRange_LORO, // Open left side, Open right side
	FilterRule_InRange_LCRO, // Close left side, Open right side
	FilterRule_InRange_LORC, // Open left side, Close right side
	FilterRule_OutOfRange_LCRC, // Close left side, Close right side
	FilterRule_OutOfRange_LORO, // Open left side, Open right side
	FilterRule_OutOfRange_LCRO, // Close left side, Open right side
	FilterRule_OutOfRange_LORC, // Open left side, Close right side
};

enum FilterOperatorType
{
	FilterOperator_AND,
	FilterOperator_OR,
	FilterOperator_Size
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FilterRuleThresholdBase
{
private:
	FinanceFieldType filter_data_type;
	FilterRuleType filter_rule_type;

protected:
	FilterRuleThresholdBase(FinanceFieldType data_type, FilterRuleType rule_type);

public:
	virtual ~FilterRuleThresholdBase();
	FinanceFieldType get_data_type()const;
	FilterRuleType get_rule_type()const;
};
typedef FilterRuleThresholdBase* PFILTER_RULE_THRESHOLD_BASE;

template <typename T>
class FilterRuleThresholdTemplate : public FilterRuleThresholdBase
{
private:
	T threshold;

public:
	FilterRuleThresholdTemplate(FinanceFieldType data_type, FilterRuleType rule_type, T threshold_value);
	T get_threshold()const;
};

//typedef FilterRuleThresholdTemplate<int> FilterRuleThresholdInt;
class FilterRuleThresholdInt : public FilterRuleThresholdTemplate<int>
{
public:
	FilterRuleThresholdInt(FilterRuleType rule_type, int threshold_value);
};
typedef FilterRuleThresholdInt* PFILTER_RULE_THRESHOLD_INT;
//typedef FilterRuleThresholdTemplate<long> FilterRuleThresholdLong;
class FilterRuleThresholdLong : public FilterRuleThresholdTemplate<long>
{
public:
	FilterRuleThresholdLong(FilterRuleType rule_type, long threshold_value);
};
typedef FilterRuleThresholdLong* PFILTER_RULE_THRESHOLD_LONG;
//typedef FilterRuleThresholdTemplate<float> FilterRuleThresholdFloat;
class FilterRuleThresholdFloat : public FilterRuleThresholdTemplate<float>
{
public:
	FilterRuleThresholdFloat(FilterRuleType rule_type, float threshold_value);
};
typedef FilterRuleThresholdFloat* PFILTER_RULE_THRESHOLD_FLOAT;

template <typename T>
class FilterRuleThresholdRangeTemplate : public FilterRuleThresholdBase
{
private:
	T l_threshold;
	T r_threshold;

public:
	FilterRuleThresholdRangeTemplate(FinanceFieldType data_type, FilterRuleType rule_type, T l_threshold_value, T r_threshold_value);
	T get_left_threshold()const;
	T get_right_threshold()const;
};

class FilterRuleThresholdRangeInt : public FilterRuleThresholdRangeTemplate<int>
{
public:
	FilterRuleThresholdRangeInt(FilterRuleType rule_type, int l_threshold_value, int r_threshold_value);
};
typedef FilterRuleThresholdRangeInt* PFILTER_RULE_THRESHOLD_RANGE_INT;
class FilterRuleThresholdRangeLong : public FilterRuleThresholdRangeTemplate<long>
{
public:
	FilterRuleThresholdRangeLong(FilterRuleType rule_type, long l_threshold_value, long r_threshold_value);
};
typedef FilterRuleThresholdRangeLong* PFILTER_RULE_THRESHOLD_RANGE_LONG;
class FilterRuleThresholdRangeFloat : public FilterRuleThresholdRangeTemplate<float>
{
public:
	FilterRuleThresholdRangeFloat(FilterRuleType rule_type, float l_threshold_value, float r_threshold_value);
};
typedef FilterRuleThresholdRangeFloat* PFILTER_RULE_THRESHOLD_RANGE_FLOAT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
bool filter_rule_equal(T data, T threshold);

extern template bool filter_rule_equal<int>(int data, int threshold);
extern template bool filter_rule_equal<long>(long data, long threshold);
extern template bool filter_rule_equal<float>(float data, float threshold);

template <typename T>
bool filter_rule_not_equal(T data, T threshold);

extern template bool filter_rule_not_equal<int>(int data, int threshold);
extern template bool filter_rule_not_equal<long>(long data, long threshold);
extern template bool filter_rule_not_equal<float>(float data, float threshold);

template <typename T>
bool filter_rule_greater_than(T data, T threshold);

extern template bool filter_rule_greater_than<int>(int data, int threshold);
extern template bool filter_rule_greater_than<long>(long data, long threshold);
extern template bool filter_rule_greater_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_greater_equal_than(T data, T threshold);

extern template bool filter_rule_greater_equal_than<int>(int data, int threshold);
extern template bool filter_rule_greater_equal_than<long>(long data, long threshold);
extern template bool filter_rule_greater_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_less_than(T data, T threshold);

extern template bool filter_rule_less_than<int>(int data, int threshold);
extern template bool filter_rule_less_than<long>(long data, long threshold);
extern template bool filter_rule_less_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_less_equal_than(T data, T threshold);

extern template bool filter_rule_less_equal_than<int>(int data, int threshold);
extern template bool filter_rule_less_equal_than<long>(long data, long threshold);
extern template bool filter_rule_less_equal_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_in_range_LCRC(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_in_range_LCRC<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_in_range_LCRC<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_in_range_LCRC<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_in_range_LORO(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_in_range_LORO<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_in_range_LORO<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_in_range_LORO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_in_range_LCRO(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_in_range_LCRO<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_in_range_LCRO<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_in_range_LCRO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_in_range_LORC(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_in_range_LORC<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_in_range_LORC<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_in_range_LORC<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LCRC(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_out_of_range_LCRC<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_out_of_range_LCRC<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_out_of_range_LCRC<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LORO(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_out_of_range_LORO<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_out_of_range_LORO<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_out_of_range_LORO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LCRO(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_out_of_range_LCRO<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_out_of_range_LCRO<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_out_of_range_LCRO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LORC(T data, T l_threshold, T r_threshold);

extern template bool filter_rule_out_of_range_LORC<int>(int data, int l_threshold, int r_threshold);
extern template bool filter_rule_out_of_range_LORC<long>(long data, long l_threshold, long r_threshold);
extern template bool filter_rule_out_of_range_LORC<float>(float data, float l_threshold, float r_threshold);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool check_operator_and_exit(bool filter_result);
bool check_operator_or_exit(bool filter_result);

unsigned short add_filter_rule(
	PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	PRESULT_SET_ACCESS_PARAM new_result_set_access_param,
	PFILTER_RULE_THRESHOLD_BASE new_filter_rule_threshold
	);

void show_filter_rule(
	ResultSetAccessParamDeque& result_set_access_param_deque,
	FilterRuleThresholdDeque& filter_rule_threshold_deque
	);

void cleanup_filter_rule(
	ResultSetAccessParamDeque& result_set_access_param_deque,
	FilterRuleThresholdDeque& filter_rule_threshold_deque
	);

unsigned short filter(
	const PRESULT_SET result_set,
	const PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	const PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	FilterOperatorType filter_operator_type,
	FinanceBoolDataArray& filter_array
	);

unsigned short filter_and(
	const PRESULT_SET result_set,
	const PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	const PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	FinanceBoolDataArray& filter_array
);
unsigned short filter_or(
	const PRESULT_SET result_set,
	const PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	const PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	FinanceBoolDataArray& filter_array
);

#endif
