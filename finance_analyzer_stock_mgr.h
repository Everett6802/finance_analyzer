#ifndef FINANCE_ANALYZER_STOCK_MGR_H
#define FINANCE_ANALYZER_STOCK_MGR_H

#include "finance_analyzer_mgr_base.h"


class FinanceAnalyzerStockMgr : public FinanceAnalyzerMgrBase
{
private:

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerStockMgr();
	~FinanceAnalyzerStockMgr();

	unsigned short initialize();
};

#endif
