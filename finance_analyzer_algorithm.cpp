#include <math.h>
#include <assert.h>
#include "finance_analyzer_algorithm.h"


template <typename T>
float average(const FinanceDataArrayBase<T>& finance_data_array)
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

template float average<int>(const FinanceDataArrayBase<int>& finance_data_array);
template float average<long>(const FinanceDataArrayBase<long>& finance_data_array);
template float average<float>(const FinanceDataArrayBase<float>& finance_data_array);

template <typename T>
float variance(const FinanceDataArrayBase<T>& finance_data_array)
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

template float variance<int>(const FinanceDataArrayBase<int>& finance_data_array);
template float variance<long>(const FinanceDataArrayBase<long>& finance_data_array);
template float variance<float>(const FinanceDataArrayBase<float>& finance_data_array);

template <typename T>
float standard_deviation(const FinanceDataArrayBase<T>& finance_data_array){return sqrt(variance(finance_data_array));}

template float standard_deviation<int>(const FinanceDataArrayBase<int>& finance_data_array);
template float standard_deviation<long>(const FinanceDataArrayBase<long>& finance_data_array);
template float standard_deviation<float>(const FinanceDataArrayBase<float>& finance_data_array);

template <typename T1, typename T2>
float covariance(const FinanceDataArrayBase<T1>& finance_data_array1, const FinanceDataArrayBase<T2>& finance_data_array2)
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

template float covariance<int, int>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
template float covariance<int, long>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
template float covariance<int, float>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
template float covariance<long, int>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
template float covariance<long, long>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
template float covariance<long, float>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
template float covariance<float, int>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
template float covariance<float, long>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
template float covariance<float, float>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);

template <typename T1, typename T2>
float correlation(const FinanceDataArrayBase<T1>& finance_data_array1, const FinanceDataArrayBase<T2>& finance_data_array2)
{
	return covariance(finance_data_array1, finance_data_array2) / sqrt(variance(finance_data_array1) * variance(finance_data_array2));
}

template float correlation<int, int>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
template float correlation<int, long>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
template float correlation<int, float>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
template float correlation<long, int>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
template float correlation<long, long>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
template float correlation<long, float>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
template float correlation<float, int>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
template float correlation<float, long>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
template float correlation<float, float>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
