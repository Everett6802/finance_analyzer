#ifndef FINANCE_ANALYZER_FILTER_H
#define FINANCE_ANALYZER_FILTER_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


enum FilterRuleType
{
	FilterRule_Equal,
	FilterRule_NotEqual,
	FilterRule_GreaterThan,
	FilterRule_GreaterEqualThan,
	FilterRule_LessThan,
	FilterRule_LessEqualThan,
	FilterRule_Size
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool check_operator_and_exit(bool filter_result);
bool check_operator_or_exit(bool filter_result);

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
