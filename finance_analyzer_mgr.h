#ifndef FINANCE_ANALYZER_MGR_H
#define FINANCE_ANALYZER_MGR_H


class FinanceAnalyzerSqlReader;

class FinanceAnalyzerMgr
{
private:
	FinanceAnalyzerSqlReader* finance_analyzer_sql_reader;

public:
	FinanceAnalyzerMgr();
	~FinanceAnalyzerMgr();

	unsigned short test();
};

#endif
