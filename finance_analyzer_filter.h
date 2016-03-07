#ifndef FINANCE_ANALYZER_FILTER_H
#define FINANCE_ANALYZER_FILTER_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


enum FilterRuleType
{
	FilterRule_Equal,
	FilterRule_GreaterThan,
	FilterRule_GreaterEqualThan,
	FilterRule_LessThan,
	FilterRule_LessEqualThan,
	FilterRule_Size
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FilterActionBase
{
private:
	FilterRuleType filter_rule_type;

protected:
	FilterActionBase();

public:
	virtual ~FilterActionBase();
	FilterRuleType get_type()const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
bool filter_rule_equal(T data, T threshold);

extern template bool filter_rule_equal<int>(int data, int threshold);
extern template bool filter_rule_equal<long>(long data, long threshold);
extern template bool filter_rule_equal<float>(float data, float threshold);

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

#endif
