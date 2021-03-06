#include "mgr_factory.h"


using namespace std;

PIFINANCE_ANALYZER_MGR MgrFactory::get_instance(int class_mode)
{
	CONSTRUCT_MAP::iterator iter = construct_map.find(class_mode);
	if (iter == construct_map.end())
		return NULL;
	return iter->second();
}
