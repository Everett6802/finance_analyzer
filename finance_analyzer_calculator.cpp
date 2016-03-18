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

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, ArrayElementCalculationType calculation_type1, FinanceSourceType finance_source_type2, int finance_field_no2, ArrayElementCalculationType calculation_type2, int start_index1, int start_index2, int end_index1, int end_index2, float& correlation_value)
{
	assert(result_set != NULL && "result_set should NOT be NULL");
// Get the array
	WRITE_FORMAT_DEBUG("Find the correlation between [%s,%d,%s] and [%s,%d,%s]", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2]);
	unsigned short ret = RET_SUCCESS;
	PFINANCE_DATA_ARRAY_BASE finance_data_array1 = result_set->get_array(finance_source_type1, finance_field_no1, calculation_type1);
	assert(finance_data_array1 != NULL && "finance_data_array1 should NOT be NULL");
	PFINANCE_DATA_ARRAY_BASE finance_data_array2 = result_set->get_array(finance_source_type2, finance_field_no2, calculation_type2);
	assert(finance_data_array2 != NULL && "finance_data_array2 should NOT be NULL");
// Check the end index
	if (end_index1 < 0)
		end_index1 = get_end_index_ex(end_index1, finance_data_array1->get_size()) + 1;
	if (end_index2 < 0)
		end_index2 = get_end_index_ex(end_index2, finance_data_array2->get_size()) + 1;
	INDEX_IN_RANGE(start_index1, end_index1, 0, finance_data_array1->get_size());
	INDEX_IN_RANGE(start_index2, end_index2, 0, finance_data_array2->get_size());
// Check the data length
	int finance_data_len1 = end_index1 - start_index1;
	int finance_data_len2 = end_index2 - start_index2;
	WRITE_FORMAT_DEBUG("Array1 start_index: %d, end_index: %d, length: %d", start_index1, end_index1, finance_data_len1);
	WRITE_FORMAT_DEBUG("Array2 start_index: %d, end_index: %d, length: %d", start_index2, end_index2, finance_data_len2);
	assert(finance_data_len1 == finance_data_len2 && "The lengths of two data array are NOT identical");
// Get the correlation
	correlation_value = correlation(finance_data_array1, finance_data_array2, start_index1, start_index2, end_index1, end_index2);
	WRITE_FORMAT_INFO("The correlation between [%s,%d,%s] and [%s,%d,%s]: %.2f", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2], correlation_value);
// Update the latest detailed info
	if (need_res_info)
	{
		snprintf(res_info, RES_INFO_LENGTH, "[%s:%d:%s %s/%s] [%s:%d:%s %s/%s] %d",
			FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1],
			finance_field_no1,
			FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1],
			result_set->get_date_array_element(start_index1),
			result_set->get_date_array_element(end_index1 - 1),
			FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2],
			finance_field_no2,
			FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2],
			result_set->get_date_array_element(start_index2),
			result_set->get_date_array_element(end_index2 - 1),
			finance_data_len1
		);
		WRITE_FORMAT_DEBUG("Correlation Detail: %s", res_info);
	}
	return ret;
}

unsigned short FinanceAnalyzerCalculator::correlate(const PRESULT_SET result_set, const SmartPointer<ResultSetAccessParam>& access_param1, const SmartPointer<ResultSetAccessParam>& access_param2, float& correlation_value)
{
// Get each parameter
	FinanceSourceType finance_source_type1 = access_param1->get_finance_source_type(), finance_source_type2 = access_param2->get_finance_source_type();
	int finance_field_no1 = access_param1->get_finance_field_no(), finance_field_no2 = access_param2->get_finance_field_no();
	ArrayElementCalculationType calculation_type1 = access_param1->get_calculation_type(), calculation_type2 = access_param2->get_calculation_type();
	int start_index1 = access_param1->get_start_index(), start_index2 = access_param2->get_start_index();
	int end_index1 = access_param1->get_end_index(), end_index2 = access_param2->get_end_index();

	return correlate(result_set, finance_source_type1, finance_field_no1, calculation_type1, finance_source_type2, finance_field_no2, calculation_type2, start_index1, start_index2, end_index1, end_index2, correlation_value);
}

unsigned short FinanceAnalyzerCalculator::correlate_auto_alignment(const PRESULT_SET result_set, const SmartPointer<ResultSetAccessParam>& access_param1, const SmartPointer<ResultSetAccessParam>& access_param2, float& correlation_value)
{
// Auto Alignment: Modify the end index of the larger array
	assert(result_set != NULL && "result_set should NOT be NULL");
// Get each parameter
	FinanceSourceType finance_source_type1 = access_param1->get_finance_source_type(), finance_source_type2 = access_param2->get_finance_source_type();
	int finance_field_no1 = access_param1->get_finance_field_no(), finance_field_no2 = access_param2->get_finance_field_no();
	ArrayElementCalculationType calculation_type1 = access_param1->get_calculation_type(), calculation_type2 = access_param2->get_calculation_type();
	int start_index1 = access_param1->get_start_index(), start_index2 = access_param2->get_start_index();

	PFINANCE_DATA_ARRAY_BASE finance_data_array1 = result_set->get_array(finance_source_type1, finance_field_no1, calculation_type1);
	assert(finance_data_array1 != NULL && "finance_data_array1 should NOT be NULL");
	PFINANCE_DATA_ARRAY_BASE finance_data_array2 = result_set->get_array(finance_source_type2, finance_field_no2, calculation_type2);
	assert(finance_data_array2 != NULL && "finance_data_array2 should NOT be NULL");
// Check if the original data size is identical
	int end_index1 = finance_data_array1->get_size();
	int end_index2 = finance_data_array2->get_size();
	int finance_data_array_len1 = finance_data_array1->get_size() - start_index1;
	int finance_data_array_len2 = finance_data_array2->get_size() - start_index2;
// Do array calculation. The size of the array is changed
	if (finance_data_array_len1 > finance_data_array_len2)
	{
		int data_aray_size_diff = finance_data_array_len1 - finance_data_array_len2;
//		start_index1 += data_aray_size_diff;
//		WRITE_FORMAT_DEBUG("Modify the start index1 of the array [%s,%d,%s]: %d", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], start_index1);
		end_index1 -= data_aray_size_diff;
		WRITE_FORMAT_DEBUG("Modify the end index1 of the array [%s,%d,%s]: %d", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], end_index1);
	}
	else if (finance_data_array_len2 > finance_data_array_len1)
	{
		int data_aray_size_diff = finance_data_array_len2 - finance_data_array_len1;
//		start_index2 += data_aray_size_diff;
//		WRITE_FORMAT_DEBUG("Modify the start index2 of the array [%s,%d,%s]: %d", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type2], finance_field_no2, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type2], start_index2);
		end_index2 -= data_aray_size_diff;
		WRITE_FORMAT_DEBUG("Modify the end index2 of the array [%s,%d,%s]: %d", FINANCE_DATABASE_DESCRIPTION_LIST[finance_source_type1], finance_field_no1, FINANCE_ARRAY_ELEMENT_CALCULATION_DESCRIPTION_LIST[calculation_type1], end_index2);
	}

	return correlate(result_set, finance_source_type1, finance_field_no1, calculation_type1, finance_source_type2, finance_field_no2, calculation_type2, start_index1, start_index2, end_index1, end_index2, correlation_value);
}
