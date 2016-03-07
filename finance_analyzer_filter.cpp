#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_filter.h"


using namespace std;

//static const int BUF_SIZE = 256;
//static char errmsg[BUF_SIZE];
//
//static void check_index(int start_index, int end_index, int data_size, const char* array_name="")
//{
//	if (start_index < 0 || start_index >= data_size)
//	{
//		snprintf(errmsg, BUF_SIZE, "%sstart index is out of range[0, %d)", array_name, data_size);
//		throw out_of_range(errmsg);
//	}
//	if (end_index < 1 || end_index > data_size)
//	{
//		snprintf(errmsg, BUF_SIZE, "%send index is out of range(1, %d]", array_name, data_size);
//		throw out_of_range(errmsg);
//	}
//	if (start_index >= end_index)
//	{
//		snprintf(errmsg, BUF_SIZE, "%send index[%d] should be larger than start index[%d]", array_name, end_index, start_index);
//		throw invalid_argument(errmsg);
//	}
//}

FilterRuleThresholdBase::FilterRuleThresholdBase(FilterRuleType rule_type){filter_rule_type = rule_type;}
FilterRuleThresholdBase::~FilterRuleThresholdBase(){}
FilterRuleType FilterRuleThresholdBase::get_type()const{return filter_rule_type;}

template <typename T>
FilterRuleThresholdSingleTemplate::FilterRuleThresholdSingleTemplate(FilterRuleType rule_type, T threshold_value) :
	FilterRuleThresholdBase(rule_type)
{
	threshold = threshold_value;
}

template <typename T>
T FilterRuleThresholdSingleTemplate::get_threshold()const{return threshold_value;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
bool filter_rule_equal(T data, T threshold){return (data == threshold);}

template bool filter_rule_equal<int>(int data, int threshold);
template bool filter_rule_equal<long>(long data, long threshold);
template bool filter_rule_equal<float>(float data, float threshold);

template <typename T>
bool filter_rule_greater_than(T data, T threshold){return (data > threshold);}

template bool filter_rule_greater_than<int>(int data, int threshold);
template bool filter_rule_greater_than<long>(long data, long threshold);
template bool filter_rule_greater_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_greater_equal_than(T data, T threshold){return (data >= threshold);}

template bool filter_rule_greater_equal_than<int>(int data, int threshold);
template bool filter_rule_greater_equal_than<long>(long data, long threshold);
template bool filter_rule_greater_equal_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_less_than(T data, T threshold){return (data > threshold);}

template bool filter_rule_less_than<int>(int data, int threshold);
template bool filter_rule_less_than<long>(long data, long threshold);
template bool filter_rule_less_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_less_equal_than(T data, T threshold){return (data >= threshold);}

template bool filter_rule_less_equal_than<int>(int data, int threshold);
template bool filter_rule_less_equal_than<long>(long data, long threshold);
template bool filter_rule_less_equal_than<float>(float data, float threshold);
