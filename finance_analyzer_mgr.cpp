#include <stdexcept>
#include <string>
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_sql_reader.h"


using namespace std;

FinanceAnalyzerMgr::FinanceAnalyzerMgr() :
	finance_analyzer_sql_reader(NULL)
{
	finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
	if (finance_analyzer_sql_reader == NULL)
		throw bad_alloc();
}

FinanceAnalyzerMgr::~FinanceAnalyzerMgr()
{
	if (finance_analyzer_sql_reader != NULL)
	{
		delete finance_analyzer_sql_reader;
		finance_analyzer_sql_reader = NULL;
	}
}

unsigned short FinanceAnalyzerMgr::test()
{
	string database_name = FINANCE_DATABASE_NAME_LIST[0];

	unsigned short ret = RET_SUCCESS;
	ret = finance_analyzer_sql_reader->try_connect_mysql(database_name);
	if (CHECK_FAILURE(ret))
		return ret;

	ret = finance_analyzer_sql_reader->select_data("year2014");
	if (CHECK_FAILURE(ret))
		return ret;

	ret = finance_analyzer_sql_reader->disconnect_mysql();
	if (CHECK_FAILURE(ret))
		return ret;

	return ret;
}

