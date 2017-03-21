#ifndef FINANCE_ANALYZER_INTERACTIVE_SESSION_H
#define FINANCE_ANALYZER_INTERACTIVE_SESSION_H

// This header file contains definitions of a number of data types used in system calls. These types are used in the next two include files.
#include <sys/types.h> 
// The header file socket.h includes a number of definitions of structures needed for sockets.
#include <sys/socket.h>
// The header file in.h contains constants and structures needed for internet domain addresses.
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include "finance_analyzer_common.h"
#include "finance_analyzer_mgr_factory.h"
#include "finance_analyzer_mgr.h"


enum InteractiveSessionEventType
{
	InteractiveSessionEvent_Exit,
	InteractiveSessionEventSize
};

class FinanceAnalyzerInteractiveSession
{
	DECLARE_MSG_DUMPER()
private:
	static const int REQ_BUF_SIZE;
	static const int RSP_BUF_VERY_SHORT_SIZE;
	static const int RSP_BUF_SHORT_SIZE;
	static const int RSP_BUF_SIZE;
	static const int RSP_BUF_LONG_SIZE;
	static const int RSP_BUF_VERY_LONG_SIZE;
	static const int MAX_ARGC;
	static FinanceAnalyzerMgrFactory mgr_factory;
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
	FinanceAnalyzerSqlReader finance_analyzer_sql_reader;
	FinanceAnalysisMode finance_analysis_mode;
	PIFINANCE_ANALYZER_MGR finance_analyzer_mgr;
	char* source_string_param;
	char* time_range_string_param;
	char* company_string_param;
	bool search_rule_need_reset;
	// PSEARCH_RULE_SET search_rule_set;
	PRESULT_SET_MAP result_set_map;
	// unsigned short thread_ret;

	unsigned short thread_handler_internal();
	unsigned short handle_command(int argc, char **argv);
	unsigned short handle_get_finance_mode_command(int argc, char **argv);
	unsigned short handle_set_finance_mode_command(int argc, char **argv);
	unsigned short handle_get_source_command(int argc, char **argv);
	unsigned short handle_set_source_command(int argc, char **argv);
	unsigned short handle_get_time_range_command(int argc, char **argv);
	unsigned short handle_set_time_range_command(int argc, char **argv);
	unsigned short handle_get_company_command(int argc, char **argv);
	unsigned short handle_set_company_command(int argc, char **argv);
	unsigned short handle_search_command(int argc, char **argv);
	unsigned short handle_help_command(int argc, char **argv);
	unsigned short handle_exit_command(int argc, char **argv);
	unsigned short print_to_console(std::string response)const;
	unsigned short print_prompt_to_console()const;
	unsigned short init_finance_manager(FinanceAnalysisMode new_finance_analysis_mode);

public:
	FinanceAnalyzerInteractiveSession(int client_fd, sockaddr_in& client_sockaddress, int interactive_session_id, PIEVENT_NOTIFY parent);
	~FinanceAnalyzerInteractiveSession();

	unsigned short initialize();
	void notify_exit();
};
typedef FinanceAnalyzerInteractiveSession* PFINANCE_ANALYZER_INTERACTIVE_SESSION;

#endif
