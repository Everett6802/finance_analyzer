#ifndef FINANCE_ANALYZER_WORKDAY_CANLENDAR_H
#define FINANCE_ANALYZER_WORKDAY_CANLENDAR_H

#include <pthread.h>
#include <deque>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


#define DECLARE_COMPANY_PROFILE() PFINANCE_ANALYZER_COMPANY_PROFILE company_profile;
#define IMPLEMENT_COMPANY_PROFILE() company_profile = FinanceAnalyzerCompanyProfile::get_instance();
#define RELEASE_COMPANY_PROFILE() SAFE_RELEASE(company_profile)

#define DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE()\
static PFINANCE_ANALYZER_COMPANY_PROFILE company_profile = FinanceAnalyzerCompanyProfile::get_instance();

class CompanyProfileEntry;

typedef std::deque<std::string> STRING_DEQUE;
typedef STRING_DEQUE* PSTRING_DEQUE;

// typedef std::deque<std::string> PROFILE_ELEMENT_DEQUE;
// typedef PROFILE_ELEMENT_DEQUE* PPROFILE_ELEMENT_DEQUE;
// typedef std::deque<std::string> STRING_DEQUE;
// typedef COMPANY_NUMBER_DEQUE* PCOMPANY_NUMBER_DEQUE;

class FinanceAnalyzerCompanyProfile
{
	DECLARE_MSG_DUMPER()

	class CompanyProfileEntry
	{
	public:
		PSTRING_DEQUE profile_element_deque;
		CompanyProfileEntry() :
			profile_element_deque(NULL)
		{
			profile_element_deque = new STRING_DEQUE();
			if (profile_element_deque == NULL)
				throw std::bad_alloc();
		}
		~CompanyProfileEntry()
		{
			if (profile_element_deque != NULL)
			{
				delete profile_element_deque;
				profile_element_deque = NULL;
			}
		}
		void push_back(const std::string& new_element){profile_element_deque->push_back(new_element);}
		bool operator< (const FinanceAnalyzerCompanyProfile::CompanyProfileEntry& another)
		{
			assert(profile_element_deque != NULL && "profile_element_deque should NOT be NULL");
			assert(another.profile_element_deque != NULL && "another.profile_element_deque should NOT be NULL");
			if (this == &another)
				return true;
			return strcmp((*profile_element_deque)[COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER].c_str(), (*another.profile_element_deque)[COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER].c_str());
		}
		const std::string& operator[](int index)const
		{
			assert ((index >= 0 && index < COMPANY_PROFILE_ENTRY_FIELD_SIZE) && "index should NOT be Out of Range");
			return (*profile_element_deque)[index];
		}
		const std::string to_string()const
		{
			static std::string white_space_str(" ");
			assert(profile_element_deque != NULL && "profile_element_deque != NULL");
			STRING_DEQUE::iterator iter = profile_element_deque->begin();
			std::string res = "";
			while(iter != profile_element_deque->end())
			{
				res += (std::string(*iter) + white_space_str);
				iter++;
			}
			return res;
		}
		size_t size()const{return profile_element_deque->size();}
	};
	typedef CompanyProfileEntry* PCOMPANY_PROFILE_ENTRY;
	typedef std::map<std::string, PCOMPANY_PROFILE_ENTRY> COMPANY_PROFILE_MAP;
	typedef COMPANY_PROFILE_MAP* PCOMPANY_PROFILE_MAP;
	typedef std::deque<PCOMPANY_PROFILE_ENTRY> COMPANY_PROFILE_DEQUE;
	typedef COMPANY_PROFILE_DEQUE* PCOMPANY_PROFILE_DEQUE;
	typedef std::deque<PCOMPANY_PROFILE_DEQUE> COMPANY_GROUP_PROFILE_DEQUE;
	typedef COMPANY_GROUP_PROFILE_DEQUE* PCOMPANY_GROUP_PROFILE_DEQUE;
	typedef std::deque<PCOMPANY_PROFILE_ENTRY>::iterator COMPANY_PROFILE_ENTRY_DEQUE_ITER;

	static bool compare_company_number(const PCOMPANY_PROFILE_ENTRY company_profile_entry1, const PCOMPANY_PROFILE_ENTRY company_profile_entry2);

private:
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER;
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_NAME;
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_LISTING_DATE;
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_MARKET;
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_INDUSTRY;
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NAME;
	static const int COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NUMBER;
	static const int COMPANY_PROFILE_ENTRY_FIELD_SIZE;
	static FinanceAnalyzerCompanyProfile* instance;
	volatile int ref_cnt;
	COMPANY_PROFILE_MAP company_profile_map;
	PCOMPANY_PROFILE_DEQUE company_profile_sorted_deque;
	int company_group_size;
	PCOMPANY_GROUP_PROFILE_DEQUE company_group_profile_sorted_deque;
	std::vector<std::string> company_group_description_vector;
	// std::vector<STRING_DEQUE> company_number_list_in_group_vector;

