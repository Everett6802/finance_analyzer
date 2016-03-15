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

FilterRuleThresholdBase::FilterRuleThresholdBase(FinanceFieldType data_type, FilterRuleType rule_type)
{
	filter_data_type = data_type;
	filter_rule_type = rule_type;
}
FilterRuleThresholdBase::~FilterRuleThresholdBase(){}
FinanceFieldType FilterRuleThresholdBase::get_data_type()const{return filter_data_type;}
FilterRuleType FilterRuleThresholdBase::get_rule_type()const{return filter_rule_type;}

template <typename T>
FilterRuleThresholdTemplate<T>::FilterRuleThresholdTemplate(FinanceFieldType data_type, FilterRuleType rule_type, T threshold_value) :
	FilterRuleThresholdBase(data_type, rule_type)
{
	threshold = threshold_value;
}

template <typename T>
T FilterRuleThresholdTemplate<T>::get_threshold()const{return threshold;}

FilterRuleThresholdInt::FilterRuleThresholdInt(FilterRuleType rule_type, int threshold_value) :
	FilterRuleThresholdTemplate<int>(FinanceField_INT, rule_type, threshold_value)
{
}

FilterRuleThresholdLong::FilterRuleThresholdLong(FilterRuleType rule_type, long threshold_value) :
	FilterRuleThresholdTemplate<long>(FinanceField_LONG, rule_type, threshold_value)
{
}

FilterRuleThresholdFloat::FilterRuleThresholdFloat(FilterRuleType rule_type, float threshold_value) :
	FilterRuleThresholdTemplate<float>(FinanceField_FLOAT, rule_type, threshold_value)
{
}

template <typename T>
FilterRuleThresholdRangeTemplate<T>::FilterRuleThresholdRangeTemplate(FinanceFieldType data_type, FilterRuleType rule_type, T l_threshold_value, T r_threshold_value) :
	FilterRuleThresholdBase(data_type, rule_type)
{
	l_threshold = l_threshold_value;
	r_threshold = r_threshold_value;
}

template <typename T>
T FilterRuleThresholdRangeTemplate<T>::get_left_threshold()const{return l_threshold;}
template <typename T>
T FilterRuleThresholdRangeTemplate<T>::get_right_threshold()const{return r_threshold;}

FilterRuleThresholdRangeInt::FilterRuleThresholdRangeInt(FilterRuleType rule_type, int l_threshold_value, int r_threshold_value) :
	FilterRuleThresholdRangeTemplate<int>(FinanceField_INT, rule_type, l_threshold_value, r_threshold_value)
{
}

FilterRuleThresholdRangeLong::FilterRuleThresholdRangeLong(FilterRuleType rule_type, long l_threshold_value, long r_threshold_value) :
	FilterRuleThresholdRangeTemplate<long>(FinanceField_LONG, rule_type, l_threshold_value, r_threshold_value)
{
}

FilterRuleThresholdRangeFloat::FilterRuleThresholdRangeFloat(FilterRuleType rule_type, float l_threshold_value, float r_threshold_value) :
	FilterRuleThresholdRangeTemplate<float>(FinanceField_FLOAT, rule_type, l_threshold_value, r_threshold_value)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
bool filter_rule_equal(T data, T threshold){return (data == threshold);}

template bool filter_rule_equal<int>(int data, int threshold);
template bool filter_rule_equal<long>(long data, long threshold);
template bool filter_rule_equal<float>(float data, float threshold);

template <typename T>
bool filter_rule_not_equal(T data, T threshold){return (data != threshold);}

template bool filter_rule_not_equal<int>(int data, int threshold);
template bool filter_rule_not_equal<long>(long data, long threshold);
template bool filter_rule_not_equal<float>(float data, float threshold);

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
bool filter_rule_less_than(T data, T threshold){return (data < threshold);}

template bool filter_rule_less_than<int>(int data, int threshold);
template bool filter_rule_less_than<long>(long data, long threshold);
template bool filter_rule_less_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_less_equal_than(T data, T threshold){return (data <= threshold);}

