#ifndef INTERACTIVE_SESSION_H
#define INTERACTIVE_SESSION_H

// This header file contains definitions of a number of data types used in system calls. These types are used in the next two include files.
#include <sys/types.h> 
// The header file socket.h includes a number of definitions of structures needed for sockets.
#include <sys/socket.h>
// The header file in.h contains constants and structures needed for internet domain addresses.
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include "common.h"
#include "mgr_factory.h"
#include "finance_analyzer_mgr.h"
#include "data_sql_reader.h"
#include "data_csv_reader.h"
#include "data_reader.h"


enum InteractiveSessionEventType
{
	InteractiveSessionEvent_Exit,
	InteractiveSessionEventSize
};

class InteractiveSession
{
	DECLARE_MSG_DUMPER()
	DECLARE_DATA_READER()
private:
	static const int REQ_BUF_SIZE;
	static const int RSP_BUF_VERY_SHORT_SIZE;
	static const int RSP_BUF_SHORT_SIZE;
	static const int RSP_BUF_SIZE;
	static const int RSP_BUF_LONG_SIZE;
	static const int RSP_BUF_VERY_LONG_SIZE;
	static const int MAX_ARGC;
	static MgrFactory mgr_factory;
	static void init_command_map();
	static void* thread_handler(void* void_tr);

	pthread_t pid;
	int exit;
	int sock_fd;
	sockaddr_in client_sock;
	char session_tag[64];
	int session_id;
	PIEVENT_NOTIFY event_notify;
	bool user_exit;
	// PSQL_READER sql_reader;
	FinanceAnalysisMode finance_analysis_mode;
	PIFINANCE_ANALYZER_MGR manager;
	char* source_string_param;
	char* time_range_string_param;
	char* company_string_param;
	FinanceDataType finance_data_type;
	DataSqlReaderParam* data_sql_reader_param;
	DataCsvReaderParam* data_csv_reader_param;
	void* data_reader_param_array[FinanceDataSize];
	bool search_rule_need_reset;
	bool show_stock_support_resistance_verbose;
	char* stock_support_resistance_date_filter;
	char* stock_support_resistance_volume_filter;
	// PSEARCH_RULE_SET search_rule_set;
	PRESULT_SET_MAP result_set_map;
	// unsigned short thread_ret;
	char* stock_support_and_resistance_root_folderpath;

	unsigned short thread_handler_internal();
	unsigned short print_single_stock_support_resistance_string(const char* stock_support_resistance_entry, int stock_support_resistance_entry_len);
	void reset_search_param();
// Handle command related fundtions
	unsigned short handle_command(int argc, char **argv);
	unsigned short handle_get_finance_mode_command(int argc, char **argv);
	unsigned short handle_set_finance_mode_command(int argc, char **argv);
	unsigned short handle_get_method_command(int argc, char **argv);
	unsigned short handle_set_method_command(int argc, char **argv);
	unsigned short handle_get_time_range_command(int argc, char **argv);
	unsigned short handle_set_time_range_command(int argc, char **argv);
	unsigned short handle_get_company_command(int argc, char **argv);
	unsigned short handle_set_company_command(int argc, char **argv);
	unsigned short handle_get_reader_type_command(int argc, char **argv);
	unsigned short handle_set_reader_type_command(int argc, char **argv);
	unsigned short handle_set_csv_root_folderpath_command(int argc, char **argv);
	unsigned short handle_search_command(int argc, char **argv);
	unsigned short handle_reset_search_param_command(int argc, char **argv);
	unsigned short handle_set_stock_support_resistance_filepath_command(int argc, char **argv);
	unsigned short handle_set_stock_support_resistance_verbose_command(int argc, char **argv);
	unsigned short handle_set_stock_support_resistance_date_filter_command(int argc, char **argv);
	unsigned short handle_set_stock_support_resistance_volume_filter_command(int argc, char **argv);
	unsigned short handle_get_stock_support_resistance_parameters_command(int argc, char **argv);
	unsigned short handle_cleanup_stock_support_resistance_parameters_command(int argc, char **argv);
	unsigned short handle_find_stock_support_resistance_command(int argc, char **argv);
	unsigned short handle_help_command(int argc, char **argv);
	unsigned short handle_exit_command(int argc, char **argv);
	unsigned short print_to_console(std::string response)const;
	unsigned short print_prompt_to_console()const;
	unsigned short init_finance_manager(FinanceAnalysisMode new_finance_analysis_mode);

public:
	InteractiveSession(int client_fd, sockaddr_in& client_sockaddress, int interactive_session_id, PIEVENT_NOTIFY parent);
	~InteractiveSession();

	unsigned short initialize();
	void notify_exit();
};
typedef InteractiveSession* PINTERACTIVE_SESSION;

#endif
