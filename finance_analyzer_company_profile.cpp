#include <assert.h>
#include <stdexcept>
#include <string>
#include <algorithm>
#include "finance_analyzer_company_profile.h"


using namespace std;

static 	pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
DECLARE_MSG_DUMPER_PARAM()

class FinanceAnalyzerCompanyProfile::CompanyProfileEntry
{
public:
	PPROFILE_ELEMENT_DEQUE profile_element_deque;
	CompanyProfileEntry() :
		profile_element_deque(NULL)
	{
		profile_element_deque = new PROFILE_ELEMENT_DEQUE();
		if (profile_element_deque == NULL)
			throw bad_alloc();
	}
	~CompanyProfileEntry()
	{
		if (profile_element_deque != NULL)
		{
			delete profile_element_deque;
			profile_element_deque = NULL;
		}
	}
	void push_back(const string& new_element){profile_element_deque->push_back(new_element);}
	bool operator< (const FinanceAnalyzerCompanyProfile::CompanyProfileEntry& another)
	{
		assert(profile_element_deque != NULL && "profile_element_deque should NOT be NULL");
		assert(another.profile_element_deque != NULL && "another.profile_element_deque should NOT be NULL");
		if (this == &another)
			return true;
		return strcmp((*profile_element_deque)[COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER].c_str(), (*another.profile_element_deque)[COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER].c_str());
	}
	const string to_string()const
	{
		static string white_space_str(" ");
		assert(profile_element_deque != NULL && "profile_element_deque != NULL");
		PROFILE_ELEMENT_DEQUE::iterator iter = profile_element_deque->begin();
		string res = "";
		while(iter != profile_element_deque->end())
		{
			res += (string(*iter) + white_space_str);
			iter++;
		}
		return res;
	}
};


const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_CODE_NUMBER = 0;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_COMPANY_NAME = 1;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_LISTING_DATE = 3;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_MARKET = 4;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_INDUSTRY = 5;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NAME = 7;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NUMBER = 8;
const int FinanceAnalyzerCompanyProfile::COMPANY_PROFILE_ENTRY_FIELD_SIZE = 9;

FinanceAnalyzerCompanyProfile* FinanceAnalyzerCompanyProfile::instance = NULL;

