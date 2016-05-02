#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_graph_table_statistics.h"
#include "finance_analyzer_mgr.h"
#include "finance_analyzer_algorithm.h"
#include "finance_analyzer_sql_reader.h"
#include "finance_analyzer_output.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()
static const int RES_INFO_LENGTH = 1024;
static char res_info[RES_INFO_LENGTH];

FinanceAnalyzerGraphTableStatistics::FinanceAnalyzerGraphTableStatistics() :
	need_res_info(true)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_WORKDAY_CANLENDAR()
	IMPLEMENT_DATABASE_TIME_RANGE()
	res_info[0] = '\0';
	finance_analyzer_sql_reader = new FinanceAnalyzerSqlReader();
	if (finance_analyzer_sql_reader == NULL)
	{
		assert(false && "Fail to allocate memory: finance_analyzer_sql_reader");
		throw bad_alloc(); 
	}
}

FinanceAnalyzerGraphTableStatistics::~FinanceAnalyzerGraphTableStatistics()
{
	if (finance_analyzer_sql_reader != NULL)
	{
		delete finance_analyzer_sql_reader;
		finance_analyzer_sql_reader = NULL;
	}
	RELEASE_DATABASE_TIME_RANGE()
	RELEASE_WORKDAY_CANLENDAR()
	RELEASE_MSG_DUMPER()
}

void FinanceAnalyzerGraphTableStatistics::enable_res_info(bool enable)
{
	need_res_info = enable;
}

const char* FinanceAnalyzerGraphTableStatistics::get_last_res_info()const
{
	return res_info;
}
