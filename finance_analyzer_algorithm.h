#ifndef FINANCE_ANALYZER_ALGORITHM_H
#define FINANCE_ANALYZER_ALGORITHM_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


template <typename T>
float average(const FinanceDataArrayTemplate<T>& finance_data_array);

extern template float average<int>(const FinanceDataArrayTemplate<int>& finance_data_array);
extern template float average<long>(const FinanceDataArrayTemplate<long>& finance_data_array);
extern template float average<float>(const FinanceDataArrayTemplate<float>& finance_data_array);


template <typename T>
float variance(const FinanceDataArrayTemplate<T>& finance_data_array);

extern template float variance<int>(const FinanceDataArrayTemplate<int>& finance_data_array);
extern template float variance<long>(const FinanceDataArrayTemplate<long>& finance_data_array);
extern template float variance<float>(const FinanceDataArrayTemplate<float>& finance_data_array);


template <typename T>
float standard_deviation(const FinanceDataArrayTemplate<T>& finance_data_array);

extern template float standard_deviation<int>(const FinanceDataArrayTemplate<int>& finance_data_array);
extern template float standard_deviation<long>(const FinanceDataArrayTemplate<long>& finance_data_array);
extern template float standard_deviation<float>(const FinanceDataArrayTemplate<float>& finance_data_array);


template <typename T1, typename T2>
float covariance(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2);

extern template float covariance<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
extern template float covariance<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
extern template float covariance<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
extern template float covariance<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
extern template float covariance<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
extern template float covariance<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
extern template float covariance<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
extern template float covariance<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
extern template float covariance<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);


template <typename T1, typename T2>
float correlation(const FinanceDataArrayTemplate<T1>& finance_data_array1, const FinanceDataArrayTemplate<T2>& finance_data_array2);

extern template float correlation<int, int>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
extern template float correlation<int, long>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
extern template float correlation<int, float>(const FinanceDataArrayTemplate<int>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
extern template float correlation<long, int>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
extern template float correlation<long, long>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
extern template float correlation<long, float>(const FinanceDataArrayTemplate<long>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);
extern template float correlation<float, int>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<int>& finance_data_array2);
extern template float correlation<float, long>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<long>& finance_data_array2);
extern template float correlation<float, float>(const FinanceDataArrayTemplate<float>& finance_data_array1, const FinanceDataArrayTemplate<float>& finance_data_array2);

float correlation(const PFINANCE_DATA_ARRAY_BASE finance_data_array1, PFINANCE_DATA_ARRAY_BASE finance_data_array2);

#endif
