#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_algorithm.h"


using namespace std;

#define IMPLEMENT_SINGLE_INPUT_FORMULA_FUNCTION(X)\
float X(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int start_index, int end_index)\
{\
	assert(finance_data_array != NULL && "finance_data_array should NOT be NULL");\
	float value;\
	switch (finance_data_array->get_type())\
	{\
	case FinanceField_INT:\
		value = X(*(PFINANCE_INT_DATA_ARRAY)finance_data_array, start_index, end_index);\
		break;\
	case FinanceField_LONG:\
		value = X(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array, start_index, end_index);\
		break;\
	case FinanceField_FLOAT:\
		value = X(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array, start_index, end_index);\
		break;\
	default:\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "Unknown type value: %d", finance_data_array->get_type());\
		throw invalid_argument(errmsg);\
	}\
		break;\
	}\
	return value;\
}

#define GET_COMBINED_VALUE(type1, type2) (type1 * 256 + type2)

#define IMPLEMENT_DOUBLE_INPUT_FORMULA_FUNCTION(X)\
float X(const PFINANCE_DATA_ARRAY_BASE finance_data_array1, PFINANCE_DATA_ARRAY_BASE finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2)\
{\
	static const int FinanceField_INT_INT = GET_COMBINED_VALUE(FinanceField_INT, FinanceField_INT);\
	static const int FinanceField_INT_LONG = GET_COMBINED_VALUE(FinanceField_INT, FinanceField_LONG);\
	static const int FinanceField_INT_FLOAT = GET_COMBINED_VALUE(FinanceField_INT, FinanceField_FLOAT);\
	static const int FinanceField_LONG_INT = GET_COMBINED_VALUE(FinanceField_LONG, FinanceField_INT);\
	static const int FinanceField_LONG_LONG = GET_COMBINED_VALUE(FinanceField_LONG, FinanceField_LONG);\
	static const int FinanceField_LONG_FLOAT = GET_COMBINED_VALUE(FinanceField_LONG, FinanceField_FLOAT);\
	static const int FinanceField_FLOAT_INT = GET_COMBINED_VALUE(FinanceField_FLOAT, FinanceField_INT);\
	static const int FinanceField_FLOAT_LONG = GET_COMBINED_VALUE(FinanceField_FLOAT, FinanceField_LONG);\
	static const int FinanceField_FLOAT_FLOAT = GET_COMBINED_VALUE(FinanceField_FLOAT, FinanceField_FLOAT);\
	assert(finance_data_array1 != NULL && "finance_data_array1 should NOT be NULL");\
	assert(finance_data_array2 != NULL && "finance_data_array2 should NOT be NULL");\
	int combined_value = GET_COMBINED_VALUE(finance_data_array1->get_type(), finance_data_array2->get_type());\
	float value;\
	switch (combined_value)\
	{\
	case FinanceField_INT_INT:\
		value = X(*(PFINANCE_INT_DATA_ARRAY)finance_data_array1, *(PFINANCE_INT_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_INT_LONG:\
		value = X(*(PFINANCE_INT_DATA_ARRAY)finance_data_array1, *(PFINANCE_LONG_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_INT_FLOAT:\
		value = X(*(PFINANCE_INT_DATA_ARRAY)finance_data_array1, *(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_LONG_INT:\
		value = X(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array1, *(PFINANCE_INT_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_LONG_LONG:\
		value = X(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array1, *(PFINANCE_LONG_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_LONG_FLOAT:\
		value = X(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array1, *(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_FLOAT_INT:\
		value = X(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array1, *(PFINANCE_INT_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_FLOAT_LONG:\
		value = X(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array1, *(PFINANCE_LONG_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	case FinanceField_FLOAT_FLOAT:\
		value = X(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array1, *(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array2, start_index1, start_index2, end_index1, end_index2);\
		break;\
	default:\
	{\
		char errmsg[64];\
		snprintf(errmsg, 64, "Unknown combined value: %d from (%d, %d)", combined_value, finance_data_array1->get_type(), finance_data_array2->get_type());\
		throw invalid_argument(errmsg);\
	}\
		break;\
	}\
	return value;\
}

static void check_index(int start_index, int end_index, int data_size, const char* array_name="")
{
	static const int BUF_SIZE = 64;
	static char errmsg[BUF_SIZE];

	if (start_index < 0 || start_index >= data_size)
	{
		snprintf(errmsg, BUF_SIZE, "%sstart index is out of range[0, %d)", array_name, data_size);
		throw out_of_range(errmsg);
	}
	if (end_index < 1 || end_index > data_size)
	{
		snprintf(errmsg, BUF_SIZE, "%send index is out of range(1, %d]", array_name, data_size);
		throw out_of_range(errmsg);
	}
	if (start_index >= end_index)
	{
		snprintf(errmsg, BUF_SIZE, "%send index[%d] should be larger than start index[%d]", array_name, end_index, start_index);
		throw invalid_argument(errmsg);
	}
}

template <typename T>
float average(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index, int end_index)
{
	end_index = get_end_index_ex(end_index, finance_data_array.get_size());
	check_index(start_index, end_index, finance_data_array.get_size());

	int data_length = end_index - start_index;
	float average_value = 0.0f;
	float sum = 0.0;
	for (int i = start_index ; i < end_index ; i++)
		sum += finance_data_array[i];
	average_value = sum / data_length;
	return average_value;
}

template float average<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
template float average<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
template float average<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

IMPLEMENT_SINGLE_INPUT_FORMULA_FUNCTION(average)

template <typename T>
float variance(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index, int end_index)
{
	end_index = get_end_index_ex(end_index, finance_data_array.get_size());
	check_index(start_index, end_index, finance_data_array.get_size());

	float average_value = average(finance_data_array, start_index, end_index);
	int data_length = end_index - start_index;
	float variance_value = 0.0;
	float sum = 0.0;
	for (int i = start_index ; i < end_index ; i++)
		sum += pow((finance_data_array[i] - average_value), 2);
	variance_value = sum / data_length;

	return variance_value;
}

template float variance<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
template float variance<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
template float variance<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

IMPLEMENT_SINGLE_INPUT_FORMULA_FUNCTION(variance)

template <typename T>
float standard_deviation(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index, int end_index){return sqrt(variance(finance_data_array, start_index, end_index));}

template float standard_deviation<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
template float standard_deviation<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
template float standard_deviation<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

IMPLEMENT_SINGLE_INPUT_FORMULA_FUNCTION(standard_deviation)

template <typename T1, typename T2>
float covariance(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2)
{
	end_index1 = get_end_index_ex(end_index1, finance_data_array1.get_size());
	check_index(start_index1, end_index1, finance_data_array1.get_size(), "array1");
	end_index2 = get_end_index_ex(end_index2, finance_data_array2.get_size());
	check_index(start_index2, end_index2, finance_data_array2.get_size(), "array2");

	int data_length1 = end_index1 - start_index1;
	int data_length2 = end_index2 - start_index2;
	assert(data_length1 == data_length2 && "The array sizes are NOT identical");
	float average_value1 = average(finance_data_array1, start_index1, end_index1);
	float average_value2 = average(finance_data_array2, start_index2, end_index2);
	float covariance_value = 0.0;
	float sum = 0.0;
	for (int i = 0 ; i < data_length1 ; i++)
	{
		sum += (finance_data_array1[i + start_index1] - average_value1) * (finance_data_array2[i + start_index2] - average_value2);
//		fprintf(stderr, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", (float)finance_data_array1[i + start_index1], (float)average_value1, (float)finance_data_array2[i + start_index2], (float)average_value2, (float)(finance_data_array1[i + start_index1] - average_value1), (float)(finance_data_array2[i + start_index2] - average_value2), (float)(finance_data_array1[i + start_index1] - average_value1) * (finance_data_array2[i + start_index2] - average_value2));
	}
	covariance_value = sum / data_length1;
//	fprintf(stderr, "%.2f %.2f\n", sum, covariance_value);

	return covariance_value;
}

template float covariance<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float covariance<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);

IMPLEMENT_DOUBLE_INPUT_FORMULA_FUNCTION(covariance)


template <typename T1, typename T2>
float correlation(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2)
{
	return covariance(finance_data_array1, finance_data_array2, start_index1, start_index2, end_index1, end_index2) / sqrt(variance(finance_data_array1, start_index1, end_index1) * variance(finance_data_array2, start_index2, end_index2));
}

template float correlation<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
template float correlation<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);

IMPLEMENT_DOUBLE_INPUT_FORMULA_FUNCTION(correlation)
