#ifndef COMMON_CLASS_BASE_H
#define COMMON_CLASS_BASE_H

#include "msg_dumper_wrapper.h"
#include "common_definition.h"
#include "common_function.h"


// class DataSetAccessParam;
// class ResultSetAccessParam;
// class FilterRuleThresholdBase;

// typedef std::deque<DataSetAccessParam*> DataSetAccessParamDeque;
// typedef DataSetAccessParamDeque* PDATA_SET_ACCESS_PARAM_DEQUE;
// typedef std::deque<ResultSetAccessParam*> ResultSetAccessParamDeque;
// typedef ResultSetAccessParamDeque* PRESULT_SET_ACCESS_PARAM_DEQUE;
// typedef std::deque<FilterRuleThresholdBase*> FilterRuleThresholdDeque;
// typedef FilterRuleThresholdDeque* PFILTER_RULE_THRESHOLD_DEQUE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class

// template <typename T>
// class SmartPointer
// {
// private:
// 	bool need_release;
// 	T* data_ptr;

// public:
// 	SmartPointer();
// 	SmartPointer(T* ptr);
// 	~SmartPointer();
// 	T& operator*();
// 	const T& operator*() const;
// 	T* operator->();
// 	const T* operator->() const;
// 	void set_new(T* ptr);
// 	T* get_instance();
// 	const T* get_const_instance()const;
// 	void disable_release();
// };
//typedef SmartPointer SP;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface

class IEventNotify
{
public:
	virtual unsigned short notify(int event_type, void* event_data)=0;
};
typedef IEventNotify* PIEVENT_NOTIFY;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class

class TimeCfg
{
	DECLARE_MSG_DUMPER()
	enum TimeType{TIME_MONTH, TIME_DATE};
private:
	static void parse_time_string(const char* time_str, int &year, int &month, int& day, TimeType& time_type);

	TimeType time_type;
	int year;
	int month;
	int day;
	char time_str[16];

public:
	static int get_int_value(int year, int month, int day);
	static int get_int_value(const TimeCfg* time_cfg);
	static int get_int_value(const char* time_str); // Format: "2015-09" or "2015-09-04"

	TimeCfg(const char* cur_time_str); // Format: "2015-09" or "2015-09-04"
	TimeCfg(int cur_year, int cur_month);
	TimeCfg(int cur_year, int cur_month, int cur_day);
	TimeCfg(const TimeCfg* another_time_cfg);
	TimeCfg& operator=(const TimeCfg& another);
	~TimeCfg();

	int get_year()const;
	int get_month()const;
	int get_day()const;
	const char* to_string()const;
	bool is_month_type()const;
	bool equal_to(const TimeCfg* another_time_cfg)const;

	void reset(const char* new_time_str); // Format: "2015-09" or "2015-09-04"
	void reset(int new_year, int new_month);
	void reset(int new_year, int new_month, int cur_day);
	void reset(const TimeCfg* new_time_cfg);

	bool operator<(const TimeCfg& another_time_cfg)const;
	bool operator>(const TimeCfg& another_time_cfg)const;
	bool operator==(const TimeCfg& another_time_cfg)const;
	bool operator>=(const TimeCfg& another_time_cfg)const;
	bool operator<=(const TimeCfg& another_time_cfg)const;
	bool operator!=(const TimeCfg& another_time_cfg)const;
	bool operator<(const char* another_time_str)const;
	bool operator>(const char* another_time_str)const;
	bool operator==(const char* another_time_str)const;
	bool operator>=(const char* another_time_str)const;
	bool operator<=(const char* another_time_str)const;
	bool operator!=(const char* another_time_str)const;
};
typedef TimeCfg* PTIME_CFG;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeRangeCfg
{
private:
	DECLARE_MSG_DUMPER()

public:
	static bool time_in_range(const TimeRangeCfg* time_range_cfg, const TimeCfg* time_cfg);
	static bool time_in_range(const TimeRangeCfg* time_range_cfg, int year, int month, int day);
	static unsigned short create_instance_from_string(const char* source_string, TimeRangeCfg& time_range_cfg);
	static unsigned short create_instance_from_string(const char* source_string, TimeRangeCfg** time_range_cfg);

protected:
	PTIME_CFG time_start_cfg;
	PTIME_CFG time_end_cfg;
	char* time_range_description;
	bool type_is_month;
	bool add_done;

public:
	TimeRangeCfg();
	TimeRangeCfg(const char* time_start_str, const char* time_end_str);
	TimeRangeCfg(int year_start, int month_start, int year_end, int month_end);
	TimeRangeCfg(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end);
	TimeRangeCfg(const TimeRangeCfg& another_time_range_cfg);
	~TimeRangeCfg();

	void add_start_time(const PTIME_CFG new_start_time_cfg);
	void add_end_time(const PTIME_CFG new_end_time_cfg);
	void add_time(const PTIME_CFG new_start_time_cfg, const PTIME_CFG new_end_time_cfg);
	void add_start_time(const char* new_start_time_str);
	void add_end_time(const char* new_end_time_str);
	void add_time(const char* new_start_time_str, const char* new_end_time_str);
	unsigned short add_time_done();

	void reset_start_time(const PTIME_CFG new_start_time_cfg);
	void reset_end_time(const PTIME_CFG new_end_time_cfg);
	void reset_time(const PTIME_CFG new_start_time_cfg, const PTIME_CFG new_end_time_cfg);
	void reset_start_time(const char* new_start_time_str);
	void reset_end_time(const char* new_end_time_str);
	void reset_time(const char* new_start_time_str, const char* new_end_time_str);

	bool is_month_type()const;
	bool is_single_time()const;
	bool is_add_time_done()const;
	const char* to_string();
	const PTIME_CFG get_start_time()const;
	const PTIME_CFG get_end_time()const;
// Don't modify the start/end time in the following way
	// PTIME_CFG get_start_time();
	// PTIME_CFG get_end_time();
};
typedef TimeRangeCfg* PTIME_RANGE_CFG;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SingleTimeRangeCfg : public TimeRangeCfg
{
public:
	SingleTimeRangeCfg(const char* time_str); // Single day
	SingleTimeRangeCfg(int year, int month); // Single month
	SingleTimeRangeCfg(int year, int month, int day); // Single day
};
typedef SingleTimeRangeCfg* PSINGLE_TIME_RANGE_CFG;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FinanceDataArrayBase
{
protected:
	DECLARE_MSG_DUMPER()
	static int DEF_ARRAY_SIZE;
	FinanceFieldType array_type;
	int array_size;
	int array_pos;

public:
	FinanceDataArrayBase();
	virtual ~FinanceDataArrayBase();

	void set_type(FinanceFieldType type);
	FinanceFieldType get_type()const;
	bool is_empty()const;
	int get_size()const;
	int get_array_size()const;
	void reset_array();
};
typedef FinanceDataArrayBase* PFINANCE_DATA_ARRAY_BASE;

