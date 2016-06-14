#ifndef FINANCE_ANALYZER_WORKDAY_CANLENDAR_H
#define FINANCE_ANALYZER_WORKDAY_CANLENDAR_H

#include <pthread.h>
#include <deque>
// #include <list>
#include <map>
#include <string>
#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


#define DECLARE_COMPANY_PROFILE() PFINANCE_ANALYZER_COMPANY_PROFILE company_profile;
#define IMPLEMENT_COMPANY_PROFILE() company_profile = FinanceAnalyzerCompanyProfile::get_instance();
#define RELEASE_COMPANY_PROFILE() SAFE_RELEASE(company_profile)

typedef std::deque<std::string> PROFILE_ELEMENT_DEQUE;
typedef PROFILE_ELEMENT_DEQUE* PPROFILE_ELEMENT_DEQUE;

class FinanceAnalyzerCompanyProfile
{
	DECLARE_MSG_DUMPER()

	class CompanyProfileEntry;
	typedef CompanyProfileEntry* PCOMPANY_PROFILE_ENTRY;
	typedef std::map<std::string, PCOMPANY_PROFILE_ENTRY> COMPANY_PROFILE_MAP;
	typedef COMPANY_PROFILE_MAP* PCOMPANY_PROFILE_MAP;
	typedef std::deque<PCOMPANY_PROFILE_ENTRY> COMPANY_PROFILE_DEQUE;
	typedef COMPANY_PROFILE_DEQUE* PCOMPANY_PROFILE_DEQUE;

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
	std::string* company_group_description_array;
	int company_group_size;

	FinanceAnalyzerCompanyProfile();
	FinanceAnalyzerCompanyProfile(const FinanceAnalyzerCompanyProfile&);
	const FinanceAnalyzerCompanyProfile& operator=(const FinanceAnalyzerCompanyProfile&);
	~FinanceAnalyzerCompanyProfile();

	unsigned short initialize();
	unsigned short parse_company_profile_conf();
	unsigned short parse_company_group_conf();

public:
	static FinanceAnalyzerCompanyProfile* get_instance();
	int add_ref();
	int release();

	const PPROFILE_ELEMENT_DEQUE lookup_company_profile(std::string company_number)const;
	std::string lookup_company_listing_date(std::string company_number)const;
	std::string lookup_company_group_name(std::string company_number)const;
	std::string lookup_company_group_number(std::string company_number)const;
	unsigned short generate_company_profile_sorted_deque();
};
typedef FinanceAnalyzerCompanyProfile* PFINANCE_ANALYZER_COMPANY_PROFILE;

#endif
