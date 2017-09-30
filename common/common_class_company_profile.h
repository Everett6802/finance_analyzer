#ifndef COMMON_CLASS_COMPANY_PROFILE_H
#define COMMON_CLASS_COMPANY_PROFILE_H

#include "msg_dumper_wrapper.h"
#include "common_definition.h"
#include "common_class_base.h"


#define DECLARE_COMPANY_PROFILE() PCOMPANY_PROFILE company_profile;
#define IMPLEMENT_COMPANY_PROFILE() company_profile = CompanyProfile::get_instance();
#define RELEASE_COMPANY_PROFILE() SAFE_RELEASE(company_profile)

#define DECLARE_AND_IMPLEMENT_STATIC_COMPANY_PROFILE()\
static PCOMPANY_PROFILE company_profile = CompanyProfile::get_instance();

class CompanyProfileEntry;
typedef CompanyProfileEntry* PCOMPANY_PROFILE_ENTRY;

class CompanyProfile
{
	DECLARE_MSG_DUMPER()

	typedef std::map<std::string, PCOMPANY_PROFILE_ENTRY> COMPANY_PROFILE_MAP;
	typedef COMPANY_PROFILE_MAP* PCOMPANY_PROFILE_MAP;
	typedef std::deque<PCOMPANY_PROFILE_ENTRY> COMPANY_PROFILE_DEQUE;
	typedef COMPANY_PROFILE_DEQUE* PCOMPANY_PROFILE_DEQUE;
	typedef std::deque<PCOMPANY_PROFILE_DEQUE> COMPANY_GROUP_PROFILE_DEQUE;
	typedef COMPANY_GROUP_PROFILE_DEQUE* PCOMPANY_GROUP_PROFILE_DEQUE;
	typedef std::deque<PCOMPANY_PROFILE_ENTRY>::iterator COMPANY_PROFILE_ENTRY_DEQUE_ITER;

private:
	static CompanyProfile* instance;
	volatile int ref_cnt;
	COMPANY_PROFILE_MAP company_profile_map;
	PCOMPANY_PROFILE_DEQUE company_profile_sorted_deque;
	PCOMPANY_GROUP_PROFILE_DEQUE company_group_profile_sorted_deque;
	int company_group_size;
	std::vector<std::string> company_group_description_vector;

	CompanyProfile();
	CompanyProfile(const CompanyProfile&);
	const CompanyProfile& operator=(const CompanyProfile&);
	~CompanyProfile();

	unsigned short initialize();
	unsigned short parse_company_profile_conf();
	unsigned short parse_company_group_conf();
	unsigned short generate_company_profile_sorted_deque();
	unsigned short generate_company_group_profile_sorted_deque();
	void init_company_profile_sorted_deque();
	void init_company_group_profile_sorted_deque();

public:
	static CompanyProfile* get_instance();
	int add_ref();
	int release();

	class const_iterator
	{
	private:
		COMPANY_PROFILE_ENTRY_DEQUE_ITER iter;

	public:
		const_iterator(COMPANY_PROFILE_ENTRY_DEQUE_ITER iterator);
		const_iterator operator++();
		bool operator==(const const_iterator& another);
		bool operator!=(const const_iterator& another);
		const PSTRING_DEQUE operator->();
		const STRING_DEQUE& operator*();
	};

	const_iterator begin();
	const_iterator end();

	const_iterator group_begin(int index); 
	const_iterator group_end(int index);

	static void show_traverse_result(CompanyProfile* company_profile, bool company_group_mode=true);

	int get_company_group_size()const;
	std::string get_company_group_description(int index)const;
	const PSTRING_DEQUE get_company_number_list_in_group(int index);

	const PSTRING_DEQUE lookup_company_profile(std::string company_number)const;
	std::string lookup_company_listing_date(std::string company_number)const;
	std::string lookup_company_group_name(std::string company_number)const;
	std::string lookup_company_group_number(std::string company_number)const;
	int get_company_group_number(std::string company_number)const;
	bool is_company_exist(std::string company_number)const;
};
typedef CompanyProfile* PCOMPANY_PROFILE;

#endif
