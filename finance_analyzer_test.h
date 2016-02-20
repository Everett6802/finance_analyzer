#ifndef FINANCE_ANALYZER_TEST_H
#define FINANCE_ANALYZER_TEST_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"


enum TestType{Test_CheckArray, Test_CheckFormula, Test_CheckCalculator, TestSize};

extern const char* TEST_TYPE_DESCRIPTION[];

class FinanceAnalyzerTest
{
//	DECLARE_MSG_DUMPER()

private:
	bool show_test_case_detail;

	void check_float_value_equal(float expected_value, float actual_value);
	void test_check_array();
	void test_check_formula();
	void test_check_calculator();

public:
	FinanceAnalyzerTest();
	~FinanceAnalyzerTest();

	void set_show_detail(bool show_detail);
	void test(TestType test_type);
};

#endif
