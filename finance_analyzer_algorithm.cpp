#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_algorithm.h"


using namespace std;

template <typename T>
float average(const FinanceDataArrayTemplate<T>& finance_data_array)
{
	float average_value = 0.0f;
	int data_size = finance_data_array.get_size();
	if (data_size != 0)
	{
		float sum = 0.0;
		for (int i = 0 ; i < data_size ; i++)
			sum += finance_data_array[i];
		average_value = sum / data_size;
	}
	return average_value;
}

template float average<int>(const FinanceDataArrayTemplate<int>& finance_data_array);
template float average<long>(const FinanceDataArrayTemplate<long>& finance_data_array);
template float average<float>(const FinanceDataArrayTemplate<float>& finance_data_array);

template <typename T>
float variance(const FinanceDataArrayTemplate<T>& finance_data_array)
{
	float average_value = average(finance_data_array);
	float variance_value = 0.0;
	int data_size = finance_data_array.get_size();
	if (data_size != 0)
	{
		float sum = 0.0;
		for (int i = 0 ; i < data_size ; i++)
			sum += pow((finance_data_array[i] - average_value), 2);
		variance_value = sum / data_size;
	}
	return variance_value;
}

template float variance<int>(const FinanceDataArrayTemplate<int>& finance_data_array);
template float variance<long>(const FinanceDataArrayTemplate<long>& finance_data_array);
template float variance<float>(const FinanceDataArrayTemplate<float>& finance_data_array);

template <typename T>
float standard_deviation(const FinanceDataArrayTemplate<T>& finance_data_array){return sqrt(variance(finance_data_array));}

template float standard_deviation<int>(const FinanceDataArrayTemplate<int>& finance_data_array);
template float standard_deviation<long>(const FinanceDataArrayTemplate<long>& finance_data_array);
template float standard_deviation<float>(const FinanceDataArrayTemplate<float>& finance_data_array);

template <typename T1, typename T2>
float covariance(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2)
{
	assert(finance_data_array1.get_size() == finance_data_array2.get_size() && "The array sizes are NOT identical");
	float average_value1 = average(finance_data_array1);
	float average_value2 = average(finance_data_array2);
	float covariance_value = 0.0;
	int data_size = finance_data_array1.get_size();
	if (data_size != 0)
	{
		float sum = 0.0;
		for (int i = 0 ; i < data_size ; i++)
			sum += (finance_data_array1[i] - average_value1) * (finance_data_array2[i] - average_value2);
		covariance_value = sum / data_size;
	}
	return covariance_value;
}

template float covariance<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
template float covariance<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
template float covariance<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
template float covariance<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
template float covariance<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
template float covariance<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
template float covariance<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
template float covariance<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
template float covariance<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);

template <typename T1, typename T2>
float correlation(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2)
{
	return covariance(finance_data_array1, finance_data_array2) / sqrt(variance(finance_data_array1) * variance(finance_data_array2));
}

template float correlation<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
template float correlation<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
template float correlation<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
template float correlation<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
template float correlation<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
template float correlation<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
template float correlation<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
template float correlation<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
template float correlation<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);

float correlation(const PFINANCE_DATA_ARRAY_BASE finance_data_array1, PFINANCE_DATA_ARRAY_BASE finance_data_array2)
{
	#define GET_COMBINED_VALUE(type1, type2) (type1 * 256 + type2)
	static const int FinanceField_INT_INT = GET_COMBINED_VALUE(FinanceField_INT, FinanceField_INT);
	static const int FinanceField_INT_LONG = GET_COMBINED_VALUE(FinanceField_INT, FinanceField_LONG);
	static const int FinanceField_INT_FLOAT = GET_COMBINED_VALUE(FinanceField_INT, FinanceField_FLOAT);
	static const int FinanceField_LONG_INT = GET_COMBINED_VALUE(FinanceField_LONG, FinanceField_INT);
	static const int FinanceField_LONG_LONG = GET_COMBINED_VALUE(FinanceField_LONG, FinanceField_LONG);
	static const int FinanceField_LONG_FLOAT = GET_COMBINED_VALUE(FinanceField_LONG, FinanceField_FLOAT);
	static const int FinanceField_FLOAT_INT = GET_COMBINED_VALUE(FinanceField_FLOAT, FinanceField_INT);
	static const int FinanceField_FLOAT_LONG = GET_COMBINED_VALUE(FinanceField_FLOAT, FinanceField_LONG);
	static const int FinanceField_FLOAT_FLOAT = GET_COMBINED_VALUE(FinanceField_FLOAT, FinanceField_FLOAT);

	assert(finance_data_array1 != NULL && "finance_data_array1 should NOT be NULL");
	assert(finance_data_array2 != NULL && "finance_data_array2 should NOT be NULL");
//	FinanceFieldType field_type1 = finance_data_array1->get_type();
//	FinanceFieldType field_type2 = finance_data_array2->get_type();
	int combined_value = GET_COMBINED_VALUE(finance_data_array1->get_type(), finance_data_array2->get_type());

	float correlation_value;
	switch (combined_value)
	{
	case FinanceField_INT_INT:
		correlation_value = correlation(*(PFINANCE_INT_DATA_ARRAY)finance_data_array1, *(PFINANCE_INT_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_INT_LONG:
		correlation_value = correlation(*(PFINANCE_INT_DATA_ARRAY)finance_data_array1, *(PFINANCE_LONG_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_INT_FLOAT:
		correlation_value = correlation(*(PFINANCE_INT_DATA_ARRAY)finance_data_array1, *(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_LONG_INT:
		correlation_value = correlation(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array1, *(PFINANCE_INT_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_LONG_LONG:
		correlation_value = correlation(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array1, *(PFINANCE_LONG_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_LONG_FLOAT:
		correlation_value = correlation(*(PFINANCE_LONG_DATA_ARRAY)finance_data_array1, *(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_FLOAT_INT:
		correlation_value = correlation(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array1, *(PFINANCE_INT_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_FLOAT_LONG:
		correlation_value = correlation(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array1, *(PFINANCE_LONG_DATA_ARRAY)finance_data_array2);
		break;
	case FinanceField_FLOAT_FLOAT:
		correlation_value = correlation(*(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array1, *(PFINANCE_FLOAT_DATA_ARRAY)finance_data_array2);
		break;
	default:
	{
		char errmsg[64];
		snprintf(errmsg, 64, "Unknown combined value: %d from (%d, %d)", combined_value, finance_data_array1->get_type(), finance_data_array2->get_type());
		throw invalid_argument(errmsg);
	}
		break;
	}

	return correlation_value;
}
