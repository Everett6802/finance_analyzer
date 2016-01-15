#ifndef FINANCE_ANALYZER_CALCULATOR_H
#define FINANCE_ANALYZER_CALCULATOR_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_database_time_range.h"


class FinanceAnalyzerCalculator
{
	DECLARE_MSG_DUMPER()

private:
//	DECLARE_DATABASE_TIME_RANGE();

public:
	FinanceAnalyzerCalculator();
	~FinanceAnalyzerCalculator();

	unsigned short correlate(const PRESULT_SET result_set, float& correlation_coeffient);
};

#endif
