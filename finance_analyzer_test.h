#ifndef FINANCE_ANALYZER_TEST_H
#define FINANCE_ANALYZER_TEST_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


enum TestType{Test_CheckArray, Test_CheckArrayStatistics, Test_CheckFilterArray, Test_CheckFilterRule, Test_CheckFormula, Test_CheckFilterFormula, Test_CheckDataStatistics, Test_CheckHistogram, TestSize};

extern const char* TEST_TYPE_DESCRIPTION[];

class FinanceAnalyzerTest
{
//	DECLARE_MSG_DUMPER()

private:
	bool show_test_case_detail;

	bool check_float_value_equal(float expected_value, float actual_value, bool throw_exception=true);
	void test_check_array();
	void test_check_array_statistics();
	void test_check_filter_array();
	void test_check_filter_rule();
	void test_check_formula();
	void test_check_filter_formula();
	void test_check_data_statistics();
	void test_check_histogram();

public:
	FinanceAnalyzerTest();
	~FinanceAnalyzerTest();

	void set_show_detail(bool show_detail);
	bool test(TestType test_type);
};

#endif
