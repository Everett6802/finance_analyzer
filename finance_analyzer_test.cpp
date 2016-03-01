#include <math.h>
#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_test.h"
#include "finance_analyzer_calculator.h"
#include "finance_analyzer_algorithm.h"


using namespace std;

const char* TEST_TYPE_DESCRIPTION[] =
{
	"Check Array",
	"Check Formula",
	"Check Formula Filter",
	"Check Calculator"
};
//DECLARE_MSG_DUMPER_PARAM()

FinanceAnalyzerTest::FinanceAnalyzerTest() :
		show_test_case_detail(false)
{
//	IMPLEMENT_MSG_DUMPER()
}

FinanceAnalyzerTest::~FinanceAnalyzerTest()
{
//	RELEASE_MSG_DUMPER()
}

void FinanceAnalyzerTest::check_float_value_equal(float expected_value, float actual_value)
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
		throw runtime_error(errmsg);
	}
}

void FinanceAnalyzerTest::set_show_detail(bool show_detail)
{
	show_test_case_detail = show_detail;
}

void FinanceAnalyzerTest::test_check_array()
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15"};
	static const char* field1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}; // Long
	static const char* field2[] = {"-1", "2", "-3", "4", "-5", "6", "-7", "8", "-9", "10"}; // Long; Dummy
	static const char* field3[] = {"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10"}; // Int
	static const char* field4[] = {"10.1", "20.1", "30.1", "40.1", "50.1", "60.1", "70.1", "80.1", "90.1", "100.1"}; // Float
	static const char* field5[] = {"1.1", "21.2", "122.3", "199.4", "200.5", "300.6", "420.7", "435.8", "599.4", "600.0"}; // Float; Dummy

	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];
	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

	unsigned short ret = RET_SUCCESS;
	ResultSet result_set;
	for (int i = 0 ; i < 6 ; i++)
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
	}

	SmartPointer<FinanceIntDataArray> sp_int_data_array(new FinanceIntDataArray());
	SmartPointer<FinanceLongDataArray> sp_long_data_array(new FinanceLongDataArray());
	SmartPointer<FinanceFloatDataArray> sp_float_data_array(new FinanceFloatDataArray());
	sp_int_data_array->set_type(FinanceField_INT);
	sp_long_data_array->set_type(FinanceField_LONG);
	sp_float_data_array->set_type(FinanceField_FLOAT);

// Check Avg/Diff array
//	if (show_test_case_detail) cout << "2 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff) << endl;
//	if (show_test_case_detail) cout << "2 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5) << endl;
//	if (show_test_case_detail) cout << "2 Avg10: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg10) << endl;
//	if (show_test_case_detail) cout << "4 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff) << endl;
//	if (show_test_case_detail) cout << "4 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5) << endl;

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
	static const float ARRAY3_WAVG10[] = {-6.11};
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

void FinanceAnalyzerTest::test_check_formula()
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15"};
	static const char* field1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}; // Long
	static const char* field2[] = {"-1", "2", "-3", "4", "-5", "6", "-7", "8", "-9", "10"}; // Long
	static const char* field3[] = {"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10"}; // Int
	static const char* field4[] = {"-2.1", "8.2", "-100.3", "20.4", "-88.5", "100.6", "-107.7", "100.8", "-202.9", "300.0"}; // Float
	static const char* field5[] = {"1.1", "21.2", "122.3", "199.4", "200.5", "300.6", "420.7", "435.8", "599.4", "600.0"}; // Float

	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

//	unsigned short ret = RET_SUCCESS;
	ResultSet result_set;
