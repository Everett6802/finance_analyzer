#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_calculator.h"
#include "finance_analyzer_algorithm.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerCalculator::FinanceAnalyzerCalculator()
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_DATABASE_TIME_RANGE()
}

FinanceAnalyzerCalculator::~FinanceAnalyzerCalculator()
{
	RELEASE_DATABASE_TIME_RANGE()
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, float& correlation_value)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(result_set->get_data_dimension() == 2 && "The dimension of result_set should be 2");

	unsigned short key[2];
//	unsigned short value[2];
	int pos = 0;
	ResultSet::iterator iter = result_set->begin();
	ResultSet::iterator iter_end = result_set->end();
	while (iter != iter_end)
	{
		key[pos] = iter->first;
//		value[pos] = iter->second;
//		printf("key: %d, value: %d\n", iter->first, iter->second);
		iter++;
		pos++;
	}
	int source_index1 = ResultSet::get_upper_subindex(key[0]);
	int field_index1 = ResultSet::get_lower_subindex(key[0]);
	int source_index2 = ResultSet::get_upper_subindex(key[1]);
	int field_index2 = ResultSet::get_lower_subindex(key[1]);

//	correlation_value = correlation();
	return RET_SUCCESS;
}
