#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <map>
// #include <string>
#include "finance_analyzer_interactive_session.h"


using namespace std;

#define REGISTER_CLASS(n, m) mgr_factory.register_class<n>(m)

// Command type definition
enum InteractiveSessionCommandType
{
	InteractiveSessionCommand_GetFinanceMode,
	InteractiveSessionCommand_SetFinanceMode,
	InteractiveSessionCommand_GetSource,
	InteractiveSessionCommand_SetSource,
	InteractiveSessionCommand_GetTimeRange,
	InteractiveSessionCommand_SetTimeRange,
	InteractiveSessionCommand_GetCompany,
	InteractiveSessionCommand_SetCompany,
	InteractiveSessionCommand_Search,
	InteractiveSessionCommand_SetStockSupportResistanceFilepath,
	InteractiveSessionCommand_FindStockSupportResistance,
	InteractiveSessionCommand_FindStockSupportResistanceVerbose,
	InteractiveSessionCommand_Help,
	InteractiveSessionCommand_Exit,
	InteractiveSessionCommandSize
};

static const char *interactive_session_command[InteractiveSessionCommandSize] = 
{
	"get_finance_mode",
	"set_finance_mode",
	"get_source",
	"set_source",
	"get_time_range",
	"set_time_range",
	"get_company",
	"set_company",
	"search",
	"set_stock_support_resistance_filepath",
	"find_stock_support_resistance",
	"help",
	"exit"
};

typedef map<string, InteractiveSessionCommandType> COMMAND_MAP;
typedef COMMAND_MAP::iterator COMMAND_MAP_ITER;

static const char* INCORRECT_COMMAND_ARGUMENT_FORMAT = "Incorrect command[%s] argument: %s";

static string welcome_phrases = "\n************** Welcome to Finance Analyzer **************\nCATUION: You should determine the finance mode before running other commands\n Two choices:\n Market Mode: market_mode, 0\n Stock Mode: stock_mode, 1\n\n";
static string incomplete_command_phrases = "\nIncomplete Command\n\n";
static string incorrect_command_phrases = "\nIncorrect Command\n\n";

static COMMAND_MAP command_map;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

const int FinanceAnalyzerInteractiveSession::REQ_BUF_SIZE = 1024;
const int FinanceAnalyzerInteractiveSession::RSP_BUF_VERY_SHORT_SIZE = 32;
const int FinanceAnalyzerInteractiveSession::RSP_BUF_SHORT_SIZE = 64;
const int FinanceAnalyzerInteractiveSession::RSP_BUF_SIZE = 256;
const int FinanceAnalyzerInteractiveSession::RSP_BUF_LONG_SIZE = 1024;
const int FinanceAnalyzerInteractiveSession::RSP_BUF_VERY_LONG_SIZE = 4096;
const int FinanceAnalyzerInteractiveSession::MAX_ARGC = 20;
FinanceAnalyzerMgrFactory FinanceAnalyzerInteractiveSession::mgr_factory;

void FinanceAnalyzerInteractiveSession::init_command_map()
{
	static bool init_map = false;
	if (!init_map)
	{
		pthread_mutex_lock(&mtx);
		if (!init_map)
		{
			for (int i = 0 ; i < InteractiveSessionCommandSize ; i++)
			{
				command_map.insert(make_pair(string(interactive_session_command[i]), (InteractiveSessionCommandType)i));
			}
			// for(COMMAND_MAP_ITER iter = command_map.begin() ; iter != command_map.end() ; iter++)
			// {
			// 	string command_description = (string)iter->first;
			// 	int command_type = (int)iter->second;
			// 	STATIC_WRITE_FORMAT_DEBUG("Command %d: %s", command_type, command_description.c_str());
			// }
// Register the manager class to manager factory
			// static FinanceAnalyzerMgrFactory mgr_factory;
			REGISTER_CLASS(FinanceAnalyzerMarketMgr, FinanceAnalysis_Market);
			REGISTER_CLASS(FinanceAnalyzerStockMgr, FinanceAnalysis_Stock);
			init_map = true;
		}
		pthread_mutex_unlock(&mtx);
	}
}

