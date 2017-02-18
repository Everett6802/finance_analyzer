#ifndef FINANCE_ANALYZER_MGR_FACTORY_H
#define FINANCE_ANALYZER_MGR_FACTORY_H

#include <map>
#include <string>
#include "finance_analyzer_common.h"
#include "finance_analyzer_mgr.h"


template <class T> PIFINANCE_ANALYZER_MGR construct(){return (PIFINANCE_ANALYZER_MGR)new T();}
// template <class T> PIFINANCE_ANALYZER_MGR construct(){return T::create_instance();}

class FinanceAnalyzerMgrFactory
{
private:
	typedef PIFINANCE_ANALYZER_MGR (*CONSTRUCT_FUNC_PTR)();
	typedef std::map<int, CONSTRUCT_FUNC_PTR> CONSTRUCT_MAP;
	CONSTRUCT_MAP construct_map;

public:
	template<class T> void register_class(int class_mode);
	PIFINANCE_ANALYZER_MGR get_instance(int class_mode);
};

template<class T> 
void FinanceAnalyzerMgrFactory::register_class(int class_mode)
{
	construct_map.insert(make_pair(class_mode, &construct<T>));
}

#endif