FinanceAnalyzerCompanyProfile* FinanceAnalyzerCompanyProfile::get_instance()
{
	if (instance == NULL)
	{
		pthread_mutex_lock(&mtx);
		if (instance == NULL)
		{
			instance = new FinanceAnalyzerCompanyProfile();
			if (instance == NULL)
				throw bad_alloc();
			unsigned short ret = instance->initialize();
			if (CHECK_FAILURE(ret))
			{
				char errmsg[256];
				snprintf(errmsg, 256, "Fail to initialize the FinanceAnalyzerCompanyProfile object , due to: %s", get_ret_description(ret));
				throw runtime_error(string(errmsg));
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	instance->add_ref();
	return instance;
}

FinanceAnalyzerCompanyProfile::FinanceAnalyzerCompanyProfile() :
	ref_cnt(0),
	company_profile_sorted_deque(NULL),
	company_group_description_array(NULL),
	company_group_size(0)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerCompanyProfile::~FinanceAnalyzerCompanyProfile()
{
	if (company_group_description_array != NULL)
	{
		free(company_group_description_array);
		company_group_description_array = NULL;
		company_group_size = 0;
	}
	if (company_profile_sorted_deque != NULL)
	{
		delete company_profile_sorted_deque;
		company_profile_sorted_deque = NULL;
	}
	for(COMPANY_PROFILE_MAP::iterator iter = company_profile_map.begin() ; iter != company_profile_map.end() ; iter++)
	{
		PCOMPANY_PROFILE_ENTRY company_profile_entry = (PCOMPANY_PROFILE_ENTRY)iter->second;
		delete company_profile_entry;
	}
	company_profile_map.clear();

	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerCompanyProfile::initialize()
{
	unsigned short ret = RET_SUCCESS;
	ret = parse_company_profile_conf();
	if (CHECK_FAILURE(ret))
		return ret;
	ret = parse_company_group_conf();
	if (CHECK_FAILURE(ret))
		return ret;

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerCompanyProfile::parse_company_profile_conf()
{
	CREATE_PROJECT_FILEPATH(file_path, CONFIG_FOLDER_NAME, COMPANY_PROFILE_CONF_FILENAME)
// First check if the config file exists
	if (!check_file_exist(file_path))
	{
		char errmsg[256];
		snprintf(errmsg, 256, "The %s config file does NOT exist", COMPANY_PROFILE_CONF_FILENAME);
		WRITE_ERROR(errmsg);
		fprintf(stderr, "%s\n", errmsg);
		return RET_FAILURE_NOT_FOUND;
	}
	WRITE_FORMAT_DEBUG("Try to parse the config: %s", file_path);

	static const unsigned int BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	unsigned short ret = RET_SUCCESS;
	WRITE_FORMAT_DEBUG("Parse the config file: %s", COMPANY_PROFILE_CONF_FILENAME);
	FILE* fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the config file: %s, due to: %s", COMPANY_PROFILE_CONF_FILENAME, strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
		// PPROFILE_ELEMENT_DEQUE profile_element_deque = new PROFILE_ELEMENT_DEQUE();
		// if (profile_element_deque == NULL)
		// {
		// 	WRITE_ERROR("Fail to allocate the memory: profile_element_deque");
		// 	ret = RET_FAILURE_INSUFFICIENT_MEMORY;
		// 	goto OUT;
		// }
		PCOMPANY_PROFILE_ENTRY company_profile_entry = new CompanyProfileEntry();
		if (company_profile_entry == NULL)
		{
			WRITE_ERROR("Fail to allocate the memory: company_profile_entry");
			ret = RET_FAILURE_INSUFFICIENT_MEMORY;
			goto OUT;
		}

		char* company_code_number = strtok(buf, ",");
		if (company_code_number == NULL)
		{
			WRITE_FORMAT_ERROR("Incorrect format: %s", buf);
			ret = RET_FAILURE_INCORRECT_CONFIG;
			goto OUT;			
		}
		string company_code_number_str(company_code_number);
		company_profile_entry->push_back(company_code_number_str);
		char* entry = NULL;
		while((entry = strtok(NULL, ",")) != NULL)
		{
			string entry_str(entry);
			company_profile_entry->push_back(entry_str);
		}
		company_profile_map[company_code_number_str] = company_profile_entry;
	}
OUT:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return ret;
}

unsigned short FinanceAnalyzerCompanyProfile::parse_company_group_conf()
{
	CREATE_PROJECT_FILEPATH(file_path, CONFIG_FOLDER_NAME, COMPANY_GROUP_CONF_FILENAME)
// First check if the config file exists
	if (!check_file_exist(file_path))
	{
		char errmsg[256];
		snprintf(errmsg, 256, "The %s config file does NOT exist", COMPANY_GROUP_CONF_FILENAME);
		WRITE_ERROR(errmsg);
		fprintf(stderr, "%s\n", errmsg);
		return RET_FAILURE_NOT_FOUND;
	}
	WRITE_FORMAT_DEBUG("Try to parse the config: %s", file_path);

	static const unsigned int DEF_COMPANY_GROUP_COUNT = 64;
	int company_group_count = DEF_COMPANY_GROUP_COUNT;
	company_group_description_array = new string [company_group_count];
	if (company_group_description_array == NULL)
	{
		WRITE_ERROR("Fail to allocate the memory: company_group_description_array");
		return RET_FAILURE_INSUFFICIENT_MEMORY;	
	}

	static const unsigned int BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	unsigned short ret = RET_SUCCESS;
	WRITE_FORMAT_DEBUG("Parse the config file: %s", COMPANY_GROUP_CONF_FILENAME);
	FILE* fp = fopen(file_path, "r");
	if (fp == NULL)
	{
		WRITE_FORMAT_ERROR("Fail to open the config file: %s, due to: %s", COMPANY_GROUP_CONF_FILENAME, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}

	int line_cnt = 0;
	char* group_number = NULL;
	char* group_description = NULL;
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
// Check if the buffer size is enough
		if (line_cnt == company_group_count)
		{
			int company_group_count_old = company_group_count;
			string* company_group_description_array_old = company_group_description_array;
			company_group_count <<= 1;
			WRITE_FORMAT_WARN("Size is NOT enough, enlarge the array from %d to %d", company_group_count, company_group_count_old);
			company_group_description_array = (string*)realloc(company_group_description_array_old, sizeof(string) * company_group_count);
			if (company_group_description_array == NULL)
			{
				WRITE_ERROR("Fail to re-allocate the memory: company_group_description_array");
				ret = RET_FAILURE_INSUFFICIENT_MEMORY;
				goto OUT;
			}
		}
		group_number = strtok(buf, " ");
		group_description = strtok(NULL, " ");
		if (atoi(group_number) != line_cnt)
		{
			WRITE_FORMAT_ERROR("Incorrect company group number, expected: %d, actual: %d", line_cnt, atoi(group_number));
			ret = RET_FAILURE_INVALID_ARGUMENT;
			goto OUT;		
		}
		company_group_description_array[line_cnt++] = string(group_description);
	}
	company_group_size = line_cnt;
	WRITE_FORMAT_DEBUG("There are totally %d company group", company_group_size);
OUT:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return ret;

}

int FinanceAnalyzerCompanyProfile::add_ref()
{
	__sync_fetch_and_add(&ref_cnt, 1);
	return ref_cnt;
}

int FinanceAnalyzerCompanyProfile::release()
{
	__sync_fetch_and_sub(&ref_cnt, 1);
	if (ref_cnt == 0)
	{
		delete this;
		return 0;
	}
	return ref_cnt;
}

const PPROFILE_ELEMENT_DEQUE FinanceAnalyzerCompanyProfile::lookup_company_profile(string company_number)const
{
	// PPROFILE_ELEMENT_DEQUE company_profile = company_profile_map.get(company_number);
	COMPANY_PROFILE_MAP::const_iterator iter = company_profile_map.find(company_number);
	if (iter == company_profile_map.end()) 
	{
		static const int ERRMSG_SIZE = 256;
		static char errmsg[ERRMSG_SIZE];
		snprintf(errmsg, ERRMSG_SIZE, "Fail to find the company profile of company number: %s", company_number.c_str());
		throw invalid_argument(string(errmsg));
	}

	return ((PCOMPANY_PROFILE_ENTRY)iter->second)->profile_element_deque;
}

string FinanceAnalyzerCompanyProfile::lookup_company_listing_date(string company_number)const
{
	return (*lookup_company_profile(company_number))[COMPANY_PROFILE_ENTRY_FIELD_INDEX_LISTING_DATE];
}

string FinanceAnalyzerCompanyProfile::lookup_company_group_name(string company_number)const
{
	return (*lookup_company_profile(company_number))[COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NAME];
}

string FinanceAnalyzerCompanyProfile::lookup_company_group_number(string company_number)const
{
	return (*lookup_company_profile(company_number))[COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NUMBER];
}

unsigned short FinanceAnalyzerCompanyProfile::generate_company_profile_sorted_deque()
{
	if (company_profile_sorted_deque != NULL)
		return RET_FAILURE_INCORRECT_OPERATION;
	company_profile_sorted_deque = new COMPANY_PROFILE_DEQUE();
	if (company_profile_sorted_deque == NULL)
	{
		WRITE_ERROR("Fail to allocate the memory: company_profile_sorted_deque");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	for(COMPANY_PROFILE_MAP::iterator iter = company_profile_map.begin() ; iter != company_profile_map.end() ; iter++)
	{
		PCOMPANY_PROFILE_ENTRY company_profile_entry = (PCOMPANY_PROFILE_ENTRY)iter->second;
		company_profile_sorted_deque->push_back(company_profile_entry);
	}
	cout << "size: " << company_profile_sorted_deque->size() << endl;
	std::sort(company_profile_sorted_deque->begin(), company_profile_sorted_deque->end());
	COMPANY_PROFILE_DEQUE::iterator iter = company_profile_sorted_deque->begin();
	while(iter != company_profile_sorted_deque->end())
	{
		cout << ((PCOMPANY_PROFILE_ENTRY)*iter)->to_string();
		iter++;
	}
	// for(int i = 0 ; i < company_profile_sorted_deque->size() ; i++)
	// 	cout << (*company_profile_sorted_deque)[i]->to_string() << endl;
	return RET_SUCCESS;
}
