#ifndef FINANCE_ANALYZER_CALCULATOR_H
#define FINANCE_ANALYZER_CALCULATOR_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_database_time_range.h"


class FinanceAnalyzerCalculator
{
	DECLARE_MSG_DUMPER()

private:
	DECLARE_DATABASE_TIME_RANGE();

public:
	FinanceAnalyzerCalculator();
	~FinanceAnalyzerCalculator();

	unsigned short correlate(const PRESULT_SET result_set, float& correlation_value);
	unsigned short correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, ArrayElementCalculationType calculation_type1, FinanceSourceType finance_source_type2, int finance_field_no2, ArrayElementCalculationType calculation_type2, float& correlation_value);
	unsigned short correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, FinanceSourceType finance_source_type2, int finance_field_no2, float& correlation_value);
};

#endif
