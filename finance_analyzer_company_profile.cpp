#include <assert.h>
#include <stdexcept>
#include <string>
#include <algorithm>
#include "finance_analyzer_company_profile.h"


using namespace std;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
DECLARE_MSG_DUMPER_PARAM()

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
	company_group_size(0),
	company_group_profile_sorted_deque(NULL)
{
	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerCompanyProfile::~FinanceAnalyzerCompanyProfile()
{
	if (company_group_profile_sorted_deque != NULL)
	{
		for (int i = 0 ; i < company_group_size ; i++)
		{
			PCOMPANY_PROFILE_DEQUE company_profile_deque = (*company_group_profile_sorted_deque)[i];
			if (company_profile_deque != NULL)
				delete company_profile_deque;
		}
		delete company_group_profile_sorted_deque;
		company_group_profile_sorted_deque = NULL;
	}
	if (company_group_profile_sorted_deque != NULL)
	{
		COMPANY_GROUP_PROFILE_DEQUE::iterator iter = company_group_profile_sorted_deque->begin();
		while(iter !=  company_group_profile_sorted_deque->end())
		{
			PCOMPANY_PROFILE_DEQUE company_profile_deque = (PCOMPANY_PROFILE_DEQUE)(*iter);
			iter++;
			delete company_profile_deque;
		}
		delete company_group_profile_sorted_deque;
		company_group_profile_sorted_deque = NULL;
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
	ret = generate_company_profile_sorted_deque();
	if (CHECK_FAILURE(ret))
		return ret;
	ret = generate_company_group_profile_sorted_deque();
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
		return RET_FAILURE_SYSTEM_API;
	}
// Parse the config file
	int buf_len;
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
		buf_len = strlen(buf);
		if (buf[buf_len - 1] == '\n')
			buf[buf_len - 1] = '\0';
		PCOMPANY_PROFILE_ENTRY company_profile_entry = new CompanyProfileEntry();
		if (company_profile_entry == NULL)
		{
			WRITE_ERROR("Fail to allocate the memory: company_profile_entry");
			ret = RET_FAILURE_INSUFFICIENT_MEMORY;
			goto OUT;
		}
// Parse each element in the entry
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
		assert(company_profile_entry->size() == (size_t)COMPANY_PROFILE_ENTRY_FIELD_SIZE && "Incorrect company profile entry size");
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

	// static const unsigned int DEF_COMPANY_GROUP_COUNT = 64;
	// int company_group_count = DEF_COMPANY_GROUP_COUNT;
	// company_group_description_vector = new string [company_group_count];
	// if (company_group_description_vector == NULL)
	// {
	// 	WRITE_ERROR("Fail to allocate the memory: company_group_description_array");
	// 	return RET_FAILURE_INSUFFICIENT_MEMORY;	
	// }

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

// Parse the config file
	int buf_len;
	int line_cnt = 0;
	char* group_number = NULL;
	char* group_description = NULL;
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
		buf_len = strlen(buf);
		if (buf[buf_len - 1] == '\n')
			buf[buf_len - 1] = '\0';
// Parse each element in the entry
		group_number = strtok(buf, " ");
		group_description = strtok(NULL, " ");
		if (atoi(group_number) != line_cnt)
		{
			WRITE_FORMAT_ERROR("Incorrect company group number, expected: %d, actual: %d", line_cnt, atoi(group_number));
			ret = RET_FAILURE_INVALID_ARGUMENT;
			goto OUT;		
		}
		company_group_description_vector.push_back(string(group_description));
		line_cnt++;
	}
	assert(company_group_description_vector.size() == (unsigned int)line_cnt && "The company_group_description_vector size is NOT correct");
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

void FinanceAnalyzerCompanyProfile::show_traverse_result(FinanceAnalyzerCompanyProfile* company_profile, bool company_group_mode)
{
	assert(company_profile != NULL && "company_profile should NOT be NULL");
	if (company_group_mode)
	{
		int company_group_size = company_profile->get_company_group_size();
		for (int i = 0 ; i < company_group_size ; i++)
		{
			cout << "****************** " << i << " : " << company_profile->get_company_group_description(i) << " ******************" << endl;
			FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->group_begin(i);
			while (iter != company_profile->group_end(i))
			{
				const PROFILE_ELEMENT_DEQUE& profile_element_deque = *iter;
				cout << profile_element_deque[0] << endl;
				++iter;
			}
		}
	}
	else
	{
		FinanceAnalyzerCompanyProfile::const_iterator iter = company_profile->begin();
		while (iter != company_profile->end())
		{
			const PROFILE_ELEMENT_DEQUE& profile_element_deque = *iter;
			cout << profile_element_deque[0] << endl;
			++iter;
		}
	}
}

int FinanceAnalyzerCompanyProfile::get_company_group_size()const{return company_group_size;}

std::string FinanceAnalyzerCompanyProfile::get_company_group_description(int index)const
{
	if (index < 0 || index >= company_group_size)
		throw std::invalid_argument("index is Out Of Range");
	return company_group_description_vector[index];
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
	std::sort(company_profile_sorted_deque->begin(), company_profile_sorted_deque->end());
	// cout << "size: " << company_profile_sorted_deque->size() << endl;
	// COMPANY_PROFILE_DEQUE::iterator iter = company_profile_sorted_deque->begin();
	// while(iter != company_profile_sorted_deque->end())
	// {
	// 	cout << ((PCOMPANY_PROFILE_ENTRY)*iter)->to_string();
	// 	iter++;
	// }

	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerCompanyProfile::generate_company_group_profile_sorted_deque()
{
	if (company_group_profile_sorted_deque != NULL)
		return RET_FAILURE_INCORRECT_OPERATION;
	company_group_profile_sorted_deque = new COMPANY_GROUP_PROFILE_DEQUE();
	if (company_group_profile_sorted_deque == NULL)
	{
		WRITE_ERROR("Fail to allocate the memory: company_group_profile_sorted_deque");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	for (int i = 0 ; i < company_group_size ; i++)
	{
		PCOMPANY_PROFILE_DEQUE company_profile_deque = new COMPANY_PROFILE_DEQUE();
		if (company_profile_deque == NULL)
		{
			WRITE_ERROR("Fail to allocate the memory: ompany_profile_deque");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
		company_group_profile_sorted_deque->push_back(company_profile_deque);
	}
	for(COMPANY_PROFILE_MAP::iterator iter = company_profile_map.begin() ; iter != company_profile_map.end() ; iter++)
	{
		PCOMPANY_PROFILE_ENTRY company_profile_entry = (PCOMPANY_PROFILE_ENTRY)iter->second;
		assert(company_profile_entry != NULL && "company_profile_entry should NOT be NULL");
		// cout << "entry: " << company_profile_entry->to_string() << endl;
		const string& company_group_number = (*company_profile_entry)[COMPANY_PROFILE_ENTRY_FIELD_INDEX_GROUP_NUMBER];
		(*company_group_profile_sorted_deque)[atoi(company_group_number.c_str())]->push_back(company_profile_entry);
	}
	for (int i = 0 ; i < company_group_size ; i++)
	{
		PCOMPANY_PROFILE_DEQUE company_profile_deque = (*company_group_profile_sorted_deque)[i];
		assert(company_profile_deque != NULL && "company_profile_deque should NOT be NULL");
		std::sort(company_profile_deque->begin(), company_profile_deque->end());
		// cout << "++++++++++++++++ group [" << i << "] member count: " << company_profile_deque->size() << " ++++++++++++++++" << endl;
		// COMPANY_PROFILE_DEQUE::iterator iter = company_profile_deque->begin();
		// while(iter != company_profile_deque->end())
		// {
		// 	cout << ((PCOMPANY_PROFILE_ENTRY)*iter)->to_string() << endl;
		// 	iter++;
		// }
	}

	return RET_SUCCESS;
}

	// const PCOMPANY_PROFILE_DEQUE FinanceAnalyzerCompanyProfile::get_company_profile_sorted_deque()
	// {
	// 	if (company_profile_sorted_deque == NULL)
	// 	{
	// 		unsigned short ret = generate_company_profile_sorted_deque();
	// 		if (CHECK_FAILURE(ret))
	// 			throw std::runtime_error(std::string(get_ret_description(ret)));
	// 	}
	// 	return company_profile_sorted_deque;
	// }

	// const PCOMPANY_GROUP_PROFILE_DEQUE FinanceAnalyzerCompanyProfile::get_company_group_profile_sorted_deque()
	// {
	// 	if (company_group_profile_sorted_deque == NULL)
	// 	{
	// 		unsigned short ret = generate_company_group_profile_sorted_deque();
	// 		if (CHECK_FAILURE(ret))
	// 			throw std::runtime_error(std::string(get_ret_description(ret)));
	// 	}
	// 	return company_group_profile_sorted_deque;
	// }


// const PCOMPANY_PROFILE_DEQUE FinanceAnalyzerCompanyProfile::get_company_profile_sorted_deque()
// {
// 	if (company_profile_sorted_deque == NULL)
// 	{
// 		unsigned short ret = generate_company_profile_sorted_deque();
// 		if (CHECK_FAILURE(ret))
// 			throw runtime_error(string(get_ret_description(ret)));
// 	}
// 	return company_profile_sorted_deque;
// }

// const PCOMPANY_GROUP_PROFILE_DEQUE FinanceAnalyzerCompanyProfile::get_company_group_profile_sorted_deque()
// {
// 	if (company_group_profile_sorted_deque == NULL)
// 	{
// 		unsigned short ret = generate_company_group_profile_sorted_deque();
// 		if (CHECK_FAILURE(ret))
// 			throw runtime_error(string(get_ret_description(ret)));
// 	}
// 	return company_group_profile_sorted_deque;
// }
