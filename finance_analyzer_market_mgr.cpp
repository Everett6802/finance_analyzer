#include "finance_analyzer_market_mgr.h"


DECLARE_MSG_DUMPER_PARAM()

IFinanceAnalyzerMgr* FinanceAnalyzerMarketMgr::create_instance()
{
	IFinanceAnalyzerMgr* finance_analyzer_mgr = new FinanceAnalyzerMarketMgr();
	return finance_analyzer_mgr;
}

FinanceAnalyzerMarketMgr::FinanceAnalyzerMarketMgr()
{
}

FinanceAnalyzerMarketMgr::~FinanceAnalyzerMarketMgr()
{
}

unsigned short FinanceAnalyzerMarketMgr::initialize()
{
	WRITE_DEBUG("Initialize FinanceAnalyzerMarketMgr.....");
	fprintf(stderr, "Initialize FinanceAnalyzerMarketMgr.....\n");
	return RET_SUCCESS;
}
