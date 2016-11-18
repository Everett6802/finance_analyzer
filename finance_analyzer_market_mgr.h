#ifndef FINANCE_ANALYZER_MARKET_MGR_H
#define FINANCE_ANALYZER_MARKET_MGR_H

#include "finance_analyzer_mgr_base.h"


class FinanceAnalyzerMarketMgr : public FinanceAnalyzerMgrBase
{
private:

public:
	static IFinanceAnalyzerMgr* create_instance();

	FinanceAnalyzerMarketMgr();
	~FinanceAnalyzerMarketMgr();

	unsigned short initialize();
};

#endif
