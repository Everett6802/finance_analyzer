#ifndef FINANCE_ANALYZER_CALCULATOR_H
#define FINANCE_ANALYZER_CALCULATOR_H

#include "finance_analyzer_common.h"
#include "finance_analyzer_common_class.h"
#include "finance_analyzer_database_time_range.h"


#define RES_INFO_LENGTH 1024

class FinanceAnalyzerCalculator
{
	DECLARE_MSG_DUMPER()

private:
	DECLARE_DATABASE_TIME_RANGE();
	char res_info[RES_INFO_LENGTH];
	bool need_res_info;

	unsigned short correlate(const PRESULT_SET result_set, FinanceSourceType finance_source_type1, int finance_field_no1, ArrayElementCalculationType calculation_type1, FinanceSourceType finance_source_type2, int finance_field_no2, ArrayElementCalculationType calculation_type2, int start_index1, int start_index2, int end_index1, int end_index2, float& correlation_value);

public:
	FinanceAnalyzerCalculator();
	~FinanceAnalyzerCalculator();

	void enable_res_info(bool enable);
	const char* get_last_res_info()const;
	unsigned short correlate(const PRESULT_SET result_set, const SmartPointer<ResultSetAccessParam>& access_param1, const SmartPointer<ResultSetAccessParam>& access_param2, float& correlation_value);
	unsigned short correlate_auto_alignment(const PRESULT_SET result_set, const SmartPointer<ResultSetAccessParam>& access_param1, const SmartPointer<ResultSetAccessParam>& access_param2, float& correlation_value);
};

#endif
