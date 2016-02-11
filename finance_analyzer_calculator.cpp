#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_calculator.h"
#include "finance_analyzer_algorithm.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerCalculator::FinanceAnalyzerCalculator() :
	need_res_info(true)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_DATABASE_TIME_RANGE()
	res_info[0] = '\0';
}

FinanceAnalyzerCalculator::~FinanceAnalyzerCalculator()
{
	RELEASE_DATABASE_TIME_RANGE()
	RELEASE_MSG_DUMPER()
}

void FinanceAnalyzerCalculator::enable_res_info(bool enable)
{
	need_res_info = enable;
}

const char* FinanceAnalyzerCalculator::get_last_res_info()const
{
	return res_info;
}

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, ArrayElementCalculationType calculation_type1, FinanceSourceType finance_source_type2, int finance_field_no2, ArrayElementCalculationType calculation_type2, float& correlation_value)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
//	assert(result_set->get_data_dimension() == 2 && "The dimension of result_set should be 2");

	WRITE_FORMAT_DEBUG("Find the correlation between [%s,%d,%s] and [%s,%d,%s]", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2]);
	unsigned short ret = RET_SUCCESS;
	PFINANCE_DATA_ARRAY_BASE finance_data_array1 = result_set->get_array(finance_source_type1, finance_field_no1);
	assert(finance_data_array1 != NULL && "finance_data_array1 should NOT be NULL");
	PFINANCE_DATA_ARRAY_BASE finance_data_array2 = result_set->get_array(finance_source_type2, finance_field_no2);
	assert(finance_data_array2 != NULL && "finance_data_array2 should NOT be NULL");
// Check if the original data size is identical
	assert(finance_data_array1->get_size() == finance_data_array2->get_size() && "The array size of finance_data_array1 and finance_data_array2 is NOT identical");

	int start_index1 = 0, start_index2 = 0;
	if (calculation_type1 != ArrayElementCalculation_None || calculation_type2 != ArrayElementCalculation_None)
	{
// Do array calculation. The size of the array is changed
		if (calculation_type1 != ArrayElementCalculation_None)
			finance_data_array1 = result_set->get_array(finance_source_type1, finance_field_no1, calculation_type1);
		if (calculation_type2 != ArrayElementCalculation_None)
			finance_data_array2 = result_set->get_array(finance_source_type2, finance_field_no2, calculation_type2);
		int finance_data_array_size1 = finance_data_array1->get_size();
		int finance_data_array_size2 = finance_data_array2->get_size();
		if (finance_data_array_size1 > finance_data_array_size2)
		{
			int data_aray_size_diff = finance_data_array_size1 - finance_data_array_size2;
			start_index1 = data_aray_size_diff;
			WRITE_FORMAT_DEBUG("Modify the start index of the array [%s,%d,%s]: %d", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], start_index1);
		}
		else if (finance_data_array_size2 > finance_data_array_size1)
		{
			int data_aray_size_diff = finance_data_array_size2 - finance_data_array_size1;
			start_index2 = data_aray_size_diff;
			WRITE_FORMAT_DEBUG("Modify the start index of the array [%s,%d,%s]: %d", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2], start_index2);
		}
	}
	correlation_value = correlation(finance_data_array1, finance_data_array2, start_index1, start_index2);
	WRITE_FORMAT_INFO("The correlation between [%s,%d,%s] and [%s,%d,%s]: %.2f", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2], correlation_value);

	if (need_res_info)
	{
		int res_info_start_index = start_index1 > start_index2 ? start_index1 : start_index2;
		int res_info_end_index = get_end_index_ex(-1, result_set->get_data_size());
		snprintf(res_info, RES_INFO_LENGTH, "[%s:%d:%s] and [%s:%d:%s] [%s %s] %d",
			FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1],
			finance_field_no1,
			FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1],
			FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2],
			finance_field_no2,
			FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2],
			result_set->get_date_array_element(res_info_start_index),
			result_set->get_date_array_element(res_info_end_index - 1),
			res_info_end_index - res_info_start_index
		);
		WRITE_FORMAT_DEBUG("Correlation Detail: %s", res_info);
	}
	return ret;
}

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, FinanceSourceType finance_source_type2, int finance_field_no2, float& correlation_value)
{
	return correlate(result_set, finance_source_type1, finance_field_no1, ArrayElementCalculation_None, finance_source_type2, finance_field_no2, ArrayElementCalculation_None, correlation_value);
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