FinanceAnalyzerInteractiveSession::FinanceAnalyzerInteractiveSession(int client_fd, sockaddr_in& client_sockaddress, int interactive_session_id, PIEVENT_NOTIFY parent) :
	pid(0),
	exit(0),
	sock_fd(client_fd),
	session_id(interactive_session_id),
	event_notify(parent),
	user_exit(false),
	sql_reader(NULL),
	finance_analysis_mode(FinanceAnalysis_None),
	manager(NULL),
	source_string_param(NULL),
	time_range_string_param(NULL),
	company_string_param(NULL),
	search_rule_need_reset(true),
	// search_rule_set(NULL),
	result_set_map(NULL),
	price_support_and_resistance_root_folderpath(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	init_command_map();
	memcpy(&client_sock, &client_sockaddress, sizeof(sockaddr_in));
	memset(session_tag, 0x0, sizeof(char) * 64);
	snprintf(session_tag, 64, "%s:%d", inet_ntoa(client_sock.sin_addr), htons(client_sock.sin_port));
}
	
FinanceAnalyzerInteractiveSession::~FinanceAnalyzerInteractiveSession()
{
	if (price_support_and_resistance_root_folderpath != NULL)
	{
		delete[] price_support_and_resistance_root_folderpath;
		price_support_and_resistance_root_folderpath = NULL;
	}
	if (sql_reader != NULL)
	{
		delete sql_reader;
		sql_reader = NULL;
	}
	if (manager != NULL)
	{
		delete manager;
		manager = NULL;
	}
	if (source_string_param != NULL)
	{
		delete[] source_string_param;
		source_string_param = NULL;
	}
	if (time_range_string_param != NULL)
	{
		delete[] time_range_string_param;
		time_range_string_param = NULL;
	}
	if (company_string_param != NULL)
	{
		delete[] company_string_param;
		company_string_param = NULL;
	}
	// if (search_rule_set != NULL)
	// {
	// 	delete search_rule_set;
	// 	search_rule_set = NULL;
	// }
	if (result_set_map != NULL)
	{
		delete result_set_map;
		result_set_map = NULL;
	}
	if (sock_fd > 0)
	{
		close(sock_fd);
		sock_fd = -1;
	}
	RELEASE_MSG_DUMPER()
}

unsigned short FinanceAnalyzerInteractiveSession::initialize()
{
	if (pthread_create(&pid, NULL, thread_handler, this) != 0)
	{
		WRITE_FORMAT_ERROR("Fail to create a handler thread of interactive session, due to: %s", strerror(errno));
		return RET_FAILURE_HANDLE_THREAD;
	}
	return RET_SUCCESS;
}

void FinanceAnalyzerInteractiveSession::notify_exit()
{
	WRITE_FORMAT_DEBUG("The worker thread of interactive session[%s] is notified to exit", session_tag);
	if (pid == 0)
		return;
	int kill_ret = pthread_kill(pid, 0);
	if(kill_ret == ESRCH)
	{
		WRITE_FORMAT_WARN("The worker thread of interactive session[%s] did NOT exist......", session_tag);
		// ret = RET_SUCCESS;
		return;
	}
	else if(kill_ret == EINVAL)
	{
		WRITE_FORMAT_ERROR("The signal to the worker thread of interactive session[%s] is invalid", session_tag);
		// ret = RET_FAILURE_HANDLE_THREAD;
		return;
	}
	WRITE_FORMAT_DEBUG("The signal to the worker thread of interactive session[%s] is STILL alive", session_tag);
// Notify the worker thread it's time to exit
	__sync_fetch_and_add(&exit, 1);

	void* status;
	WRITE_FORMAT_DEBUG("Wait for the worker thread of interactive session[%s]'s death......", session_tag);
	pthread_join(pid, (void**)&status);
// The return value of status is incorrect after the second thread die for unknown reason......
// 	if (status == NULL)
// 	{
// // WRITE_FORMAT_DEBUG can NOT called in this place, since the vfprintf function throws the exception for unknown reason
// 		// WRITE_FORMAT_DEBUG("The worker thread of interactive session[%s]'s is dead......", session_tag);
// 		WRITE_DEBUG("The seesion is closed successfully");
// 	}
// 	else
// 	{
// // WRITE_FORMAT_ERROR can NOT called in this place, since the vfprintf function throws the exception for unknown reason
// 		// WRITE_FORMAT_ERROR("Error occur in the worker thread of interactive session[%s], due to: %s", session_tag, (char*)status);
// 		WRITE_ERROR("Error occurs while trying to close the session");
// 	}
	WRITE_DEBUG("The session is closed......");
	pid = 0;
}

