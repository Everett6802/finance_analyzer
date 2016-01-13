#ifndef FINANCE_ANALYZER_ALGORITHM_H
#define FINANCE_ANALYZER_ALGORITHM_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


template <typename T>
float average(const FinanceDataArrayBase<T>& finance_data_array);

extern template float average<int>(const FinanceDataArrayBase<int>& finance_data_array);
extern template float average<long>(const FinanceDataArrayBase<long>& finance_data_array);
extern template float average<float>(const FinanceDataArrayBase<float>& finance_data_array);


template <typename T>
float variance(const FinanceDataArrayBase<T>& finance_data_array);

extern template float variance<int>(const FinanceDataArrayBase<int>& finance_data_array);
extern template float variance<long>(const FinanceDataArrayBase<long>& finance_data_array);
extern template float variance<float>(const FinanceDataArrayBase<float>& finance_data_array);


template <typename T>
float standard_deviation(const FinanceDataArrayBase<T>& finance_data_array);

extern template float standard_deviation<int>(const FinanceDataArrayBase<int>& finance_data_array);
extern template float standard_deviation<long>(const FinanceDataArrayBase<long>& finance_data_array);
extern template float standard_deviation<float>(const FinanceDataArrayBase<float>& finance_data_array);


template <typename T1, typename T2>
float covariance(const FinanceDataArrayBase<T1>& finance_data_array1, const FinanceDataArrayBase<T2>& finance_data_array2);

extern template float covariance<int, int>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
extern template float covariance<int, long>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
extern template float covariance<int, float>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
extern template float covariance<long, int>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
extern template float covariance<long, long>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
extern template float covariance<long, float>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
extern template float covariance<float, int>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
extern template float covariance<float, long>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
extern template float covariance<float, float>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);


template <typename T1, typename T2>
float correlation(const FinanceDataArrayBase<T1>& finance_data_array1, const FinanceDataArrayBase<T2>& finance_data_array2);

extern template float correlation<int, int>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
extern template float correlation<int, long>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
extern template float correlation<int, float>(const FinanceDataArrayBase<int>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
extern template float correlation<long, int>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
extern template float correlation<long, long>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
extern template float correlation<long, float>(const FinanceDataArrayBase<long>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);
extern template float correlation<float, int>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<int>& finance_data_array2);
extern template float correlation<float, long>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<long>& finance_data_array2);
extern template float correlation<float, float>(const FinanceDataArrayBase<float>& finance_data_array1, const FinanceDataArrayBase<float>& finance_data_array2);


#endif
