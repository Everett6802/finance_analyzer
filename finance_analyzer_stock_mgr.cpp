#include "finance_analyzer_stock_mgr.h"


DECLARE_MSG_DUMPER_PARAM()

IFinanceAnalyzerMgr* FinanceAnalyzerStockMgr::create_instance()
{
	IFinanceAnalyzerMgr* finance_analyzer_mgr = new FinanceAnalyzerStockMgr();
	return finance_analyzer_mgr;
}

FinanceAnalyzerStockMgr::FinanceAnalyzerStockMgr()
{
}

FinanceAnalyzerStockMgr::~FinanceAnalyzerStockMgr()
{
}

unsigned short FinanceAnalyzerStockMgr::initialize()
{
	WRITE_DEBUG("Initialize FinanceAnalyzerStockMgr.....");
	fprintf(stderr, "Initialize FinanceAnalyzerStockMgr.....\n");
	return RET_SUCCESS;
}