template bool filter_rule_less_equal_than<int>(int data, int threshold);
template bool filter_rule_less_equal_than<long>(long data, long threshold);
template bool filter_rule_less_equal_than<float>(float data, float threshold);

template <typename T>
bool filter_rule_in_range_LCRC(T data, T l_threshold, T r_threshold)
{
	return filter_rule_greater_equal_than<T>(data, l_threshold) && filter_rule_less_equal_than<T>(data, r_threshold);
}

template bool filter_rule_in_range_LCRC<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_in_range_LCRC<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_in_range_LCRC<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_in_range_LORO(T data, T l_threshold, T r_threshold)
{
	return filter_rule_greater_than<T>(data, l_threshold) && filter_rule_less_than<T>(data, r_threshold);
}

template bool filter_rule_in_range_LORO<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_in_range_LORO<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_in_range_LORO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_in_range_LCRO(T data, T l_threshold, T r_threshold)
{
	return filter_rule_greater_equal_than<T>(data, l_threshold) && filter_rule_less_than<T>(data, r_threshold);
}

template bool filter_rule_in_range_LCRO<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_in_range_LCRO<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_in_range_LCRO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_in_range_LORC(T data, T l_threshold, T r_threshold)
{
	return filter_rule_greater_than<T>(data, l_threshold) && filter_rule_less_equal_than<T>(data, r_threshold);
}

template bool filter_rule_in_range_LORC<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_in_range_LORC<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_in_range_LORC<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LCRC(T data, T l_threshold, T r_threshold)
{
	return filter_rule_less_equal_than<T>(data, l_threshold) || filter_rule_greater_equal_than<T>(data, r_threshold);
}

template bool filter_rule_out_of_range_LCRC<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_out_of_range_LCRC<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_out_of_range_LCRC<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LORO(T data, T l_threshold, T r_threshold)
{
	return filter_rule_less_than<T>(data, l_threshold) || filter_rule_greater_than<T>(data, r_threshold);
}

template bool filter_rule_out_of_range_LORO<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_out_of_range_LORO<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_out_of_range_LORO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LCRO(T data, T l_threshold, T r_threshold)
{
	return filter_rule_less_equal_than<T>(data, l_threshold) || filter_rule_greater_than<T>(data, r_threshold);
}

template bool filter_rule_out_of_range_LCRO<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_out_of_range_LCRO<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_out_of_range_LCRO<float>(float data, float l_threshold, float r_threshold);

template <typename T>
bool filter_rule_out_of_range_LORC(T data, T l_threshold, T r_threshold)
{
	return filter_rule_less_than<T>(data, l_threshold) || filter_rule_greater_equal_than<T>(data, r_threshold);
}

template bool filter_rule_out_of_range_LORC<int>(int data, int l_threshold, int r_threshold);
template bool filter_rule_out_of_range_LORC<long>(long data, long l_threshold, long r_threshold);
template bool filter_rule_out_of_range_LORC<float>(float data, float l_threshold, float r_threshold);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool check_operator_and_exit(bool filter_result){return (filter_result ? false : true);}
bool check_operator_or_exit(bool filter_result){return (filter_result ? true : false);}

