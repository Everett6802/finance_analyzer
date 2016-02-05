#include <assert.h>
#include <stdexcept>
#include "finance_analyzer_test.h"
#include "finance_analyzer_calculator.h"
#include "finance_analyzer_algorithm.h"


using namespace std;

const char* TEST_TYPE_DESCRIPTION[] =
{
	"Check Formula",
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
	static const int VALUE_SIZE = 16;
	static char expected_value_str[VALUE_SIZE];
	static char actual_value_str[VALUE_SIZE];
	snprintf(expected_value_str, VALUE_SIZE, "%.2f", expected_value);
	snprintf(actual_value_str, VALUE_SIZE, "%.2f", actual_value);
	if (strcmp(expected_value_str, actual_value_str) != 0)
	{
		static const int ERRMSG_SIZE = 64;
		static char errmsg[ERRMSG_SIZE];
		snprintf(errmsg, ERRMSG_SIZE, "Value is NOT equal; Expected: %s, Actual: %s", expected_value_str, actual_value_str);
		throw runtime_error(errmsg);
	}
}

void FinanceAnalyzerTest::set_show_detail(bool show_detail)
{
	show_test_case_detail = show_detail;
}

void FinanceAnalyzerTest::test_check_formula()
{
	static const char* date[] = {"2016-01-04", "2016-01-05", "2016-01-06", "2016-01-07", "2016-01-08", "2016-01-11", "2016-01-12", "2016-01-13", "2016-01-14", "2016-01-15"};
	static const char* field1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}; // Long
	static const char* field2[] = {"-1", "2", "-3", "4", "-5", "6", "-7", "8", "-9", "10"}; // Long
	static const char* field3[] = {"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "-10"}; // Int
	static const char* field4[] = {"-2.1", "8.2", "-100.3", "20.4", "-88.5", "100.6", "-107.7", "100.8", "-202.9", "300.0"}; // Float
	static const char* field5[] = {"1.1", "21.2", "122.3", "199.4", "200.5", "300.6", "420.7", "435.8", "599.4", "600.0"}; // Float

	printf("Check Formula...\n");
	FinanceAnalyzerCalculator calculator;
//	unsigned short ret = RET_SUCCESS;
	ResultSet result_set;
//	float correlation_value;
	for (int i = 0 ; i < 6 ; i++)
		result_set.add_set(FinanceSource_StockExchangeAndVolume, i);
	char data[32];
	for (int i = 0 ; i < 10 ; i++)
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
// Show array
	if (show_test_case_detail) cout << "1: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 1) << endl;
	if (show_test_case_detail) cout << "2: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2) << endl;
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
// Check Avg/Diff array
	if (show_test_case_detail) cout << "2 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff) << endl;
	if (show_test_case_detail) cout << "2 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5) << endl;
	if (show_test_case_detail) cout << "2 Avg10: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg10) << endl;
	if (show_test_case_detail) cout << "4 Diff: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff) << endl;
	if (show_test_case_detail) cout << "4 Avg5: " << *result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5) << endl;
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
	check_float_value_equal(-12.60, value);
	if (show_test_case_detail) printf("[4 Avg5] average: %.2f\n", value);
	value = variance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(750.47, value);
	if (show_test_case_detail) printf("[4 Avg5] variance: %.2f\n", value);
	value = standard_deviation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(27.39, value);
	if (show_test_case_detail) printf("[4 Avg5] standard deviation: %.2f\n", value);
// Check Diff/Avg5 correlation
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Diff));
	check_float_value_equal(0.97, value);
	if (show_test_case_detail) printf("[2(Diff),4(Diff)] correlation: %.2f\n", value);
	value = covariance(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(29.11, value);
	if (show_test_case_detail) printf("[2(Avg5),4(Avg5)] covariance: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), result_set.get_array(FinanceSource_StockExchangeAndVolume, 4, ArrayElementCalculation_Avg5));
	check_float_value_equal(0.93, value);
	if (show_test_case_detail) printf("[2(Avg5),4(Avg5)] correlation: %.2f\n", value);
	value = correlation(result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Diff), result_set.get_array(FinanceSource_StockExchangeAndVolume, 2, ArrayElementCalculation_Avg5), 2, 3, 4, 5);
	check_float_value_equal(1, value);
	if (show_test_case_detail) printf("[2(Diff 2:4),2(Avg5 3:5)] correlation: %.2f\n", value);

	printf("Check Formula... Pass\n");
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

	printf("Check Calculator...\n");
	FinanceAnalyzerCalculator calculator;

	ResultSet result_set;
	for (int i = 0 ; i < 6 ; i++)
		result_set.add_set(FinanceSource_StockExchangeAndVolume, i);
	char data[32];
	for (int i = 0 ; i < 10 ; i++)
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

// Check Different type of array
	ret = calculator.correlate(&result_set, FinanceSource_StockExchangeAndVolume, 2, FinanceSource_StockExchangeAndVolume, 3, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [2,3] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[2,3] correlation_value: %.2f\n", correlation_value);
	ret = calculator.correlate(&result_set, FinanceSource_StockExchangeAndVolume, 2, FinanceSource_StockExchangeAndVolume, 4, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [2,4] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[2,4] correlation_value: %.2f\n", correlation_value);
	ret = calculator.correlate(&result_set, FinanceSource_StockExchangeAndVolume, 1, FinanceSource_StockExchangeAndVolume, 4, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [1,4] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[1,4] correlation_value: %.2f\n", correlation_value);
	ret = calculator.correlate(&result_set, FinanceSource_StockExchangeAndVolume, 1, FinanceSource_StockExchangeAndVolume, 5, correlation_value);
	if (CHECK_FAILURE(ret))
	{
		snprintf(errmsg, ERRMSG_SIZE, "Fail in [1,5] correlation_value, due to: %s", get_ret_description(ret));
		throw runtime_error(string(errmsg));
	}
	if (show_test_case_detail) printf("[1,5] correlation_value: %.2f\n", correlation_value);

	printf("Check Calculator... Pass\n");
}

void FinanceAnalyzerTest::test(TestType test_type)
{
	typedef void (FinanceAnalyzerTest::*test_func_ptr)();
	static test_func_ptr test_func_array[] =
	{
		&FinanceAnalyzerTest::test_check_formula,
		&FinanceAnalyzerTest::test_check_calculator
	};

	(this->*(test_func_array[test_type]))();
}