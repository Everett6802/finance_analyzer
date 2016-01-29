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
	FinanceSourceType finance_source_type1 = (FinanceSourceType)ResultSet::get_upper_subindex(key[0]);
	int field_index1 = ResultSet::get_lower_subindex(key[0]);
	FinanceSourceType finance_source_type2 = (FinanceSourceType)ResultSet::get_upper_subindex(key[1]);
	int field_index2 = ResultSet::get_lower_subindex(key[1]);

//	WRITE_FORMAT_DEBUG("Find the correlation between [%s,%d] and [%s,%d]", FINANCE_DATABASE_DESCRIPTION_LIST[source_index1], field_index1, FINANCE_DATABASE_DESCRIPTION_LIST[source_index2], field_index2);
//	unsigned short ret = RET_SUCCESS;
//	correlation_value = correlation(result_set->get_array(source_index1, field_index1), result_set->get_array(source_index2, field_index2));
//	return ret;
	return correlate(result_set, finance_source_type1, field_index1, finance_source_type2, field_index2, correlation_value);
}

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, ArrayElementCalculationType calculation_type1, FinanceSourceType finance_source_type2, int finance_field_no2, ArrayElementCalculationType calculation_type2, float& correlation_value)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
	assert(result_set->get_data_dimension() == 2 && "The dimension of result_set should be 2");

	WRITE_FORMAT_DEBUG("Find the correlation between [%s,%d,%s] and [%s,%d,%s]", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2]);
	unsigned short ret = RET_SUCCESS;
	correlation_value = correlation(result_set->get_array(finance_source_type1, finance_field_no1, calculation_type1), result_set->get_array(finance_source_type2, finance_field_no2, calculation_type2));
	return ret;
}

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, FinanceSourceType finance_source_type2, int finance_field_no2, float& correlation_value)
{
	return correlate(result_set, finance_source_type1, finance_field_no1, ArrayElementCalculation_None, finance_source_type2, finance_field_no2, ArrayElementCalculation_None, correlation_value);
}