template <typename T>
class FinanceDataArrayTemplate : public FinanceDataArrayBase
{
private:
// Since the memory can only be calcuated in FinanceDataArrayTemplate, not in FinanceDataPtrArrayTemplate.
// The memory restriction is NOT implemented in FinanceDataPtrArrayTemplate
	static const int MAX_MEMORY_SIZE = 2 * 1024 * 1024; // 2M
	// bool over_max_memroy_size;
protected:
	T* array_data;

	void alloc_new(int new_array_size);

public:
	FinanceDataArrayTemplate();
	FinanceDataArrayTemplate(const FinanceDataArrayTemplate& another);
	~FinanceDataArrayTemplate();

	// bool is_over_max_memroy_size()const;
	void set_data_array(const T* array, int size); // Set the data from 0 postion of the array
	const T* get_data_array()const;
	const T operator[](int index)const;
//	unsigned short get_sub_array(FinanceDataArrayTemplate& new_data_array, int start_index, int end_index=-1);
//	unsigned short get_diff_array(FinanceDataArrayTemplate& new_data_array, int start_index, int end_index=-1);

// Caution: It's illegal to override the operator function in the template
//	FinanceDataArrayTemplate<T>& operator=(const FinanceDataArrayTemplate& another);
//	FinanceDataArrayTemplate<T>& operator+=(const FinanceDataArrayTemplate& another);
//	FinanceDataArrayTemplate<T> operator+(const FinanceDataArrayTemplate& another);

	void add_data(T data); // Add the data after last postion of the array
	void add_data_array(const T* data, int size); // Add the data after last postion of the array
	// unsigned short get_data_range(T& data_min, T& data_max)const;
	// unsigned short get_histogram_interval(int interval_amount, SmartPointer<T> &sp_histogram_interval)const;
	// unsigned short get_histogram(int interval_amount, const T* histogram_interval, SmartPointer<int> &sp_histogram_statistics)const;
	// unsigned short get_histogram(int interval_amount, SmartPointer<int> &sp_histogram_statistics)const;
};

template <typename T>
class FinanceDataPtrArrayTemplate : public FinanceDataArrayBase
{
protected:
	T** array_data;

	void alloc_new(int new_array_size);
public:
	FinanceDataPtrArrayTemplate();
	FinanceDataPtrArrayTemplate(const FinanceDataPtrArrayTemplate& another);
	virtual ~FinanceDataPtrArrayTemplate();

	// void set_data_array(const T** array, int size);
	// const T** get_data_array()const;
	const T* operator[](int index)const;
	void add_data(const T* data, size_t data_size); // Add the data after last postion of the array
	void add_data_array(const T** data, size_t data_size, int size); // Add the data after last postion of the array
};

class FinanceCharDataArray;
class FinanceIntDataArray;
class FinanceLongDataArray;
class FinanceFloatDataArray;
class FinanceBoolDataArray;