	FinanceAnalyzerCompanyProfile();
	FinanceAnalyzerCompanyProfile(const FinanceAnalyzerCompanyProfile&);
	const FinanceAnalyzerCompanyProfile& operator=(const FinanceAnalyzerCompanyProfile&);
	~FinanceAnalyzerCompanyProfile();

	unsigned short initialize();
	unsigned short parse_company_profile_conf();
	unsigned short parse_company_group_conf();
	unsigned short generate_company_profile_sorted_deque();
	unsigned short generate_company_group_profile_sorted_deque();

// It seems that it's NOT sutiable for lazy initialization in Singleton
	// const PCOMPANY_PROFILE_DEQUE get_company_profile_sorted_deque();
	// {
	// 	if (company_profile_sorted_deque == NULL)
	// 	{
	// 		unsigned short ret = generate_company_profile_sorted_deque();
	// 		if (CHECK_FAILURE(ret))
	// 			throw std::runtime_error(std::string(get_ret_description(ret)));
	// 	}
	// 	return company_profile_sorted_deque;
	// }
	// const PCOMPANY_GROUP_PROFILE_DEQUE get_company_group_profile_sorted_deque();
	// {
	// 	if (company_group_profile_sorted_deque == NULL)
	// 	{
	// 		unsigned short ret = generate_company_group_profile_sorted_deque();
	// 		if (CHECK_FAILURE(ret))
	// 			throw std::runtime_error(std::string(get_ret_description(ret)));
	// 	}
	// 	return company_group_profile_sorted_deque;
	// }

public:
	static FinanceAnalyzerCompanyProfile* get_instance();
	int add_ref();
	int release();

	class const_iterator
	{
	private:
		COMPANY_PROFILE_ENTRY_DEQUE_ITER iter;

	public:
		const_iterator(COMPANY_PROFILE_ENTRY_DEQUE_ITER iterator) : iter(iterator){}
		const_iterator operator++()
		{
			++iter;
			return *this;
		}
		bool operator==(const const_iterator& another)
		{
			if (this == &another)
				return true;
			return iter == another.iter;
		}
		bool operator!=(const const_iterator& another){return !(*this == another);}
		const PSTRING_DEQUE operator->(){return ((PCOMPANY_PROFILE_ENTRY)(*iter))->profile_element_deque;}
		const STRING_DEQUE& operator*(){return *((PCOMPANY_PROFILE_ENTRY)(*iter))->profile_element_deque;}
	};

	const_iterator begin() 
	{
		// const PCOMPANY_PROFILE_DEQUE company_profile_deque = get_company_profile_sorted_deque();
		// return const_iterator(company_profile_deque->begin());
		return const_iterator(company_profile_sorted_deque->begin());
	}
	const_iterator end() 
	{
		// const PCOMPANY_PROFILE_DEQUE company_profile_deque = get_company_profile_sorted_deque();
		// return const_iterator(company_profile_deque->end());
		return const_iterator(company_profile_sorted_deque->end());
	}
	const_iterator group_begin(int index) 
	{
		// const PCOMPANY_GROUP_PROFILE_DEQUE company_group_profile_deque = get_company_group_profile_sorted_deque();
		if (index < 0 || index >= company_group_size)
		{
			throw std::invalid_argument("index is Out Of Range");
		}
		// return const_iterator(((*company_group_profile_deque)[index])->begin());
		return const_iterator(((*company_group_profile_sorted_deque)[index])->begin());
	}
	const_iterator group_end(int index) 
	{
		// const PCOMPANY_GROUP_PROFILE_DEQUE company_group_profile_deque = get_company_group_profile_sorted_deque();
		if (index < 0 || index >= company_group_size)
		{
			throw std::invalid_argument("index is Out Of Range");
		}
		// return const_iterator(((*company_group_profile_deque)[index])->end());
		return const_iterator(((*company_group_profile_sorted_deque)[index])->end());
	}

	static void show_traverse_result(FinanceAnalyzerCompanyProfile* company_profile, bool company_group_mode=true);

	int get_company_group_size()const;
	std::string get_company_group_description(int index)const;
	const PSTRING_DEQUE get_company_number_list_in_group(int index);
	const PSTRING_DEQUE lookup_company_profile(std::string company_number)const;
	std::string lookup_company_listing_date(std::string company_number)const;
	std::string lookup_company_group_name(std::string company_number)const;
	std::string lookup_company_group_number(std::string company_number)const;
};
typedef FinanceAnalyzerCompanyProfile* PFINANCE_ANALYZER_COMPANY_PROFILE;

#endif