//	float correlation_value;
	for (int i = 0 ; i < 6 ; i++)
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
	}

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
	check_float_value_equal(33.57, value);
	if (show_test_case_detail) printf("[4 Diff] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(55103.06, value);
	if (show_test_case_detail) printf("[4 Diff] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(234.74, value);
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
	check_float_value_equal(-12.62, value);
	if (show_test_case_detail) printf("[4 Avg5] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(752.91, value);
	if (show_test_case_detail) printf("[4 Avg5] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(27.44, value);
	if (show_test_case_detail) printf("[4 Avg5] standard deviation: %.2f\n", value);
// Check Diff/Avg5 correlation
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(0.97, value);
	if (show_test_case_detail) printf("[2(Diff),4(Diff)] correlation: %.2f\n", value);
	value = covariance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(29.19, value);
	if (show_test_case_detail) printf("[2(Avg5),4(Avg5)] covariance: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(0.93, value);
	if (show_test_case_detail) printf("[2(Avg5),4(Avg5)] correlation: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), 2, 3, 4, 5);
	check_float_value_equal(1, value);
	if (show_test_case_detail) printf("[2(Diff 2:4),2(Avg5 3:5)] correlation: %.2f\n", value);
}


void FinanceAnalyzerTest::test_check_filter_formula()
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15", "2016-01-18", "2016-01-19", "2016-01-20", "2016-01-21", "2016-01-22", "2016-01-25", "2016-01-26", "2016-01-27", "2016-01-28", "2016-01-29"};
	static const char* field1[] = {"1", "0", "2", "0", "3", "0", "4", "0", "5", "0", "6", "0", "7", "0", "8", "0", "9", "0", "10", "0"}; // Long
	static const char* field2[] = {"-1", "0", "2", "0", "-3", "0", "4", "0", "-5", "0", "6", "0", "-7", "0", "8", "0", "-9", "0", "10", "0"}; // Long; Dummy
	static const char* field3[] = {"-1", "0", "-2", "0", "-3", "0", "-4", "0", "-5", "0", "-6", "0", "-7", "0", "-8", "0", "-9", "0", "-10", "0"}; // Int
	static const char* field4[] = {"10.1", "0.0", "20.1", "0.0", "30.1", "0.0", "40.1", "0.0", "50.1", "0.0", "60.1", "0.0", "70.1", "0.0", "80.1", "0.0", "90.1", "0.0", "100.1", "0.0"}; // Float
	static const char* field5[] = {"1.1", "0.0", "21.2", "0.0", "122.3", "0.0", "199.4", "0.0", "200.5", "0.0", "300.6", "0.0", "420.7", "0.0", "435.8", "0.0", "599.4", "0.0", "600.0", "0.0"}; // Float; Dummy
	static const bool filter[] = {true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false};

//	static const int ERRMSG_SIZE = 256;
//	static char errmsg[ERRMSG_SIZE];
	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

	FinanceBoolDataArray filter_data_array;

//	unsigned short ret = RET_SUCCESS;
	ResultSet result_set;
	for (int i = 0 ; i < 6 ; i++)
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

		filter_data_array.add(filter[i]);
	}

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

void FinanceAnalyzerTest::test_check_calculator()
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15"};
	static const char* field1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}; // Long
	static const char* field2[] = {"-1", "2", "-3", "4", "-5", "6", "-7", "8", "-9", "10"}; // Long
	static const char* field3[] = {"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10"}; // Int
	static const char* field4[] = {"-2.1", "8.2", "-100.3", "20.4", "-88.5", "100.6", "-107.7", "100.8", "-202.9", "300.0"}; // Float
	static const char* field5[] = {"1.1", "21.2", "122.3", "199.4", "200.5", "300.6", "420.7", "435.8", "599.4", "600.0"}; // Float

	static const int ERRMSG_SIZE = 256;
	static char errmsg[ERRMSG_SIZE];
	static const int DATA_SIZE = sizeof(date) / sizeof(date[0]);

	FinanceAnalyzerCalculator calculator;

	ResultSet result_set;
	for (int i = 0 ; i < 6 ; i++)
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
	}

	unsigned short ret = RET_SUCCESS;
	float correlation_value;
	SmartPointer<ResultSetAccessParam> sp_access_param1(NULL);
	SmartPointer<ResultSetAccessParam> sp_access_param2(NULL);
// Check Different type of array
	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 2));
	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 3));
	ret = calculator.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [2,3] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[2,3] correlation_value: %.2f\n", correlation_value);

	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 2));
	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 4));
	ret = calculator.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [2,4] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[2,4] correlation_value: %.2f\n", correlation_value);

	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 1));
	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 4));
	ret = calculator.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [1,4] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[1,4] correlation_value: %.2f\n", correlation_value);

	sp_access_param1.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 1));
	sp_access_param2.set_new(new ResultSetAccessParam(FinanceSource_StockExchangeAndVolume, 5));
	ret = calculator.correlate_auto_alignment(&result_set, sp_access_param1, sp_access_param2, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [1,5] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[1,5] correlation_value: %.2f\n", correlation_value);
}

void FinanceAnalyzerTest::test(TestType test_type)
{
	typedef void (FinanceAnalyzerTest::*test_func_ptr)();
	static test_func_ptr test_func_array[] =
	{
		&FinanceAnalyzerTest::test_check_array,
		&FinanceAnalyzerTest::test_check_formula,
		&FinanceAnalyzerTest::test_check_filter_formula,
		&FinanceAnalyzerTest::test_check_calculator
	};

	printf("Run Test Case: %s\n", TEST_TYPE_DESCRIPTION[test_type]);
	try
	{
		(this->*(test_func_array[test_type]))();
		printf("Test Case: %s... Pass\n", TEST_TYPE_DESCRIPTION[test_type]);
	}
	catch(exception& e)
	{
		printf("Test Case: %s... FAILED, due to: %s\n", TEST_TYPE_DESCRIPTION[test_type], e.what());
	}
}
