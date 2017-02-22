#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_test.h"
// #include "finance_analyzer_data_statistics.h"
#include "finance_analyzer_algorithm.h"
#include "finance_analyzer_filter.h"
#include "finance_analyzer_output.h"


using namespace std;

//DECLARE_MSG_DUMPER_PARAM()

char* TEST_TYPE_DESCRIPTION[TestTypeSize] = 
{
	"Check Array",
	"Check Array Statistics",
	"Check Filter Array",
	"Check Filter Rule",
	"Check Formula",
	"Check Filter Formula",
	"Check Histogram"
};

FinanceAnalyzerTest::FinanceAnalyzerTest() :
		show_test_case_detail(false)
{
//	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerTest::~FinanceAnalyzerTest()
{
//	RELEASE_MSG_DUMPER()
}

bool FinanceAnalyzerTest::check_float_value_equal(float expected_value, float actual_value, bool throw_exception)
{
	static const float TOLERANCE = 0.01f;
//	static const int VALUE_SIZE = 16;
//	static char expected_value_str[VALUE_SIZE];
//	static char actual_value_str[VALUE_SIZE];
//	snprintf(expected_value_str, VALUE_SIZE, "%.2f", expected_value);
//	snprintf(actual_value_str, VALUE_SIZE, "%.2f", actual_value);
//	if (strcmp(expected_value_str, actual_value_str) != 0)
	float abs_diff = expected_value - actual_value;
	if(abs_diff < 0)
		abs_diff = -abs_diff;
	if (abs_diff >= TOLERANCE)
	{
		static const int ERRMSG_SIZE = 64;
		static char errmsg[ERRMSG_SIZE];
//		snprintf(errmsg, ERRMSG_SIZE, "Value is NOT equal; Expected: %s, Actual: %s", expected_value_str, actual_value_str);
		snprintf(errmsg, ERRMSG_SIZE, "Value is NOT equal; Expected: %.2f, Actual: %.2f", expected_value, actual_value);
		if (throw_exception)
			throw runtime_error(errmsg);
		return false;
	}
	return true;
}

void FinanceAnalyzerTest::set_show_detail(bool show_detail)
{
	show_test_case_detail = show_detail;
}

void FinanceAnalyzerTest::test_check_array()
{
	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];

	ResultSet result_set;
	unsigned short ret = RET_SUCCESS;
	SmartPointer<FinanceIntDataArray> sp_int_data_array(new FinanceIntDataArray());
	SmartPointer<FinanceLongDataArray> sp_long_data_array(new FinanceLongDataArray());
	SmartPointer<FinanceFloatDataArray> sp_float_data_array(new FinanceFloatDataArray());
	sp_int_data_array->set_type(FinanceField_INT);
	sp_long_data_array->set_type(FinanceField_LONG);
	sp_float_data_array->set_type(FinanceField_FLOAT);
// Generate data
	ResultSet::generate_data_for_simulation(result_set);
// Check Avg/Diff array
//	if (show_test_case_detail) cout << "2 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff) << endl;
//	if (show_test_case_detail) cout << "2 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5) << endl;
//	if (show_test_case_detail) cout << "2 Avg10: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg10) << endl;
//	if (show_test_case_detail) cout << "4 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff) << endl;
//	if (show_test_case_detail) cout << "4 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5) << endl;
	// result_set.show_data_mapping();

	PFINANCE_LONG_DATA_ARRAY data_array1 = (PFINANCE_LONG_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 1);
	ret = data_array1->get_diff_array(*sp_long_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 Diff], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 Diff: " << *sp_long_data_array << endl;
	static const long ARRAY1_DIFF[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	if (*sp_long_data_array != ARRAY1_DIFF)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Diff] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_long_data_array->reset_array();

	ret = data_array1->get_rate_array(*sp_float_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 Rate], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 Rate: " << *sp_float_data_array << endl;
	static const float ARRAY1_RATE[] = {1.0, 0.5, 0.33, 0.25, 0.2, 0.17, 0.14, 0.13, 0.11};
	if (*sp_float_data_array != ARRAY1_RATE)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Rate] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array1->get_sum_array(*sp_long_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 Sum5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 Sum5: " << *sp_long_data_array << endl;
	static const long ARRAY1_SUM5[] = {15, 20, 25, 30, 35, 40};
	if (*sp_long_data_array != ARRAY1_SUM5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Sum5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_long_data_array->reset_array();

	ret = data_array1->get_avg_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 Avg5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 Avg5: " << *sp_float_data_array << endl;
	static const float ARRAY1_AVG5[] = {3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
	if (*sp_float_data_array != ARRAY1_AVG5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Avg5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array1->get_weighted_avg_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 WAvg5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 WAvg5: " << *sp_float_data_array << endl;
	static const float ARRAY1_WAVG5[] = {3.56, 4.56, 5.56, 6.56, 7.56, 8.56};
	if (*sp_float_data_array != ARRAY1_WAVG5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 WAvg5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array1->get_weighted_avg_array(*sp_float_data_array, 10, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 WAvg10], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 WAvg10: " << *sp_float_data_array << endl;
	static const float ARRAY1_WAVG10[] = {6.61};
	if (*sp_float_data_array != ARRAY1_WAVG10)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 WAvg10] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PFINANCE_INT_DATA_ARRAY data_array3 = (PFINANCE_INT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 3);
	ret = data_array3->get_diff_array(*sp_int_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 Diff], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 Diff: " << *sp_int_data_array << endl;
	static const int ARRAY3_DIFF[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
	if (*sp_int_data_array != ARRAY3_DIFF)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Diff] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_int_data_array->reset_array();

	ret = data_array3->get_rate_array(*sp_float_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 Rate], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 Rate: " << *sp_float_data_array << endl;
	static const float ARRAY3_RATE[] = {1.0, 0.5, 0.33, 0.25, 0.2, 0.17, 0.14, 0.13, 0.11};
	if (*sp_float_data_array != ARRAY3_RATE)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Rate] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array3->get_sum_array(*sp_int_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 Sum5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 Sum5: " << *sp_int_data_array << endl;
	static const int ARRAY3_SUM5[] = {-15, -20, -25, -30, -35, -40};
	if (*sp_int_data_array != ARRAY3_SUM5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Sum5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_int_data_array->reset_array();

	ret = data_array3->get_avg_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 Avg5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 Avg5: " << *sp_float_data_array << endl;
	static const float ARRAY3_AVG5[] = {-3.0, -4.0, -5.0, -6.0, -7.0, -8.0};
	if (*sp_float_data_array != ARRAY3_AVG5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Avg5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array3->get_weighted_avg_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 WAvg5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 WAvg5: " << *sp_float_data_array << endl;
	static const float ARRAY3_WAVG5[] = {-3.56, -4.56, -5.56, -6.56, -7.56, -8.56};
	if (*sp_float_data_array != ARRAY3_WAVG5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 WAvg5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array3->get_weighted_avg_array(*sp_float_data_array, 10, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 WAvg10], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 WAvg10: " << *sp_float_data_array << endl;
	static const float ARRAY3_WAVG10[] = {-6.61};
	if (*sp_float_data_array != ARRAY3_WAVG10)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 WAvg10] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PFINANCE_FLOAT_DATA_ARRAY data_array4 = (PFINANCE_FLOAT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 4);
	ret = data_array4->get_diff_array(*sp_float_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 Sum5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 Diff: " << *sp_float_data_array << endl;
	static const float ARRAY4_DIFF[] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
	if (*sp_float_data_array != ARRAY4_DIFF)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Diff] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array4->get_rate_array(*sp_float_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 Rate], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 Rate: " << *sp_float_data_array << endl;
	static const float ARRAY4_RATE[] = {0.99, 0.5, 0.33, 0.25, 0.2, 0.17, 0.14, 0.12, 0.11};
	if (*sp_float_data_array != ARRAY4_RATE)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Rate] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array4->get_sum_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 Sum5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 Sum5: " << *sp_float_data_array << endl;
	static const float ARRAY4_SUM5[] = {150.5, 200.5, 250.5, 300.5, 350.5, 400.5};
	if (*sp_float_data_array != ARRAY4_SUM5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Sum5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array4->get_avg_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 Avg5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 Avg5: " << *sp_float_data_array << endl;
	static const float ARRAY4_AVG5[] = {30.1, 40.1, 50.1, 60.1, 70.1, 80.1};
	if (*sp_float_data_array != ARRAY4_AVG5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Avg5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array4->get_weighted_avg_array(*sp_float_data_array, 5, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 WAvg5], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 WAvg5: " << *sp_float_data_array << endl;
	static const float ARRAY4_WAVG5[] = {35.66, 45.66, 55.66, 65.66, 75.66, 85.66};
	if (*sp_float_data_array != ARRAY4_WAVG5)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 WAvg5] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();

	ret = data_array4->get_weighted_avg_array(*sp_float_data_array, 10, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 WAvg10], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 WAvg10: " << *sp_float_data_array << endl;
	static const float ARRAY4_WAVG10[] = {66.21};
	if (*sp_float_data_array != ARRAY4_WAVG10)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 WAvg10] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();
}

void FinanceAnalyzerTest::test_check_array_statistics()
{
	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];

	ResultSet result_set;
	unsigned short ret = RET_SUCCESS;
	SmartPointer<FinanceIntDataArray> sp_int_data_array(new FinanceIntDataArray());
	SmartPointer<FinanceLongDataArray> sp_long_data_array(new FinanceLongDataArray());
	SmartPointer<FinanceFloatDataArray> sp_float_data_array(new FinanceFloatDataArray());
	sp_int_data_array->set_type(FinanceField_INT);
	sp_long_data_array->set_type(FinanceField_LONG);
	sp_float_data_array->set_type(FinanceField_FLOAT);
// Generate data
	ResultSet::generate_data_for_simulation(result_set);
// Check Avg/Diff array
//	if (show_test_case_detail) cout << "2 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff) << endl;
//	if (show_test_case_detail) cout << "2 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5) << endl;
//	if (show_test_case_detail) cout << "2 Avg10: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg10) << endl;
//	if (show_test_case_detail) cout << "4 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff) << endl;
//	if (show_test_case_detail) cout << "4 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5) << endl;

	PFINANCE_LONG_DATA_ARRAY data_array1 = (PFINANCE_LONG_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 1);
	long data_array1_min, data_array1_max;
	ret = get_data_range(*data_array1, data_array1_min, data_array1_max);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 Range], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 Min: " << data_array1_min << ", 1 Max: " << data_array1_max << endl;
	static const long ARRAY1_MIN = 1;
	if (data_array1_min != ARRAY1_MIN)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Min] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	static const long ARRAY1_MAX = 10;
	if (data_array1_max != ARRAY1_MAX)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Max] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_long_data_array->reset_array();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PFINANCE_INT_DATA_ARRAY data_array3 = (PFINANCE_INT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 3);
	int data_array3_min, data_array3_max;
	ret = get_data_range(*data_array3, data_array3_min, data_array3_max);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 Range], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 Min: " << data_array3_min << ", 3 Max: " << data_array3_max << endl;
	static const long ARRAY3_MIN = -10;
	if (data_array3_min != ARRAY3_MIN)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Min] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	static const long ARRAY3_MAX = -1;
	if (data_array3_max != ARRAY3_MAX)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Max] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_int_data_array->reset_array();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PFINANCE_FLOAT_DATA_ARRAY data_array4 = (PFINANCE_FLOAT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 4);
	float data_array4_min, data_array4_max;
	ret = get_data_range(*data_array4, data_array4_min, data_array4_max);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 Range], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 Min: " << data_array4_min << ", 4 Max: " << data_array4_max << endl;
	static const float ARRAY4_MIN = 10.1f;
	if (!check_float_value_equal(data_array4_min, ARRAY4_MIN))
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Min] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	static const float ARRAY4_MAX = 100.1f;
	if (!check_float_value_equal(data_array4_max, ARRAY4_MAX))
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Max] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();
}

