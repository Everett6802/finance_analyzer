#ifndef FINANCE_ANALYZER_COMMON_CLASS_H
#define FINANCE_ANALYZER_COMMON_CLASS_H

#include <deque>
#include <map>
#include "msg_dumper_wrapper.h"


template <typename T>
class SmartPointer
{
private:
	bool need_release;
	T* data_ptr;

public:
	SmartPointer();
	SmartPointer(T* ptr);
	~SmartPointer();
	T& operator*();
	const T& operator*() const;
	T* operator->();
	const T* operator->() const;
	void set_new(T* ptr);
	T* get_instance();
	void disable_release();
};
//typedef SmartPointer SP;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeCfg
{
	DECLARE_MSG_DUMPER()
	enum TimeType{TIME_MONTH, TIME_DATE};
private:
	static void parse_time_string(const char* time_str, int&year, int&month, int& day, TimeType& time_type);

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
	TimeRangeCfg(const TimeRangeCfg& another_time_range_cfg);
	~TimeRangeCfg();

	bool is_month_type()const{return type_is_month;}
	bool is_single_time()const;
	const char* to_string();
	const PTIME_CFG get_start_time()const;
	const PTIME_CFG get_end_time()const;
	PTIME_CFG get_start_time();
	PTIME_CFG get_end_time();
	void reset_time(const char* new_time_start_str, const char* new_time_end_str);
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
	FinanceFieldType array_type;

public:
	FinanceDataArrayBase();
	~FinanceDataArrayBase();

	void set_type(FinanceFieldType type);
	FinanceFieldType get_type()const;
};
typedef FinanceDataArrayBase* PFINANCE_DATA_ARRAY_BASE;

template <typename T>
class FinanceDataArrayTemplate : public FinanceDataArrayBase
{
//	DECLARE_MSG_DUMPER()
protected:
	static int DEF_ARRAY_SIZE;

	T* array_data;
	int array_size;
	int array_pos;

	void alloc_new();

public:
	FinanceDataArrayTemplate();
	FinanceDataArrayTemplate(const FinanceDataArrayTemplate& another);
	virtual ~FinanceDataArrayTemplate();

	bool is_empty()const;
	int get_size()const;
	int get_array_size()const;
	const T* get_data_array()const;
	const T operator[](int index)const;
//	unsigned short get_sub_array(FinanceDataArrayTemplate& new_data_array, int start_index, int end_index=-1);
//	unsigned short get_diff_array(FinanceDataArrayTemplate& new_data_array, int start_index, int end_index=-1);

// Caution: It's illegal to override the operator function in the template
//	FinanceDataArrayTemplate<T>& operator=(const FinanceDataArrayTemplate& another);
//	FinanceDataArrayTemplate<T>& operator+=(const FinanceDataArrayTemplate& another);
//	FinanceDataArrayTemplate<T> operator+(const FinanceDataArrayTemplate& another);

	void add(T data);
};

template <typename T>
class FinanceDataPtrArrayTemplate : public FinanceDataArrayTemplate<T>
{
public:
	virtual ~FinanceDataPtrArrayTemplate();

	void add(T data, size_t data_size);
};

#define DECLARE_DATA_ARRAY_OPERATOR(m)\
Finance##m##DataArray& operator=(const Finance##m##DataArray& another);\
Finance##m##DataArray& operator+=(const Finance##m##DataArray& another);\
Finance##m##DataArray operator+(const Finance##m##DataArray& another);\
Finance##m##DataArray& operator-=(const Finance##m##DataArray& another);\
Finance##m##DataArray operator-(const Finance##m##DataArray& another);