unsigned short filter(
	const PRESULT_SET result_set,
	const PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	const PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	FilterOperatorType filter_operator_type,
	FinanceBoolDataArray& filter_array
	)
{
	typedef bool (*PFUNC_FILTER_RULE_INT)(int data, int threshold);
	typedef bool (*PFUNC_FILTER_RULE_LONG)(long data, long threshold);
	typedef bool (*PFUNC_FILTER_RULE_FLOAT)(float data, float threshold);
	static PFUNC_FILTER_RULE_INT filter_rule_int_array[] = {filter_rule_equal<int>, filter_rule_not_equal<int>, filter_rule_greater_than<int>, filter_rule_greater_equal_than<int>, filter_rule_less_than<int>, filter_rule_less_equal_than<int>};
	static PFUNC_FILTER_RULE_LONG filter_rule_long_array[] = {filter_rule_equal<long>, filter_rule_not_equal<long>, filter_rule_greater_than<long>, filter_rule_greater_equal_than<long>, filter_rule_less_than<long>, filter_rule_less_equal_than<long>};
	static PFUNC_FILTER_RULE_FLOAT filter_rule_float_array[] = {filter_rule_equal<float>, filter_rule_not_equal<float>, filter_rule_greater_than<float>, filter_rule_greater_equal_than<float>, filter_rule_less_than<float>, filter_rule_less_equal_than<float>};

	typedef bool (*PFUNC_FILTER_RULE_RANGE_INT)(int data, int l_threshold, int r_threshold);
	typedef bool (*PFUNC_FILTER_RULE_RANGE_LONG)(long data, long l_threshold, long r_threshold);
	typedef bool (*PFUNC_FILTER_RULE_RANGE_FLOAT)(float data, float l_threshold, float r_threshold);
	static PFUNC_FILTER_RULE_RANGE_INT filter_rule_range_int_array[] = {filter_rule_in_range_LCRC<int>, filter_rule_in_range_LORO<int>, filter_rule_in_range_LCRO<int>, filter_rule_in_range_LORC<int>, filter_rule_out_of_range_LCRC<int>, filter_rule_out_of_range_LORO<int>, filter_rule_out_of_range_LCRO<int>, filter_rule_out_of_range_LORC<int>};
	static PFUNC_FILTER_RULE_RANGE_LONG filter_rule_range_long_array[] = {filter_rule_in_range_LCRC<long>, filter_rule_in_range_LORO<long>, filter_rule_in_range_LCRO<long>, filter_rule_in_range_LORC<long>, filter_rule_out_of_range_LCRC<long>, filter_rule_out_of_range_LORO<long>, filter_rule_out_of_range_LCRO<long>, filter_rule_out_of_range_LORC<long>};
	static PFUNC_FILTER_RULE_RANGE_FLOAT filter_rule_range_float_array[] = {filter_rule_in_range_LCRC<float>, filter_rule_in_range_LORO<float>, filter_rule_in_range_LCRO<float>, filter_rule_in_range_LORC<float>, filter_rule_out_of_range_LCRC<float>, filter_rule_out_of_range_LORO<float>, filter_rule_out_of_range_LCRO<float>, filter_rule_out_of_range_LORC<float>};

	typedef bool (*PFUNC_CHECK_OPERATOR_EXIT)(bool filter_result);
	static PFUNC_CHECK_OPERATOR_EXIT check_operator_exit_array[] = {check_operator_and_exit, check_operator_or_exit};

	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(result_set_access_param_deque != NULL && "result_set_access_param_deque should NOT be NULL");
	assert(filter_rule_threshold_deque != NULL && "filter_rule_threshold_deque should NOT be NULL");
	size_t data_dimension = result_set_access_param_deque->size();
	assert(data_dimension == filter_rule_threshold_deque->size() && "The size of filter_rule_threshold_array is NOT identical to the one of result_set_access_param_array");
	int data_size = result_set->get_data_size();
	for (int i = 0 ; i < data_size ; i++)
	{
		bool check_can_exit = false;
		bool filter_result = false;
		for (unsigned int data_index = 0 ; data_index < data_dimension ; data_index++)
		{
			const PRESULT_SET_ACCESS_PARAM result_set_access_param = (*result_set_access_param_deque)[data_index];
			const PFILTER_RULE_THRESHOLD_BASE filter_rule_threshold = (*filter_rule_threshold_deque)[data_index];
			assert(result_set_access_param != NULL && "result_set_access_param should NOT be NULL");
			assert(filter_rule_threshold != NULL && "filter_rule_threshold should NOT be NULL");
			FinanceSourceType source_type = result_set_access_param->get_finance_source_type();
			int field_no = result_set_access_param->get_finance_field_no();
			switch(FINANCE_DATABASE_FIELD_TYPE_LIST[source_type][field_no])
			{
			case FinanceField_INT:
			{
				assert(filter_rule_threshold->get_data_type() == FinanceField_INT && "The data type of filter_rule_threshold is NOT FinanceField_INT");
				if (IS_RULE_RANGE_TYPE(filter_rule_threshold->get_rule_type()))
					filter_result = (*filter_rule_range_int_array[filter_rule_threshold->get_rule_type() - FILTER_RULT_IN_RANGE_START_INDEX])(result_set->get_int_array_element(source_type, field_no, i), ((PFILTER_RULE_THRESHOLD_RANGE_INT)filter_rule_threshold)->get_left_threshold(), ((PFILTER_RULE_THRESHOLD_RANGE_INT)filter_rule_threshold)->get_right_threshold());
				else
					filter_result = (*filter_rule_int_array[filter_rule_threshold->get_rule_type()])(result_set->get_int_array_element(source_type, field_no, i), ((PFILTER_RULE_THRESHOLD_INT)filter_rule_threshold)->get_threshold());
			}
			break;
			case FinanceField_LONG:
			{
				assert(filter_rule_threshold->get_data_type() == FinanceField_LONG && "The data type of filter_rule_threshold is NOT FinanceField_LONG");
				if (IS_RULE_RANGE_TYPE(filter_rule_threshold->get_rule_type()))
					filter_result = (*filter_rule_range_long_array[filter_rule_threshold->get_rule_type() - FILTER_RULT_IN_RANGE_START_INDEX])(result_set->get_long_array_element(source_type, field_no, i), ((PFILTER_RULE_THRESHOLD_RANGE_LONG)filter_rule_threshold)->get_left_threshold(), ((PFILTER_RULE_THRESHOLD_RANGE_LONG)filter_rule_threshold)->get_right_threshold());
				else
					filter_result = (*filter_rule_long_array[filter_rule_threshold->get_rule_type()])(result_set->get_long_array_element(source_type, field_no, i), ((PFILTER_RULE_THRESHOLD_LONG)filter_rule_threshold)->get_threshold());
			}
			break;
			case FinanceField_FLOAT:
			{
				assert(filter_rule_threshold->get_data_type() == FinanceField_FLOAT && "The data type of filter_rule_threshold is NOT FinanceField_FLOAT");
				if (IS_RULE_RANGE_TYPE(filter_rule_threshold->get_rule_type()))
					filter_result = (*filter_rule_range_float_array[filter_rule_threshold->get_rule_type() - FILTER_RULT_IN_RANGE_START_INDEX])(result_set->get_float_array_element(source_type, field_no, i), ((PFILTER_RULE_THRESHOLD_RANGE_FLOAT)filter_rule_threshold)->get_left_threshold(), ((PFILTER_RULE_THRESHOLD_RANGE_FLOAT)filter_rule_threshold)->get_right_threshold());
				else
					filter_result = (*filter_rule_float_array[filter_rule_threshold->get_rule_type()])(result_set->get_float_array_element(source_type, field_no, i), ((PFILTER_RULE_THRESHOLD_FLOAT)filter_rule_threshold)->get_threshold());
			}
			break;
			case FinanceField_DATE:
				assert("The DATE field type is NOT supported");
				return RET_FAILURE_INVALID_ARGUMENT;
			default:
				assert("The unsupported field type");
				return RET_FAILURE_INVALID_ARGUMENT;
			}
			if ((*check_operator_exit_array[filter_operator_type])(filter_result))
			{
				check_can_exit = true;
				break;
			}
		}
		if (check_can_exit)
			filter_array.add(((filter_operator_type == FilterOperator_AND) ? false : true));
		else
			filter_array.add(((filter_operator_type == FilterOperator_AND) ? true : false));
	}

	return RET_SUCCESS;
}

unsigned short filter_and(
	const PRESULT_SET result_set,
	const PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	const PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	FinanceBoolDataArray& filter_array
	)
{
	return filter(result_set, result_set_access_param_deque, filter_rule_threshold_deque, FilterOperator_AND, filter_array);
}
unsigned short filter_or(
	const PRESULT_SET result_set,
	const PRESULT_SET_ACCESS_PARAM_DEQUE result_set_access_param_deque,
	const PFILTER_RULE_THRESHOLD_DEQUE filter_rule_threshold_deque,
	FinanceBoolDataArray& filter_array
	)
{
	return filter(result_set, result_set_access_param_deque, filter_rule_threshold_deque, FilterOperator_OR, filter_array);
}