void FinanceAnalyzerTest::test_check_filter_array()
{
	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];

	ResultSet result_set;
	FinanceBoolDataArray filter_data_array;
	unsigned short ret = RET_SUCCESS;
	SmartPointer<FinanceIntDataArray> sp_int_data_array(new FinanceIntDataArray());
	SmartPointer<FinanceLongDataArray> sp_long_data_array(new FinanceLongDataArray());
	SmartPointer<FinanceFloatDataArray> sp_float_data_array(new FinanceFloatDataArray());
	sp_int_data_array->set_type(FinanceField_INT);
	sp_long_data_array->set_type(FinanceField_LONG);
	sp_float_data_array->set_type(FinanceField_FLOAT);
// Generate data
	ResultSet::generate_filtered_data_for_simulation(result_set, filter_data_array);

	PFINANCE_LONG_DATA_ARRAY data_array1 = (PFINANCE_LONG_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 1);
	ret = data_array1->get_sub_array(*sp_long_data_array, &filter_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [1 Sub], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "1 Sub: " << *sp_long_data_array << endl;
	static const long ARRAY1_SUB[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	if (*sp_long_data_array != ARRAY1_SUB)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[1 Sub] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_long_data_array->reset_array();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PFINANCE_INT_DATA_ARRAY data_array3 = (PFINANCE_INT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 3);
	ret = data_array3->get_sub_array(*sp_int_data_array, &filter_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 Sub], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 Sub: " << *sp_int_data_array << endl;
	static const int ARRAY3_SUB[] = {-1, -2, -3, -4, -5, -6, -7, -8, -9, -10};
	if (*sp_int_data_array != ARRAY3_SUB)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 Sub] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_int_data_array->reset_array();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PFINANCE_FLOAT_DATA_ARRAY data_array4 = (PFINANCE_FLOAT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 4);
	ret = data_array4->get_sub_array(*sp_float_data_array, &filter_data_array, 0);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [4 Sub], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "4 Sub: " << *sp_float_data_array << endl;
	static const float ARRAY4_SUB[] = {10.1, 20.1, 30.1, 40.1, 50.1, 60.1, 70.1, 80.1, 90.1, 100.1};
	if (*sp_float_data_array != ARRAY4_SUB)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[4 Sub] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	sp_float_data_array->reset_array();
}

