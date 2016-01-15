#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_calculator.h"
#include "finance_analyzer_algorithm.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerCalculator::FinanceAnalyzerCalculator()
{
	IMPLEMENT_MSG_DUMPER()
//	IMPLEMENT_DATABASE_TIME_RANGE()
}

FinanceAnalyzerCalculator::~FinanceAnalyzerCalculator()
{
//	RELEASE_DATABASE_TIME_RANGE()
	RELEASE_MSG_DUMPER()
}

unsigned short correlate(const PRESULT_SET result_set, float& correlation_coeffient)
{
	assert(result_set != NULL && "result_set should NOT be NULL");

	return RET_SUCCESS;
}
