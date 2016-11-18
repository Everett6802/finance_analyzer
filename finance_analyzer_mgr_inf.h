#ifndef FINANCE_ANALYZER_MGR_INF_H
#define FINANCE_ANALYZER_MGR_INF_H


class IFinanceAnalyzerMgr
{
private:
public:
	static IFinanceAnalyzerMgr* create_instance();

	virtual unsigned short initialize()=0;
};
typedef IFinanceAnalyzerMgr* PIFINANCE_ANALYZER_MGR;

#endif