void FinanceAnalyzerTest::test_check_filter_rule()
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15"};
	static const char* field1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}; // Long
	static const char* field2[] = {"-1", "2", "-3", "4", "-5", "6", "-7", "8", "-9", "10"}; // Long
	static const char* field3[] = {"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10"}; // Int
	static const char* field4[] = {"10.1", "20.1", "30.1", "40.1", "50.1", "60.1", "70.1", "80.1", "90.1", "100.1"}; // Float
	static const char* field5[] = {"1.1", "21.2", "122.3", "199.4", "200.5", "300.6", "420.7", "435.8", "599.4", "600.0"}; // Float

	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];
	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

	FinanceBoolDataArray filter_data_array;

	unsigned short ret = RET_SUCCESS;
	ResultSet result_set;
	for (int i = 1 ; i < 6 ; i++)
		result_set.add_set(FinanceSource_StockExchangeAndVolume, i);
	char data[32];
	for (int i = 0 ; i < DATA_SIZE ; i++)
	{
		snprintf(data, 32, "%s", date[i]);
		result_set.set_date(data);
		snprintf(data, 32, "%s", field1[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 1, data);
		snprintf(data, 32, "%s", field2[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 2, data);
		snprintf(data, 32, "%s", field3[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 3, data);
		snprintf(data, 32, "%s", field4[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 4, data);
		snprintf(data, 32, "%s", field5[i]);
		result_set.set_data(FinanceSource_StockExchangeAndVolume, 5, data);
//		filter_data_array.add(filter[i]);
	}

	SmartPointer<FinanceIntDataArray> sp_int_data_array(new FinanceIntDataArray());
	SmartPointer<FinanceLongDataArray> sp_long_data_array(new FinanceLongDataArray());
	SmartPointer<FinanceFloatDataArray> sp_float_data_array(new FinanceFloatDataArray());
	sp_int_data_array->set_type(FinanceField_INT);
	sp_long_data_array->set_type(FinanceField_LONG);
	sp_float_data_array->set_type(FinanceField_FLOAT);

	ResultSetAccessParamDeque result_set_access_param_deque;
	FilterRuleThresholdDeque filter_rule_threshold_deque;
// Add the filter rule
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 1), new FilterRuleThresholdLong(FilterRule_GreaterThan, 3));
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 3), new FilterRuleThresholdRangeInt(FilterRule_OutOfRange_LCRC, -5, 5));
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 5), new FilterRuleThresholdRangeFloat(FilterRule_InRange_LCRO, 100.0, 600.0));
	if (show_test_case_detail) show_filter_rule(result_set_access_param_deque, filter_rule_threshold_deque);
	ret = filter_and(&result_set, &result_set_access_param_deque, &filter_rule_threshold_deque, filter_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array[Rule1 AND], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "Rule1 AND Result: " << filter_data_array << "; Probability: " << filter_data_array.get_probability() << endl;
	static const bool ARRAY_RULE1_AND_RESULT[] = {false, false, false, false, true, true, true, true, true, false};
	if (filter_data_array != ARRAY_RULE1_AND_RESULT)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[Rule1 AND] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	filter_data_array.reset_array();
	ret = filter_or(&result_set, &result_set_access_param_deque, &filter_rule_threshold_deque, filter_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array[Rule1 OR], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "Rule1 OR Result: " << filter_data_array << "; Probability: " << filter_data_array.get_probability() << endl;
	static const bool ARRAY_RULE1_OR_RESULT[] = {false, false, true, true, true, true, true, true, true, true};
	if (filter_data_array != ARRAY_RULE1_OR_RESULT)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[Rule1 OR] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	filter_data_array.reset_array();
	cleanup_filter_rule(result_set_access_param_deque, filter_rule_threshold_deque);

// Add the filter rule
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 1), new FilterRuleThresholdLong(FilterRule_LessThan, 9));
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 2), new FilterRuleThresholdLong(FilterRule_NotEqual, -5));
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 3), new FilterRuleThresholdRangeInt(FilterRule_OutOfRange_LORO, -5, 5));
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 4), new FilterRuleThresholdRangeFloat(FilterRule_InRange_LORC, 40.1, 90.1));
	add_filter_rule(&result_set_access_param_deque, &filter_rule_threshold_deque, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 5), new FilterRuleThresholdRangeFloat(FilterRule_InRange_LORO, 200.0, 600.0));
	if (show_test_case_detail) show_filter_rule(result_set_access_param_deque, filter_rule_threshold_deque);
	ret = filter_and(&result_set, &result_set_access_param_deque, &filter_rule_threshold_deque, filter_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array[Rule2 AND], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "Rule2 AND Result: " << filter_data_array << "; Probability: " << filter_data_array.get_probability() << endl;
	static const bool ARRAY_RULE2_AND_RESULT[] = {false, false, false, false, false, true, true, true, false, false};
	if (filter_data_array != ARRAY_RULE2_AND_RESULT)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[Rule2 AND] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	filter_data_array.reset_array();
	ret = filter_or(&result_set, &result_set_access_param_deque, &filter_rule_threshold_deque, filter_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array[Rule2 OR], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "Rule2 OR Result: " << filter_data_array << "; Probability: " << filter_data_array.get_probability() << endl;
	static const bool ARRAY_RULE2_OR_RESULT[] = {true, true, true, true, true, true, true, true, true, true};
	if (filter_data_array != ARRAY_RULE2_OR_RESULT)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[Rule2 OR] is NOT correct");
		throw runtime_error(string(errmsg));
	}
	filter_data_array.reset_array();
	cleanup_filter_rule(result_set_access_param_deque, filter_rule_threshold_deque);
}