#define DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(m)\
unsigned short get_sub_array(Finance##m##DataArray& new_data_array, int start_index, int length=-1);\
unsigned short get_diff_array(Finance##m##DataArray& new_data_array, int start_index, int length=-1);\
unsigned short get_avg_array(FinanceFloatDataArray& new_data_array, int n, int start_index, int length=-1);


class FinanceIntDataArray;
class FinanceLongDataArray;
class FinanceFloatDataArray;

//typedef FinanceDataArrayTemplate<int> FinanceIntDataArray;
class FinanceIntDataArray : public FinanceDataArrayTemplate<int>
{
public:
	DECLARE_DATA_ARRAY_OPERATOR(Int)
	DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(Int)
//	unsigned short get_sub_array(FinanceIntDataArray& new_data_array, int start_index, int length=-1);
//	unsigned short get_diff_array(FinanceIntDataArray& new_data_array, int start_index, int length=-1);
//	unsigned short get_avg_array(FinanceFloatDataArray& new_data_array, int n, int start_index, int length=-1);
};
typedef FinanceIntDataArray* PFINANCE_INT_DATA_ARRAY;

//typedef FinanceDataArrayTemplate<long> FinanceLongDataArray;
class FinanceLongDataArray : public FinanceDataArrayTemplate<long>
{
public:
	DECLARE_DATA_ARRAY_OPERATOR(Long)
	DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(Long)
};
typedef FinanceLongDataArray* PFINANCE_LONG_DATA_ARRAY;

//typedef FinanceDataArrayTemplate<float> FinanceFloatDataArray;
class FinanceFloatDataArray : public FinanceDataArrayTemplate<float>
{
public:
	DECLARE_DATA_ARRAY_OPERATOR(Float)
	DECLARE_DATA_ARRAY_ELEMENT_CALCULATION(Float)
};
typedef FinanceFloatDataArray* PFINANCE_FLOAT_DATA_ARRAY;

typedef FinanceDataPtrArrayTemplate<char*> FinanceCharDataPtrArray;
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

#define ADD_QUERY_EX(QuerySet, SourceIndex, FieldIndex, SourceTypeIndexSet)\
do{\
ADD_QUERY(QuerySet, SourceIndex, FieldIndex);\
SourceTypeIndexSet.insert(SourceIndex);\
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

typedef std::map<unsigned short, unsigned short> MAP_SHORT;
typedef MAP_SHORT* PMAP_SHORT;
typedef MAP_SHORT::iterator MAP_SHORT_ITER;
typedef MAP_SHORT_ITER* PMAP_SHORT_ITER;
class ResultSet
{
	DECLARE_MSG_DUMPER()

public:
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
	int data_set_mapping_size;
	int date_data_pos;
	int int_data_set_size;
	int long_data_set_size;
	int float_data_set_size;

	unsigned short find_data_pos(int source_index, int field_index, unsigned short& field_type_index, unsigned short& field_type_pos)const;

public:
	ResultSet();
	~ResultSet();

	class iterator
	{
	private:
		MAP_SHORT_ITER iter;

	public:
		iterator(MAP_SHORT_ITER iterator) : iter(iterator){}
		iterator operator++()
		{
			iter++;
			return *this;
		}
		iterator operator++(int junk)
		{
			iterator i = *this;
			iter++;
			return i;
		}
		bool operator==(const iterator& another)
		{
			if (this == &another)
				return true;
			return iter == another.iter;
		}
		bool operator!=(const iterator& another){return !(*this == another);}
		const MAP_SHORT_ITER operator->(){return iter;}
	};

	iterator begin() {return iterator(data_set_mapping.begin());}
	iterator end() {return iterator(data_set_mapping.end());}

	unsigned short add_set(int source_index, int field_index);
	unsigned short add_set(int source_index, const DEQUE_INT& field_set);
	unsigned short set_date(char* element_value);
	unsigned short set_data(int source_index, int field_index, char* data_string);

	void switch_to_check_date_mode();
	unsigned short check_data()const;
	unsigned short show_data()const;

	int get_data_dimension()const;
	int get_data_size()const;

#define DECLARE_GET_ARRAY_FUNC(n, m) const PFINANCE_##m##_DATA_ARRAY get_##n##_array(int source_index, int field_index)const;
	DECLARE_GET_ARRAY_FUNC(int, INT)
	DECLARE_GET_ARRAY_FUNC(long, LONG)
	DECLARE_GET_ARRAY_FUNC(float, FLOAT)
	const PFINANCE_DATA_ARRAY_BASE get_array(int source_index, int field_index)const;
#define DECLARE_GET_ARRAY_ELEMENT_FUNC(n) n get_##n##_array_element(int source_index, int field_index, int index)const;
	DECLARE_GET_ARRAY_ELEMENT_FUNC(int)
	DECLARE_GET_ARRAY_ELEMENT_FUNC(long)
	DECLARE_GET_ARRAY_ELEMENT_FUNC(float)
};
typedef ResultSet* PRESULT_SET;

#endif