void* FinanceAnalyzerInteractiveSession::thread_handler(void* void_ptr)
{
	if (void_ptr == NULL)
		throw invalid_argument(string("void_ptr should NOT be NULL"));
	PFINANCE_ANALYZER_INTERACTIVE_SESSION this_ptr = (PFINANCE_ANALYZER_INTERACTIVE_SESSION)void_ptr;
	this_ptr->thread_handler_internal();
// STATIC_WRITE_FORMAT_DEBUG can NOT called in this place, since the vfprintf function throws the exception for unknown reason
	// STATIC_WRITE_FORMAT_DEBUG("The result in the worker thread of interactive session[%s]: %s", this_ptr->session_tag, get_ret_description(ret));
	// pthread_exit((CHECK_SUCCESS(ret) ? NULL : (void*)get_ret_description(ret)));
	pthread_exit(NULL);
}

unsigned short FinanceAnalyzerInteractiveSession::thread_handler_internal()
{
	WRITE_FORMAT_INFO("The worker thread of interactive session[%s] is running", session_tag);
	unsigned short ret = RET_SUCCESS;

	struct pollfd pollfds[1];
	char req_buf[REQ_BUF_SIZE];
	FILE* sock_fp = NULL;
	if ((sock_fp = fdopen(sock_fd, "a+")) == 0) 
	{
		WRITE_FORMAT_ERROR("Fail to transform FD to FP, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Initliaze the finance manager class
	ret = init_finance_manager(FinanceAnalysis_None);
	if (CHECK_FAILURE(ret))
		return ret;
// Print the welcome phrases
	print_to_console(welcome_phrases);
// Print the prompt
	print_prompt_to_console();
// Parse the command from user
	while(true)
	{
		if (exit > 0)
			break;
// Wait for the input event from user
      	memset(pollfds, 0, sizeof(pollfds));
      	pollfds[0].fd = sock_fd;
      	pollfds[0].events = POLLIN;
      	if (poll(pollfds, 1, 3) != 1) 
      	{
			usleep(100000);
			continue;
      	}
      	if ((pollfds[0].revents & POLLIN) != POLLIN)
		{
			continue;
			// WRITE_ERROR("The return event should be POLLIN");
			// return RET_FAILURE_SYSTEM_API;
		}
// Read the command and parse it
    	if (fgets(req_buf, REQ_BUF_SIZE, sock_fp) == NULL)
    		break;
     	// WRITE_FORMAT_DEBUG("Command Line: %s", req_buf);
// Parse the command
		char *command_line_outer = req_buf;
		char *rest_command_line_outer =  NULL;
		char *argv_outer[MAX_ARGC];
		int cur_argc_outer = 0;
		while ((argv_outer[cur_argc_outer] = strtok_r(command_line_outer, ";\t\n\r", &rest_command_line_outer)) != NULL)
		{
			// WRITE_FORMAT_DEBUG("Command Argument[Outer]: %s, rest: %s", argv_outer[cur_argc_outer], rest_command_line_outer);
			char *command_line_inner =  argv_outer[cur_argc_outer];
			char *rest_command_line_inner =  NULL;
			char *argv_inner[MAX_ARGC];
			int cur_argc_inner = 0;
			bool can_execute = true;
			while ((argv_inner[cur_argc_inner] = strtok_r(command_line_inner, " ", &rest_command_line_inner)) != NULL)
			{
				// WRITE_FORMAT_DEBUG("Command Argument[Inner]: %s, rest: %s", argv_inner[cur_argc_inner], rest_command_line_inner);
				if (command_line_inner != NULL)
				{
// Check if the command exist
					COMMAND_MAP::iterator iter = command_map.find(string(argv_inner[cur_argc_inner]));
					if (iter == command_map.end())
					{
						WRITE_FORMAT_ERROR("Error!! Unknown command: %s", argv_inner[0]);
						char unknown_command_error[64];
						snprintf(unknown_command_error, 64, "Unknown command: %s\n", argv_inner[0]);
						print_to_console(string(unknown_command_error));
						can_execute = false;
						break;
					}
					else
						WRITE_FORMAT_DEBUG("Try to execute the %s command......", argv_inner[0]);
					command_line_inner = NULL;
				}
				cur_argc_inner++;
			}
// Handle command
			if (can_execute)
			{
				ret = handle_command(cur_argc_inner, argv_inner);
				if (CHECK_FAILURE(ret))
				{
					char rsp_buf[RSP_BUF_SIZE];
					snprintf(rsp_buf, RSP_BUF_SIZE, "Error occurs while executing the %s command, due to: %s\n Close the session: %s\n", argv_inner[0], get_ret_description(ret), session_tag);
// Show warning if error occurs while executing the command and then exit
					WRITE_ERROR(rsp_buf);
					print_to_console(string(rsp_buf));
					return ret;				
				}
				else if (CHECK_WARN(ret))
				{
					static char rsp_buf[RSP_BUF_SIZE];
					snprintf(rsp_buf, RSP_BUF_SIZE, "Warning occurs while executing the %s command in the session: %s, due to: %s\n", argv_inner[0], session_tag, get_ret_description(ret));
// Show warning if warn occurs while executing the command
					WRITE_WARN(rsp_buf);
					print_to_console(string(rsp_buf));
					// return ret;	
				}
			}
			if (command_line_outer != NULL)
				command_line_outer = NULL;
			cur_argc_outer++;
		}
		if (exit == 0)
		{
// Print the prompt again
			print_prompt_to_console();
		}
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::print_to_console(string response)const
{
	const char* response_ptr = response.c_str();
	int response_size = response.size();
	int n;
	while (response_size > 0)
	{
		n = write(sock_fd, response_ptr, response_size);
		if (n < 0)
		{
			WRITE_FORMAT_ERROR("write() fails, due to: %s", strerror(errno));		
			return RET_FAILURE_SYSTEM_API;
		}
		else if(n < response_size)
		{
			response_ptr += n;
			response_size -= n;
		}
		else
			break;
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::print_prompt_to_console()const
{
	static string prompt(INTERACTIVE_PROMPT);
	return print_to_console(prompt);
}

unsigned short FinanceAnalyzerInteractiveSession::init_finance_manager(FinanceAnalysisMode new_finance_analysis_mode)
{
	if (finance_analysis_mode == FinanceAnalysis_None)
	{
		finance_analysis_mode = get_finance_analysis_mode_from_file();
	}
	else
	{
		if (finance_analysis_mode == new_finance_analysis_mode)
		{
			WRITE_FORMAT_DEBUG("No need to switch to the finance mode: %d", finance_analysis_mode);
			return RET_SUCCESS;
		}
// Release the old manager object
		if (manager != NULL)
		{
			delete manager;
			manager = NULL;
		}
		finance_analysis_mode = new_finance_analysis_mode;
	}
// Create the instance of the manager class due to different mode
	manager = mgr_factory.get_instance(finance_analysis_mode);
// Initialize the manager class
	WRITE_FORMAT_DEBUG("Initialize the finance manager: %d", finance_analysis_mode);
	unsigned short ret = manager->initialize();
	if (CHECK_FAILURE(ret))
	{
		WRITE_FORMAT_ERROR("Fail to initialize manager class, due to: %s", get_ret_description(ret));
		return ret;
	}
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_command(int argc, char **argv)
{
	typedef unsigned short (FinanceAnalyzerInteractiveSession::*handle_command_func_ptr)(int argc, char**argv);
	static handle_command_func_ptr handle_command_func_array[] =
	{
		&FinanceAnalyzerInteractiveSession::handle_get_finance_mode_command,
		&FinanceAnalyzerInteractiveSession::handle_set_finance_mode_command,
		&FinanceAnalyzerInteractiveSession::handle_get_source_command,
		&FinanceAnalyzerInteractiveSession::handle_set_source_command,
		&FinanceAnalyzerInteractiveSession::handle_get_time_range_command,
		&FinanceAnalyzerInteractiveSession::handle_set_time_range_command,
		&FinanceAnalyzerInteractiveSession::handle_get_company_command,
		&FinanceAnalyzerInteractiveSession::handle_set_company_command,
		&FinanceAnalyzerInteractiveSession::handle_search_command,
		&FinanceAnalyzerInteractiveSession::handle_set_stock_support_resistance_filepath_command,
		&FinanceAnalyzerInteractiveSession::handle_find_stock_support_resistance_command,
		&FinanceAnalyzerInteractiveSession::handle_help_command,
		&FinanceAnalyzerInteractiveSession::handle_exit_command
	};
	// assert (iter != command_map.end() && "Unknown command");
	COMMAND_MAP::iterator iter = command_map.find(string(argv[0]));
	int command_type = (int)iter->second;
	return (this->*(handle_command_func_array[command_type]))(argc, argv);
}

unsigned short FinanceAnalyzerInteractiveSession::handle_get_finance_mode_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get the finance mode
	static char rsp_buf[RSP_BUF_SHORT_SIZE];
	snprintf(rsp_buf, RSP_BUF_SHORT_SIZE, "\nFinance Mode: %s\n", FINANCE_MODE_DESCRIPTION[finance_analysis_mode]);
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_set_finance_mode_command(int argc, char **argv)
{
	unsigned short ret = RET_SUCCESS;
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Set the finance mode
	FinanceAnalysisMode new_finance_analysis_mode = FinanceAnalysis_None;
	try
	{		
		int new_finance_analysis_mode_value = atoi(argv[1]);
		if (new_finance_analysis_mode_value < 0 || new_finance_analysis_mode_value >= FinanceAnalysisSize)
			throw invalid_argument(string("Unsupported finance mode"));
		new_finance_analysis_mode = (FinanceAnalysisMode)new_finance_analysis_mode_value;
	}
	catch(exception &e)
	{
		char rsp_buf[RSP_BUF_SIZE];
		snprintf(rsp_buf, RSP_BUF_SIZE, INCORRECT_COMMAND_ARGUMENT_FORMAT, argv[0], argv[1]);
		WRITE_ERROR(rsp_buf);
		print_to_console(string(rsp_buf) + string("\n"));
		return RET_WARN_INTERACTIVE_COMMAND;
	}

	ret = init_finance_manager(new_finance_analysis_mode);
	if (CHECK_FAILURE(ret))
		return ret;
	return ret;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_get_source_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get the source type
	static char rsp_buf[RSP_BUF_SHORT_SIZE];
	if (source_string_param != NULL)
		snprintf(rsp_buf, RSP_BUF_SHORT_SIZE, "\nSource Type: %s\n", source_string_param);
	else
		snprintf(rsp_buf, RSP_BUF_SHORT_SIZE, "\nSource Type: Not Set\n");
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_set_source_command(int argc, char **argv)
{
	// unsigned short ret = RET_SUCCESS;
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Set the source type
	if (source_string_param != NULL)
	{
		WRITE_FORMAT_DEBUG("Cleanup the old source type: %s", source_string_param);
		delete[] source_string_param;
		source_string_param = NULL;
	}
	search_rule_need_reset = true;
	int source_string_param_size = strlen(argv[1]) + 1;
	source_string_param = new char[source_string_param_size];
	if (source_string_param == NULL)
		throw bad_alloc();
	memcpy(source_string_param, argv[1], sizeof(char) * source_string_param_size);
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_get_time_range_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get the time range
	static char rsp_buf[RSP_BUF_SHORT_SIZE];
	if (time_range_string_param != NULL)
		snprintf(rsp_buf, RSP_BUF_SHORT_SIZE, "\nTime Range: %s\n", time_range_string_param);
	else
		snprintf(rsp_buf, RSP_BUF_SHORT_SIZE, "\nTime Range: Not Set\n");
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_set_time_range_command(int argc, char **argv)
{
	// unsigned short ret = RET_SUCCESS;
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Set the time range
	if (time_range_string_param != NULL)
	{
		WRITE_FORMAT_DEBUG("Cleanup the old time range: %s", time_range_string_param);
		delete[] time_range_string_param;
		time_range_string_param = NULL;
	}
	search_rule_need_reset = true;
	int time_range_string_param_size = strlen(argv[1]) + 1;
	time_range_string_param = new char[time_range_string_param_size];
	if (time_range_string_param == NULL)
		throw bad_alloc();
	memcpy(time_range_string_param, argv[1], sizeof(char) * time_range_string_param_size);
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_get_company_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get the company
	static char rsp_buf[RSP_BUF_SIZE];
	if (company_string_param != NULL)
		snprintf(rsp_buf, RSP_BUF_SIZE, "\nCompany: %s\n", company_string_param);
	else
		snprintf(rsp_buf, RSP_BUF_SIZE, "\nCompany: Not Set\n");
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_set_company_command(int argc, char **argv)
{
	// unsigned short ret = RET_SUCCESS;
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Set the company
	if (company_string_param != NULL)
	{
		WRITE_FORMAT_DEBUG("Cleanup the old company: %s", company_string_param);
		delete[] company_string_param;
		company_string_param = NULL;
	}
	search_rule_need_reset = true;
	int company_string_param_size = strlen(argv[1]) + 1;
	company_string_param = new char[company_string_param_size];
	if (company_string_param == NULL)
		throw bad_alloc();
	memcpy(company_string_param, argv[1], sizeof(char) * company_string_param_size);
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_search_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (search_rule_need_reset)
	{
		if (sql_reader == NULL)
		{
			sql_reader = new FinanceAnalyzerSqlReader();
			if (sql_reader == NULL)
				throw bad_alloc();
		}
		if (result_set_map != NULL)
			delete result_set_map;
		result_set_map = new ResultSetMap();
		if (result_set_map == NULL)
			throw bad_alloc();
		unsigned short ret = RET_SUCCESS;
		SearchRuleSet search_rule_set;
// Set search rule
// The create_instance_from_string() function probably returns the WARN code
		ret = SearchRuleSet::create_instance_from_string(finance_analysis_mode, source_string_param, time_range_string_param, company_string_param, search_rule_set);
		if (!CHECK_SUCCESS(ret))
			return ret;
// Query the data
		ret = FinanceAnalyzerSqlReader::query(&search_rule_set, sql_reader, result_set_map);
		if (CHECK_FAILURE(ret))
			return ret;
		search_rule_need_reset = false;
	}
	// assert(result_set_map != NULL && "result_set_map should NOT be NULL");
	if (result_set_map != NULL)
		print_to_console(result_set_map->to_string());
	else
		print_to_console("Empty");
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_set_stock_support_resistance_filepath_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (price_support_and_resistance_root_folderpath != NULL)
	{
		delete[] price_support_and_resistance_root_folderpath;
		price_support_and_resistance_root_folderpath = NULL;
	}
	int price_support_and_resistance_root_folderpath_len = strlen(argv[1]);
	price_support_and_resistance_root_folderpath = new char[price_support_and_resistance_root_folderpath_len + 1];
	if (price_support_and_resistance_root_folderpath == NULL)
		throw bad_alloc();
	memcpy(price_support_and_resistance_root_folderpath, argv[1], sizeof(char) * (price_support_and_resistance_root_folderpath_len + 1));
	return RET_SUCCESS;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_find_stock_support_resistance_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	char *stock_support_resistance_entry = argv[0];
	string price_support_and_resistance_result;
// Parse the data 
	char* comma_pos = strchr(stock_support_resistance_entry, ':');
	if (comma_pos == NULL)
	{
		static char rsp_buf[RSP_BUF_SIZE];
		snprintf(rsp_buf, RSP_BUF_SIZE, "Incorrect argument format for finding stock support and resistance: %s", stock_support_resistance_entry);
		WRITE_WARN(rsp_buf);
		print_to_console(rsp_buf);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Find the company number and its close price
	int comma_pos_index = comma_pos - stock_support_resistance_entry;
	const int BUF_SIZE = 8;
	char company_number_buf[BUF_SIZE];
	char stock_close_price_buf[BUF_SIZE];
	memset(company_number_buf, 0x0, sizeof(char) * BUF_SIZE);
	memset(stock_close_price_buf, 0x0, sizeof(char) * BUF_SIZE);
	memcpy(company_number_buf, stock_support_resistance_entry, sizeof(char) * comma_pos_index);
	memcpy(stock_close_price_buf, &stock_support_resistance_entry[comma_pos_index + 1], sizeof(char) * (strlen(stock_support_resistance_entry) - (comma_pos_index + 1)));
	float stock_close_price = atof(stock_close_price_buf);
	unsigned short ret = RET_SUCCESS;
// Analyze data from each stock
	price_support_and_resistance_result += (string("==================") + string(company_number_buf) + string("==================\nClose Price: ") + string(stock_close_price_buf) + string("\n"));
	string price_support_resistance_string;
	ret = manager->get_stock_price_support_resistance_string(string(company_number_buf), stock_close_price, price_support_resistance_string, price_support_and_resistance_root_folderpath);
	if (CHECK_FAILURE(ret))
	{
		if (FAILURE_IS_NOT_FOUND(ret))
		{
			price_support_and_resistance_result += "No Data\n\n";
			WRITE_FORMAT_WARN("WARNING: The %s support resistance file does NOT exist in %s", company_number_buf, price_support_and_resistance_root_folderpath);
		}
		else
		{
			WRITE_FORMAT_ERROR("Fail to find the support and resistance of the company: %s, due to: %s", company_number_buf, get_ret_description(ret));
			return ret;
		}
	}
	else
		price_support_and_resistance_result += (price_support_resistance_string + string("\n")) ;
	ret = print_to_console(price_support_and_resistance_result);
	return ret;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_help_command(int argc, char **argv)
{
	static const int BUF_SIZE = 256;
	static char buf[BUF_SIZE];
	unsigned short ret = RET_SUCCESS;
	string usage_string;
	usage_string += string("====================== Usage ======================\n");
// Finance Mode
	usage_string += string("* get_finance_mode\n Description: Get finance mode\n");
	usage_string += string("* set_finance_mode\n Description: Set finance mode\n");
	usage_string += string(" Finance mode:\n");
	for(int i = 0 ; i < FinanceAnalysisSize ; i++)
	{
		snprintf(buf, BUF_SIZE, "  %s: %d\n", FINANCE_MODE_DESCRIPTION[i], i);
		usage_string += string(buf);
	}
	usage_string += string("* help\n Description: The usage\n");
// Source type
	usage_string += string("* get_source\nDescription: Get list of source type\n");
	usage_string += string("* set_source\nDescription: Set list of source type\n");
	int source_type_start_index, source_type_end_index;
	get_source_type_index_range(source_type_start_index, source_type_end_index, finance_analysis_mode);
	for (int i = source_type_start_index ; i < source_type_end_index ; i++)
	{
		snprintf(buf, BUF_SIZE, "  %s: %d\n", FINANCE_DATABASE_DESCRIPTION_LIST[i], i);
		usage_string += string(buf);
	}
	usage_string += string("  Format 1: All source types/fields (ex. all)\n");	
	usage_string += string("  Format 2: Source type index/index range (ex. 1,2-4,6)\n");
	usage_string += string("  Format 3: Source type index/index range with field index/index range  (ex. 1(1-2;4),2-4(2-4;5),5,6(1;3;5-7))\n");
// Time range
	usage_string += string("* get_time_range\nDescription: Get time range\n");
	usage_string += string("* set_time_range\nDescription: Set time range\n");
	usage_string += string("  Format 1: Start time: (ex. 2015-01-01)\n");
	usage_string += string("  Format 2: Start time,End time: (ex. 2015-01-01,2015-09-04)\n");
	usage_string += string("  Format 3: ,End time: (ex. ,2015-09-04)\n");
// Company
	if (finance_analysis_mode == FinanceAnalysis_Stock)
	{
		usage_string += string("* get_company\nDescription: Get list of company code number\n");
		usage_string += string("* set_company\nDescription: Set list of company code number\n");
		usage_string += string("  Format 1: Company code number (ex. 2347)\n");
		usage_string += string("  Format 2: Company code number range (ex. 2100-2200)\n");
		usage_string += string("  Format 3: Company group number (ex. [Gg]12)\n");
		usage_string += string("  Format 4: Company group number range (ex. [Gg]12-15)\n");
		usage_string += string("  Format 5: Company code number/number range/group/group range hybrid (ex. 2347,2100-2200,G12,2362,g2,1500-1510)\n");
		usage_string += string("* find_stock_support_resistance\nDescription: Find the stock support and resistance of a specific company\n");
		usage_string += string("  Format: Company code number:Stock close price Pair(ex. 1560:77.8)\n");
		// usage_string += string("  Format 2: Company code number:Stock close price Pair List(ex. 1560:77.8,1589:81.9,1215:67)\nCaution: Max up to %d stock entry once", MAX_STOCK_SUPPORT_RESISTANCE_AMOUNT);
	}
// Search
	if (finance_analysis_mode == FinanceAnalysis_Market)
		usage_string += string("* search\n Description: Search the database under the rule of source type and time range\n");
	else if (finance_analysis_mode == FinanceAnalysis_Stock)
		usage_string += string("* search\n Description: Search the database under the rule of source type, time range and company number\n");
	usage_string += string("===================================================\n");

	ret = print_to_console(usage_string);
	return ret;
}

unsigned short FinanceAnalyzerInteractiveSession::handle_exit_command(int argc, char **argv)
{
	print_to_console(string("Bye bye !!!"));
	event_notify->notify(InteractiveSessionEvent_Exit, (void*)&session_id);
	return RET_SUCCESS;
}