void FinanceAnalyzerTest::test_check_formula()
{
	ResultSet result_set;
// Generate data
	ResultSet::generate_data_for_simulation(result_set);

	float value;
// Check Formula
//// Show array
//	if (show_test_case_detail) cout << "1: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 1) << endl;
//	if (show_test_case_detail) cout << "2: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2) << endl;
// Check average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1));
	check_float_value_equal(5.5, value);
	if (show_test_case_detail) printf("[1] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1));
	check_float_value_equal(8.25, value);
	if (show_test_case_detail) printf("[1] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1));
	check_float_value_equal(2.87, value);
	if (show_test_case_detail) printf("[1] standard deviation: %.2f\n", value);
// Check average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2));
	check_float_value_equal(0.5, value);
	if (show_test_case_detail) printf("[2] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2));
	check_float_value_equal(38.25, value);
	if (show_test_case_detail) printf("[2] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2));
	check_float_value_equal(6.18, value);
	if (show_test_case_detail) printf("[2] standard deviation: %.2f\n", value);
// Check covariance, correlation
	value = covariance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2));
	check_float_value_equal(2.75, value);
	if (show_test_case_detail) printf("[1,2] covariance: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2));
	check_float_value_equal(0.15, value);
	if (show_test_case_detail) printf("[1,2] correlation: %.2f\n", value);
// Check Diff average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff));
	check_float_value_equal(1.22, value);
	if (show_test_case_detail) printf("[2 Diff] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff));
	check_float_value_equal(146.17, value);
	if (show_test_case_detail) printf("[2 Diff] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff));
	check_float_value_equal(12.09, value);
	if (show_test_case_detail) printf("[2 Diff] standard deviation: %.2f\n", value);
// Check Diff average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(10.00, value);
	if (show_test_case_detail) printf("[4 Diff] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(0.00, value);
	if (show_test_case_detail) printf("[4 Diff] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(0.00, value);
	if (show_test_case_detail) printf("[4 Diff] standard deviation: %.2f\n", value);
// Check Avg5 average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5));
	check_float_value_equal(0.10, value);
	if (show_test_case_detail) printf("[2 Avg5] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5));
	check_float_value_equal(1.32, value);
	if (show_test_case_detail) printf("[2 Avg5] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5));
	check_float_value_equal(1.15, value);
	if (show_test_case_detail) printf("[2 Avg5] standard deviation: %.2f\n", value);
// Check Diff average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(55.10, value);
	if (show_test_case_detail) printf("[4 Avg5] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(291.67, value);
	if (show_test_case_detail) printf("[4 Avg5] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(17.08, value);
	if (show_test_case_detail) printf("[4 Avg5] standard deviation: %.2f\n", value);
// Check Diff/Avg5 correlation
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(-0.16, value);
	if (show_test_case_detail) printf("[2(Diff),4(Diff)] correlation: %.2f\n", value);
	value = covariance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(5.50, value);
	if (show_test_case_detail) printf("[2(Avg5),4(Avg5)] covariance: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(0.28, value);
	if (show_test_case_detail) printf("[2(Avg5),4(Avg5)] correlation: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), 2, 3, 4, 5);
	check_float_value_equal(1.00, value);
	if (show_test_case_detail) printf("[2(Diff 2:4),2(Avg5 3:5)] correlation: %.2f\n", value);
// // Check binary search interval
// 	int index = binary_search_interval((PFINANCE_LONG_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 1));
// 	check_float_value_equal(5.5, value);
// 	if (show_test_case_detail) printf("[1] average: %.2f\n", value);
}

void FinanceAnalyzerTest::test_check_filter_formula()
{
	ResultSet result_set;
	FinanceBoolDataArray filter_data_array;
// Generate data
	ResultSet::generate_filtered_data_for_simulation(result_set, filter_data_array);

	float value;
// Check Formula
//// Show array
//	if (show_test_case_detail) cout << "1: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 1) << endl;
//	if (show_test_case_detail) cout << "2: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2) << endl;
// Check average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), &filter_data_array);
	check_float_value_equal(5.5, value);
	if (show_test_case_detail) printf("[1] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), &filter_data_array);
	check_float_value_equal(8.25, value);
	if (show_test_case_detail) printf("[1] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), &filter_data_array);
	check_float_value_equal(2.87, value);
	if (show_test_case_detail) printf("[1] standard deviation: %.2f\n", value);
// Check average, variance, standard deviation
	value = average(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2), &filter_data_array);
	check_float_value_equal(0.5, value);
	if (show_test_case_detail) printf("[2] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2), &filter_data_array);
	check_float_value_equal(38.25, value);
	if (show_test_case_detail) printf("[2] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2), &filter_data_array);
	check_float_value_equal(6.18, value);
	if (show_test_case_detail) printf("[2] standard deviation: %.2f\n", value);
// Check covariance, correlation
	value = covariance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2), &filter_data_array);
	check_float_value_equal(2.75, value);
	if (show_test_case_detail) printf("[1,2] covariance: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 1), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2), &filter_data_array);
	check_float_value_equal(0.15, value);
	if (show_test_case_detail) printf("[1,2] correlation: %.2f\n", value);
}

// void FinanceAnalyzerTest::test_check_data_statistics()
// {
// 	static const int ERRMSG_SIZE = 256;
// 	static char errmsg[ERRMSG_SIZE];

// 	FinanceAnalyzerDataStatistics data_statistics;
// 	ResultSet result_set;
// 	unsigned short ret = RET_SUCCESS;
// 	float correlation_value;
// // Generate data
// 	ResultSet::generate_data_for_simulation(result_set);

// 	SmartPointer<ResultSetAccessParam> sp_access_param1(NULL);
// 	SmartPointer<ResultSetAccessParam> sp_access_param2(NULL);
// // Check Different type of array
// 	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 2));
// 	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 3));
// 	ret = data_statistics.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
// 	if (CHECK_FAILURE(ret))
// 	{
// 		snprintf(errmsg, ERRMSG_SIZE, "Fail in [2,3] correlation_value, due to: %s", get_ret_description(ret));
// 		throw runtime_error(string(errmsg));
// 	}
// 	if (show_test_case_detail) printf("[2,3] correlation_value: %.2f\n", correlation_value);

// 	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 2));
// 	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 4));
// 	ret = data_statistics.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
// 	if (CHECK_FAILURE(ret))
// 	{
// 		snprintf(errmsg, ERRMSG_SIZE, "Fail in [2,4] correlation_value, due to: %s", get_ret_description(ret));
// 		throw runtime_error(string(errmsg));
// 	}
// 	if (show_test_case_detail) printf("[2,4] correlation_value: %.2f\n", correlation_value);

// 	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 1));
// 	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 4));
// 	ret = data_statistics.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
// 	if (CHECK_FAILURE(ret))
// 	{
// 		snprintf(errmsg, ERRMSG_SIZE, "Fail in [1,4] correlation_value, due to: %s", get_ret_description(ret));
// 		throw runtime_error(string(errmsg));
// 	}
// 	if (show_test_case_detail) printf("[1,4] correlation_value: %.2f\n", correlation_value);

// 	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 1));
// 	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 5));
// 	ret = data_statistics.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
// 	if (CHECK_FAILURE(ret))
// 	{
// 		snprintf(errmsg, ERRMSG_SIZE, "Fail in [1,5] correlation_value, due to: %s", get_ret_description(ret));
// 		throw runtime_error(string(errmsg));
// 	}
// 	if (show_test_case_detail) printf("[1,5] correlation_value: %.2f\n", correlation_value);
// }

void FinanceAnalyzerTest::test_check_histogram()
{
	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];

	ResultSet result_set;
	unsigned short ret = RET_SUCCESS;
// Generate data
	ResultSet::generate_data_for_simulation(result_set);

	FinanceIntDataArray int_data_array;
	FinanceLongDataArray long_data_array;
	FinanceFloatDataArray float_data_array;
	int_data_array.set_type(FinanceField_INT);
	long_data_array.set_type(FinanceField_LONG);
	float_data_array.set_type(FinanceField_FLOAT);

	FinanceIntDataArray int_data_statistics;
	int_data_statistics.set_type(FinanceField_INT);

	// string content_in_output_file;
	list<string> output_result_list;
	list<string>::iterator output_result_list_iter;
	int histogram_array_index = 0;

	PFINANCE_LONG_DATA_ARRAY data_array2 = (PFINANCE_LONG_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 2);
	int data_array2_interval = 4;
// Get Histogram Interval
	ret = get_histogram_interval(*data_array2, data_array2_interval, long_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [2 HistInterval], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "2 HistInterval: " << long_data_array << endl;
	static const long ARRAY2_HIST_INTERVAL[] = {-9, -4, 0, 5, 10};
	if (long_data_array != ARRAY2_HIST_INTERVAL)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[2 HistInterval] is NOT correct");
		throw runtime_error(string(errmsg));
	}
// Get Histogram Statistics
	ret = get_histogram(*data_array2, data_array2_interval, long_data_array.get_data_array(), int_data_statistics);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [2 HistStatistics], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "2 HistStatistics: " << int_data_statistics << endl;
	static const int ARRAY2_HIST_STATISTICS[] = {3, 2, 2, 3};
	if (int_data_statistics != ARRAY2_HIST_STATISTICS)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[2 HistStatistics] is NOT correct");
		throw runtime_error(string(errmsg));
	}
// Output the histogram data
	static const char* LONG_HISTOGRAM_OUTPUT_FILENAME = "long_histogram_for_test.output";
	ret = output_histogram_result(&result_set, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 2), data_array2_interval, LONG_HISTOGRAM_OUTPUT_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to output Array [2 HistStatistics], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
// Check the content of output histogram in the file
	ret = read_output_result(output_result_list, LONG_HISTOGRAM_OUTPUT_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to read Array [2 HistStatistics] from file, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (output_result_list.size() != sizeof(ARRAY2_HIST_STATISTICS) / sizeof(ARRAY2_HIST_STATISTICS[0]))
	{
		snprintf(errmsg, ERRMSG_SIZE, "the Array [2 HistStatistics] size is incorrect, expected: %d, actual: %d", (int)(sizeof(ARRAY2_HIST_STATISTICS) / sizeof(ARRAY2_HIST_STATISTICS[0])), (int)output_result_list.size());
		throw runtime_error(string(errmsg));
	}
	histogram_array_index = 0;
	output_result_list_iter = output_result_list.begin();
	while(output_result_list_iter != output_result_list.begin())
	{
		if (atol((*output_result_list_iter).c_str()) != ARRAY2_HIST_STATISTICS[histogram_array_index])
		{
			snprintf(errmsg, ERRMSG_SIZE, "the Array [2 HistStatistics] element is incorrect, expected: %d, actual: %d", ARRAY2_HIST_STATISTICS[histogram_array_index], atoi((*output_result_list_iter).c_str()));
			throw runtime_error(string(errmsg));			
		}
		++output_result_list_iter;
		histogram_array_index++;
	}
	// printf("result: %s\n", content_in_output_file.c_str());

	long_data_array.reset_array();
	int_data_statistics.reset_array();
	output_result_list.clear();

	PFINANCE_INT_DATA_ARRAY data_array3 = (PFINANCE_INT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 3);
	int data_array3_interval = 3;
// Get Histogram Interval
	ret = get_histogram_interval(*data_array3, data_array3_interval, int_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 HistInterval], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 HistInterval: " << int_data_array << endl;
	static const int ARRAY3_HIST_INTERVAL[] = {-10, -7, -4, -1};
	if (int_data_array != ARRAY3_HIST_INTERVAL)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 HistInterval] is NOT correct");
		throw runtime_error(string(errmsg));
	}
// Get Histogram Statistics
	ret = get_histogram(*data_array3, data_array3_interval, int_data_array.get_data_array(), int_data_statistics);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [3 HistStatistics], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "3 HistStatistics: " << int_data_statistics << endl;
	static const int ARRAY3_HIST_STATISTICS[] = {3, 3, 4};
	if (int_data_statistics != ARRAY3_HIST_STATISTICS)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[3 HistStatistics] is NOT correct");
		throw runtime_error(string(errmsg));
	}
// Output the histogram data
	static const char* INT_HISTOGRAM_OUTPUT_FILENAME = "int_histogram_for_test.output";
	ret = output_histogram_result(&result_set, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 3), data_array3_interval, INT_HISTOGRAM_OUTPUT_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to output Array [3 HistStatistics], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
// Check the content of output histogram in the file
	ret = read_output_result(output_result_list, INT_HISTOGRAM_OUTPUT_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to read Array [3 HistStatistics] from file, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (output_result_list.size() != sizeof(ARRAY3_HIST_STATISTICS) / sizeof(ARRAY3_HIST_STATISTICS[0]))
	{
		snprintf(errmsg, ERRMSG_SIZE, "the Array [3 HistStatistics] size is incorrect, expected: %d, actual: %d", (int)(sizeof(ARRAY3_HIST_STATISTICS) / sizeof(ARRAY3_HIST_STATISTICS[0])), (int)output_result_list.size());
		throw runtime_error(string(errmsg));
	}
	histogram_array_index = 0;
	output_result_list_iter = output_result_list.begin();
	while(output_result_list_iter != output_result_list.begin())
	{
		if (atol((*output_result_list_iter).c_str()) != ARRAY3_HIST_STATISTICS[histogram_array_index])
		{
			snprintf(errmsg, ERRMSG_SIZE, "the Array [3 HistStatistics] element is incorrect, expected: %d, actual: %d", ARRAY3_HIST_STATISTICS[histogram_array_index], atoi((*output_result_list_iter).c_str()));
			throw runtime_error(string(errmsg));			
		}
		++output_result_list_iter;
		histogram_array_index++;
	}
	// printf("result: %s\n", content_in_output_file.c_str());
	int_data_array.reset_array();
	int_data_statistics.reset_array();
	output_result_list.clear();

	PFINANCE_FLOAT_DATA_ARRAY data_array5 = (PFINANCE_FLOAT_DATA_ARRAY)result_set.get_array(FinanceSource_StockExchangeAndVolume, 5);
	int data_array5_interval = 5;
// Get Histogram Interval
	ret = get_histogram_interval(*data_array5, data_array5_interval, float_data_array);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [5 HistInterval], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "5 HistInterval: " << float_data_array << endl;
	static const float ARRAY5_HIST_INTERVAL[] = {1.1, 120.88, 240.66, 360.44, 480.22, 600.0};
	if (float_data_array != ARRAY5_HIST_INTERVAL)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[5 HistInterval] is NOT correct");
		throw runtime_error(string(errmsg));
	}
// Get Histogram Statistics
	ret = get_histogram(*data_array5, data_array5_interval, float_data_array.get_data_array(), int_data_statistics);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to get Array [5 HistStatistics], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) cout << "5 HistStatistics: " << int_data_statistics << endl;
	static const int ARRAY5_HIST_STATISTICS[] = {2, 3, 1, 2, 2};
	if (int_data_statistics != ARRAY5_HIST_STATISTICS)
	{
		snprintf(errmsg, ERRMSG_SIZE, "The Array[5 HistStatistics] is NOT correct");
		throw runtime_error(string(errmsg));
	}
// Output the histogram data
	static const char* FLOAT_HISTOGRAM_OUTPUT_FILENAME = "float_histogram_for_test.output";
	ret = output_histogram_result(&result_set, new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 5), data_array5_interval, FLOAT_HISTOGRAM_OUTPUT_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to output Array [5 HistStatistics], due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
// Check the content of output histogram in the file
	ret = read_output_result(output_result_list, FLOAT_HISTOGRAM_OUTPUT_FILENAME);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail to read Array [5 HistStatistics] from file, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (output_result_list.size() != sizeof(ARRAY5_HIST_STATISTICS) / sizeof(ARRAY5_HIST_STATISTICS[0]))
	{
		snprintf(errmsg, ERRMSG_SIZE, "the Array [5 HistStatistics] size is incorrect, expected: %d, actual: %d", (int)(sizeof(ARRAY5_HIST_STATISTICS) / sizeof(ARRAY5_HIST_STATISTICS[0])), (int)output_result_list.size());
		throw runtime_error(string(errmsg));
	}
	histogram_array_index = 0;
	output_result_list_iter = output_result_list.begin();
	while(output_result_list_iter != output_result_list.begin())
	{
		if (atol((*output_result_list_iter).c_str()) != ARRAY5_HIST_STATISTICS[histogram_array_index])
		{
			snprintf(errmsg, ERRMSG_SIZE, "the Array [5 HistStatistics] element is incorrect, expected: %d, actual: %d", ARRAY5_HIST_STATISTICS[histogram_array_index], atoi((*output_result_list_iter).c_str()));
			throw runtime_error(string(errmsg));			
		}
		++output_result_list_iter;
		histogram_array_index++;
	}

	float_data_array.reset_array();
	int_data_statistics.reset_array();
	output_result_list.clear();
}

bool FinanceAnalyzerTest::test(TestType test_type)
{
	typedef void (FinanceAnalyzerTest::*test_func_ptr)();
	static test_func_ptr test_func_array[] =
	{
		&FinanceAnalyzerTest::test_check_array,
		&FinanceAnalyzerTest::test_check_array_statistics,
		&FinanceAnalyzerTest::test_check_filter_array,
		&FinanceAnalyzerTest::test_check_filter_rule,
		&FinanceAnalyzerTest::test_check_formula,
		&FinanceAnalyzerTest::test_check_filter_formula,
		// &FinanceAnalyzerTest::test_check_data_statistics,
		&FinanceAnalyzerTest::test_check_histogram
	};

	bool pass = true;
	try
	{
		printf("Run Test Case: %s\n", TEST_TYPE_DESCRIPTION[test_type]);
		(this->*(test_func_array[test_type]))();
		printf("Test Case: %s... Pass\n", TEST_TYPE_DESCRIPTION[test_type]);
	}
	catch(exception& e)
	{
		pass = false;
		printf("Test Case: %s... FAILED, due to: %s\n", TEST_TYPE_DESCRIPTION[test_type], e.what());
	}
	return pass;
}
