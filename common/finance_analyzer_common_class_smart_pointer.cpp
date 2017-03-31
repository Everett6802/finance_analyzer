#include "finance_analyzer_common_class_smart_pointer.h"


using namespace std;

template <typename T>
SmartPointer<T>::SmartPointer() :
	need_release(true)
{
	data_ptr = NULL;
}

template <typename T>
SmartPointer<T>::SmartPointer(T* ptr) :
	need_release(true)
{
	data_ptr = ptr;
}

template <typename T>
SmartPointer<T>::~SmartPointer()
{
	if (need_release && data_ptr != NULL)
	{
		delete data_ptr;
		data_ptr = NULL;
	}
}

template <typename T>
T& SmartPointer<T>::operator*()
{
	return *data_ptr;
}

template <typename T>
const T& SmartPointer<T>::operator*() const
{
	return *data_ptr;
}

template <typename T>
T* SmartPointer<T>::operator->()
{
	return data_ptr;
}

template <typename T>
const T* SmartPointer<T>::operator->() const
{
	return data_ptr;
}

template <typename T>
void SmartPointer<T>::set_new(T* ptr)
{
	if (data_ptr != NULL)
	{
		delete data_ptr;
		data_ptr = NULL;
	}
	data_ptr = ptr;
}

template <typename T>
T* SmartPointer<T>::get_instance()
{
	return data_ptr;
}

template <typename T>
const T* SmartPointer<T>::get_const_instance()const
{
	return (const T*)data_ptr;
}

template <typename T>
void SmartPointer<T>::disable_release(){need_release = false;}

template class SmartPointer<int>;
template class SmartPointer<long>;
template class SmartPointer<float>;
template class SmartPointer<TimeCfg>;
template class SmartPointer<TimeRangeCfg>;
template class SmartPointer<QuerySet>;
template class SmartPointer<ResultSet>;
template class SmartPointer<ResultSetAccessParam>;
template class SmartPointer<ResultSetMap>;
template class SmartPointer<SearchRuleSet>;
template class SmartPointer<FinanceIntDataArray>;
template class SmartPointer<FinanceLongDataArray>;
template class SmartPointer<FinanceFloatDataArray>;
