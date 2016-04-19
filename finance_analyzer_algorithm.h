#ifndef FINANCE_ANALYZER_ALGORITHM_H
#define FINANCE_ANALYZER_ALGORITHM_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


#define DECLARE_SINGLE_INPUT_FORMULA_FUNCTION(X)\
float X(const PFINANCE_DATA_ARRAY_BASE finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index=0, int end_index=-1);\
float X(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int start_index=0, int end_index=-1);

#define DECLARE_DOUBLE_INPUT_FORMULA_FUNCTION(X)\
float X(const PFINANCE_DATA_ARRAY_BASE finance_data_array1, const PFINANCE_DATA_ARRAY_BASE finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);\
float X(const PFINANCE_DATA_ARRAY_BASE finance_data_array1, const PFINANCE_DATA_ARRAY_BASE finance_data_array2, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);


template <typename T>
T sum(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index=0, int end_index=-1);
template <typename T>
T sum(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index=0, int end_index=-1);

extern template int sum<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template long sum<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float sum<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);

extern template int sum<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
extern template long sum<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
extern template float sum<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

//float average(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int start_index=0, int end_index=-1);
DECLARE_SINGLE_INPUT_FORMULA_FUNCTION(sum)

template <typename T>
float average(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index=0, int end_index=-1);
template <typename T>
float average(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index=0, int end_index=-1);

extern template float average<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float average<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float average<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);

extern template float average<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
extern template float average<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
extern template float average<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

//float average(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int start_index=0, int end_index=-1);
DECLARE_SINGLE_INPUT_FORMULA_FUNCTION(average)


template <typename T>
float variance(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index=0, int end_index=-1);
template <typename T>
float variance(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index=0, int end_index=-1);

extern template float variance<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float variance<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float variance<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);

extern template float variance<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
extern template float variance<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
extern template float variance<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

//float variance(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int start_index=0, int end_index=-1);
DECLARE_SINGLE_INPUT_FORMULA_FUNCTION(variance)


template <typename T>
float standard_deviation(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index=0, int end_index=-1);
template <typename T>
float standard_deviation(const FinanceDataArrayTemplate<T>& finance_data_array, int start_index=0, int end_index=-1);

extern template float standard_deviation<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float standard_deviation<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);
extern template float standard_deviation<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index, int end_index);

extern template float standard_deviation<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int start_index, int end_index);
extern template float standard_deviation<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int start_index, int end_index);
extern template float standard_deviation<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int start_index, int end_index);

DECLARE_SINGLE_INPUT_FORMULA_FUNCTION(standard_deviation)


template <typename T1, typename T2>
float covariance(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);
template <typename T1, typename T2>
float covariance(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);

extern template float covariance<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);

extern template float covariance<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float covariance<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);

DECLARE_DOUBLE_INPUT_FORMULA_FUNCTION(covariance)


template <typename T1, typename T2>
float correlation(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);
template <typename T1, typename T2>
float correlation(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);

extern template float correlation<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, PFINANCE_BOOL_DATA_ARRAY filter_array, int start_index1, int start_index2, int end_index1, int end_index2);

extern template float correlation<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
extern template float correlation<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2, int start_index1, int start_index2, int end_index1, int end_index2);
//float correlation(const PFINANCE_DATA_ARRAY_BASE finance_data_array1, PFINANCE_DATA_ARRAY_BASE finance_data_array2, int start_index1=0, int start_index2=0, int end_index1=-1, int end_index2=-1);
DECLARE_DOUBLE_INPUT_FORMULA_FUNCTION(correlation)


template <typename T>
int binary_search_interval(const T* interval_array, T value, int left, int right);

// template <typename T>
// int binary_search_interval(const FinanceDataArrayTemplate<T>& finance_data_array, T value, int left, int right);

extern template int binary_search_interval<int>(const int* interval_array, int value, int left, int right);
extern template int binary_search_interval<long>(const long* interval_array, long value, int left, int right);
extern template int binary_search_interval<float>(const float* interval_array, float value, int left, int right);

// extern template int binary_search_interval<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int value, int left, int right);
// extern template int binary_search_interval<long>(const FinanceDataArrayTemplate<long>& finance_data_array, long value, int left, int right);
// extern template int binary_search_interval<float>(const FinanceDataArrayTemplate<float>& finance_data_array, float value, int left, int right);