#define DECLARE_DATA_ARRAY_OPERATOR_EQUAL(m, n)\
bool operator==(const n* another_array);\
bool operator==(const Finance##m##DataArray& another);\
bool operator!=(const n* another_array);\
bool operator!=(const Finance##m##DataArray& another);

#define DECLARE_DATA_ARRAY_OPERATOR(m, n)\
Finance##m##DataArray& operator=(const Finance##m##DataArray& another);\
Finance##m##DataArray& operator+=(const Finance##m##DataArray& another);\
Finance##m##DataArray operator+(const Finance##m##DataArray& another);\
Finance##m##DataArray& operator-=(const Finance##m##DataArray& another);\
Finance##m##DataArray operator-(const Finance##m##DataArray& another);\
DECLARE_DATA_ARRAY_OPERATOR_EQUAL(m, n)

#define DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(m)\
unsigned short get_sub_array(Finance##m##DataArray& new_data_array, const FinanceBoolDataArray* filter_array, int start_index, int end_index=-1);\
unsigned short get_sub_array(Finance##m##DataArray& new_data_array, int start_index, int end_index=-1);\
unsigned short get_diff_array(Finance##m##DataArray& new_data_array, int start_index, int end_index=-1);\
unsigned short get_rate_array(FinanceFloatDataArray& new_data_array, int start_index, int end_index=-1);\
unsigned short get_sum_array(Finance##m##DataArray& new_data_array, int N, int start_index, int end_index=-1);\
unsigned short get_avg_array(FinanceFloatDataArray& new_data_array, int N, int start_index, int end_index=-1);\
unsigned short get_weighted_avg_array(FinanceFloatDataArray& new_data_array, int N, int start_index, int end_index=-1);

#define DECLARE_DATA_ARRAY_OSTREAM(m, n)\
std::ostream& operator<<(std::ostream &out, const Finance##m##DataArray &finance_##n##_data_array);

#define DECLARE_DATA_PTR_ARRAY_OSTREAM(m, n)\
std::ostream& operator<<(std::ostream &out, const Finance##m##DataPtrArray &finance_##n##_data_array);

//typedef FinanceDataArrayTemplate<int> FinanceIntDataArray;
class FinanceIntDataArray : public FinanceDataArrayTemplate<int>
{
public:
	DECLARE_DATA_ARRAY_OPERATOR(Int, int)
	DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(Int)
};
typedef FinanceIntDataArray* PFINANCE_INT_DATA_ARRAY;
DECLARE_DATA_ARRAY_OSTREAM(Int, int)

//typedef FinanceDataArrayTemplate<long> FinanceLongDataArray;
class FinanceLongDataArray : public FinanceDataArrayTemplate<long>
{
public:
	DECLARE_DATA_ARRAY_OPERATOR(Long, long)
	DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(Long)
};
typedef FinanceLongDataArray* PFINANCE_LONG_DATA_ARRAY;
DECLARE_DATA_ARRAY_OSTREAM(Long, long)

//typedef FinanceDataArrayTemplate<float> FinanceFloatDataArray;
class FinanceFloatDataArray : public FinanceDataArrayTemplate<float>
{
public:
	DECLARE_DATA_ARRAY_OPERATOR(Float, float)
	DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(Float)
};
typedef FinanceFloatDataArray* PFINANCE_FLOAT_DATA_ARRAY;
DECLARE_DATA_ARRAY_OSTREAM(Float, float)

//std::ostream& operator<<(std::ostream &out, const FinanceDataArrayBase &finance_data_array);

class FinanceBoolDataArray : public FinanceDataArrayTemplate<bool>
{
private:
	int true_cnt;
	// unsigned short get_data_range(bool& data_min, bool& data_max)const{}
	// unsigned short get_histogram_interval(int interval_amount, SmartPointer<bool> &sp_histogram_interval)const{}
	// unsigned short get_histogram(int interval_amount, const bool* histogram_interval, SmartPointer<int> &sp_histogram_statistics)const{}
	// unsigned short get_histogram(int interval_amount, SmartPointer<int> &sp_histogram_statistics)const{}

public:
	FinanceBoolDataArray();

	DECLARE_DATA_ARRAY_OPERATOR_EQUAL(Bool, bool)
	void reset_array();
	void add_data(bool data);
	void add_data_array(bool* data, int size);
	int get_true_cnt()const;
	float get_probability()const;
};
typedef FinanceBoolDataArray* PFINANCE_BOOL_DATA_ARRAY;
DECLARE_DATA_ARRAY_OSTREAM(Bool, bool)

// std::ostream& operator<<(std::ostream &out, const FinanceBoolDataArray &finance_data_array);

// typedef FinanceDataPtrArrayTemplate<char> FinanceCharDataPtrArray;
class FinanceCharDataPtrArray : public FinanceDataPtrArrayTemplate<char>
{
};
typedef FinanceCharDataPtrArray* PFINANCE_CHAR_DATA_PTR_ARRAY;
DECLARE_DATA_PTR_ARRAY_OSTREAM(Char, char)

// std::ostream& operator<<(std::ostream &out, const Finance##m##DataArray &finance_##n##_data_array);


#endif
