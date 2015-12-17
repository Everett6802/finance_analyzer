#ifndef FINANCE_ANALYZER_COMMON_CLASS_H
#define FINANCE_ANALYZER_COMMON_CLASS_H

#include <deque>
#include <map>
#include "msg_dumper_wrapper.h"


class TimeCfg
{
	DECLARE_MSG_DUMPER()
	enum TimeType{TIME_MONTH, TIME_DATE};
private:
	TimeType time_type;
	int year;
	int month;
	int day;
	char time_str[16];

public:
	static int get_int_value(int year, int month, int day);
	static int get_int_value(const TimeCfg* time_cfg);

	TimeCfg(const char* cur_time_str); // Format: "2015-09" or "2015-09-04"
	TimeCfg(int cur_year, int cur_month);
	TimeCfg(int cur_year, int cur_month, int cur_day);
	~TimeCfg();
	TimeCfg& operator=(const TimeCfg& another);

	int get_year()const;
	int get_month()const;
	int get_day()const;
	const char* to_string()const;
	bool is_month_type()const;
	bool equal_to(const TimeCfg* another_time_cfg)const;

	bool operator<(const TimeCfg& another_time_cfg)const;
	bool operator>(const TimeCfg& another_time_cfg)const;
	bool operator==(const TimeCfg& another_time_cfg)const;
	bool operator>=(const TimeCfg& another_time_cfg)const;
	bool operator<=(const TimeCfg& another_time_cfg)const;
	bool operator!=(const TimeCfg& another_time_cfg)const;
};
typedef TimeCfg* PTIME_CFG;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeRangeCfg
{
	DECLARE_MSG_DUMPER()
protected:
	PTIME_CFG time_start_cfg;
	PTIME_CFG time_end_cfg;
	char* time_range_description;
	bool type_is_month;

public:
	static bool time_in_range(const TimeRangeCfg* time_range_cfg, const TimeCfg* time_cfg);
	static bool time_in_range(const TimeRangeCfg* time_range_cfg, int year, int month, int day);

	TimeRangeCfg(const char* time_start_str, const char* time_end_str);
	TimeRangeCfg(int year_start, int month_start, int year_end, int month_end);
	TimeRangeCfg(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end);

	~TimeRangeCfg();
	bool is_month_type()const{return type_is_month;}
	bool is_single_time()const;
	const char* to_string();
	const PTIME_CFG get_start_time()const;
	const PTIME_CFG get_end_time()const;
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

template <typename T>
class FinanceDataArrayBase
{
	DECLARE_MSG_DUMPER()
protected:
	static int DEF_ARRAY_SIZE;

	T* array_data;
	int array_size;
	int array_pos;

	void alloc_new();

public:
	FinanceDataArrayBase();
	virtual ~FinanceDataArrayBase();

	bool is_empty()const;
	int get_size()const;
	int get_array_size()const;
	const T* get_data_array()const;
	const T operator[](int index)const;

	void add(T data);
};

template <typename T>
class FinanceDataPtrArrayBase : public FinanceDataArrayBase<T>
{
public:
	virtual ~FinanceDataPtrArrayBase();

	void add(T data, size_t data_size);
};

typedef FinanceDataArrayBase<int> FinanceIntDataArray;
typedef FinanceIntDataArray* PFINANCE_INT_DATA_ARRAY;

typedef FinanceDataArrayBase<long> FinanceLongDataArray;
typedef FinanceLongDataArray* PFINANCE_LONG_DATA_ARRAY;

typedef FinanceDataArrayBase<float> FinanceFloatDataArray;
typedef FinanceFloatDataArray* PFINANCE_FLOAT_DATA_ARRAY;

typedef FinanceDataPtrArrayBase<char*> FinanceCharDataPtrArray;
typedef FinanceCharDataPtrArray* PFINANCE_CHAR_DATA_PTR_ARRAY;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::deque<int> DEQUE_INT;
typedef DEQUE_INT* PDEQUE_INT;

#define ADD_QUERY(QuerySet, SourceIndex, FieldIndex)\
do{\
unsigned short ret = QuerySet.add_query(SourceIndex, FieldIndex);\
if (CHECK_FAILURE(ret))\
	return ret;\
}while(0)

class QuerySet
{
	DECLARE_MSG_DUMPER()
private:
	DEQUE_INT query_set[FinanceSourceSize];
	bool add_done;

public:
	QuerySet();
	~QuerySet();

	unsigned short add_query(int source_index, int field_index=-1);
	unsigned short add_query_done();
	bool is_add_query_done()const;

	class iterator
	{
	private:
		PDEQUE_INT ptr_;
		PDEQUE_INT ptr_end_;
//		void get_next()
//		{
//			while(true)
//			{
//				ptr_++;
//				if (ptr_ != ptr_end_ && !ptr_->empty()) break;
//			}
//		}

	public:
		iterator(const PDEQUE_INT ptr) : ptr_(ptr), ptr_end_(ptr + FinanceSourceSize) {}
		iterator operator++()
		{
			ptr_++;
//			get_next();
			return *this;
		}
		iterator operator++(int junk)
		{
			iterator i = *this;
			ptr_++;
//			get_next();
			return i;
		}
		const DEQUE_INT& operator*() { return *ptr_; }
		const PDEQUE_INT operator->() { return ptr_; }
	};

	iterator begin() { return iterator(query_set); }
	iterator end() { return iterator(query_set + FinanceSourceSize); }

	const DEQUE_INT& operator[](int index)const;
};
typedef QuerySet* PQUERY_SET;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ResultSet
{
	DECLARE_MSG_DUMPER()

	static unsigned short get_combined_index(int x, int y);
	static unsigned short get_upper_subindex(unsigned short x);
	static unsigned short get_lower_subindex(unsigned short x);

private:
	std::map<unsigned short, unsigned short> data_set_mapping;
	FinanceCharDataPtrArray date_data;
	std::deque<PFINANCE_INT_DATA_ARRAY> int_data_set;
	std::deque<PFINANCE_LONG_DATA_ARRAY> long_data_set;
	std::deque<PFINANCE_FLOAT_DATA_ARRAY> float_data_set;
	bool check_date_data_mode;
//	int date_data_size;
	int date_date_pos;
	int int_data_set_size;
	int long_data_set_size;
	int float_data_set_size;

public:
	ResultSet();
	~ResultSet();

	unsigned short add_set(int source_index, int field_index);
	unsigned short add_set(int source_index, const DEQUE_INT& field_set);
	unsigned short set_date(char* element_value);
	unsigned short set_data(int source_index, int field_index, char* data_string);

	void switch_to_check_date_mode();
	unsigned short check_data()const;
	unsigned short show_data()const;

#define DECLARE_GET_ARRAY_FUNC(n, m) const PFINANCE_##m##_DATA_ARRAY get_##n##_array(int source_index, int field_index)const;
	DECLARE_GET_ARRAY_FUNC(int, INT)
	DECLARE_GET_ARRAY_FUNC(long, LONG)
	DECLARE_GET_ARRAY_FUNC(float, FLOAT)
#define DECLARE_GET_ARRAY_ELEMENT_FUNC(n) n get_##n##_array_element(int source_index, int field_index, int index)const;
	DECLARE_GET_ARRAY_ELEMENT_FUNC(int)
	DECLARE_GET_ARRAY_ELEMENT_FUNC(long)
	DECLARE_GET_ARRAY_ELEMENT_FUNC(float)
};
typedef ResultSet* PRESULT_SET;

#endif