template <typename T>
unsigned short get_data_range(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, T& data_min, T& data_max);
template <typename T>
unsigned short get_data_range(const FinanceDataArrayTemplate<T>& finance_data_array, T& data_min, T& data_max);

extern template unsigned short get_data_range<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int& data_min, int& data_max);
extern template unsigned short get_data_range<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, long& data_min, long& data_max);
extern template unsigned short get_data_range<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, float& data_min, float& data_max);

extern template unsigned short get_data_range<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int& data_min, int& data_max);
extern template unsigned short get_data_range<long>(const FinanceDataArrayTemplate<long>& finance_data_array, long& data_min, long& data_max);
extern template unsigned short get_data_range<float>(const FinanceDataArrayTemplate<float>& finance_data_array, float& data_min, float& data_max);


template <typename T>
unsigned short get_histogram_interval(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<T>& sp_histogram_interval);
template <typename T>
unsigned short get_histogram_interval(const FinanceDataArrayTemplate<T>& finance_data_array, int interval_amount, SmartPointer<T>& sp_histogram_interval);

template <typename T>
unsigned short get_histogram_interval(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<T>& data_histogram_interval);
template <typename T>
unsigned short get_histogram_interval(const FinanceDataArrayTemplate<T>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<T>& data_histogram_interval);

extern template unsigned short get_histogram_interval<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<int> &sp_histogram_interval);
extern template unsigned short get_histogram_interval<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<long> &sp_histogram_interval);
extern template unsigned short get_histogram_interval<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<float> &sp_histogram_interval);

extern template unsigned short get_histogram_interval<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int interval_amount, SmartPointer<int> &sp_histogram_interval);
extern template unsigned short get_histogram_interval<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int interval_amount, SmartPointer<long> &sp_histogram_interval);
extern template unsigned short get_histogram_interval<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int interval_amount, SmartPointer<float> &sp_histogram_interval);

extern template unsigned short get_histogram_interval<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_interval);
extern template unsigned short get_histogram_interval<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<long>& data_histogram_interval);
extern template unsigned short get_histogram_interval<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<float>& data_histogram_interval);

extern template unsigned short get_histogram_interval<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_interval);
extern template unsigned short get_histogram_interval<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<long>& data_histogram_interval);
extern template unsigned short get_histogram_interval<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<float>& data_histogram_interval);


template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const T* histogram_interval, SmartPointer<int> &sp_histogram_statistics);
template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, int interval_amount, const T* histogram_interval, SmartPointer<int> &sp_histogram_statistics);

template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const T* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);
template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, int interval_amount, const T* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const int* histogram_interval, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const long* histogram_interval, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const float* histogram_interval, SmartPointer<int> &sp_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int interval_amount, const int* histogram_interval, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int interval_amount, const long* histogram_interval, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int interval_amount, const float* histogram_interval, SmartPointer<int> &sp_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const int* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const long* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const float* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int interval_amount, const int* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int interval_amount, const long* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int interval_amount, const float* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);

unsigned short get_histogram(const PFINANCE_DATA_ARRAY_BASE finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, const void* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);
unsigned short get_histogram(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int interval_amount, const void* histogram_interval, FinanceDataArrayTemplate<int>& data_histogram_statistics);

template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);
template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);

template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);
template <typename T>
unsigned short get_histogram(const FinanceDataArrayTemplate<T>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int interval_amount, SmartPointer<int> &sp_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);

extern template unsigned short get_histogram<int>(const FinanceDataArrayTemplate<int>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<long>(const FinanceDataArrayTemplate<long>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);
extern template unsigned short get_histogram<float>(const FinanceDataArrayTemplate<float>& finance_data_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);

unsigned short get_histogram(const PFINANCE_DATA_ARRAY_BASE finance_data_array, PFINANCE_BOOL_DATA_ARRAY filter_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);
unsigned short get_histogram(const PFINANCE_DATA_ARRAY_BASE finance_data_array, int interval_amount, FinanceDataArrayTemplate<int>& data_histogram_statistics);

#endif
