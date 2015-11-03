#ifndef FINANCE_ANALYZER_SQL_READER_H
#define FINANCE_ANALYZER_SQL_READER_H

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <string>
#include "finance_analyzer_common.h"


#define CMD_BUF_SIZE 512

class FinanceAnalyzerSqlReader
{
	DECLARE_MSG_DUMPER()

private:
	static const char* MYSQL_SERVER;
	static const char* MYSQL_USERNAME;
	static const char* MYSQL_PASSWORD;

	static const char* FORMAT_CMD_CREATE_DATABASE;
//	static const char* format_cmd_create_table;
//	static const char* format_cmd_insert_into_table;

	MYSQL* connection;
	char cmd_buf[CMD_BUF_SIZE];
//	char current_time_string[CURRENT_TIME_STRING_LENGTH];
//	bool table_created;
//	char server[MSG_DUMPER_STRING_SIZE];
//	char username[MSG_DUMPER_STRING_SIZE];
//	char password[MSG_DUMPER_STRING_SIZE];
	std::string database_name;

protected:
//	virtual unsigned short parse_config_param(const char* param_title, const char* param_content);

public:
	FinanceAnalyzerSqlReader();
	~FinanceAnalyzerSqlReader();

	unsigned short try_connect_mysql(const std::string database);
	unsigned short disconnect_mysql();

//	virtual unsigned short open_device();
//	virtual unsigned short close_device();
//
//	virtual unsigned short initialize(const char* config_path, void* config=NULL);
//	virtual unsigned short deinitialize();
//	virtual unsigned short write_msg(PMSG_CFG msg_cfg);
};

#endif
