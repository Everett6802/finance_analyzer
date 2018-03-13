#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <map>
// #include <string>
#include "interactive_session.h"


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
	InteractiveSessionCommand_GetDataType,
	InteractiveSessionCommand_SetDataType,
	InteractiveSessionCommand_GetContinueWhenNotExist,
	InteractiveSessionCommand_SetContinueWhenNotExist,
	InteractiveSessionCommand_GetCsvRootFolderPath,
	InteractiveSessionCommand_SetCsvRootFolderPath,
	InteractiveSessionCommand_GetShmRootFolderPath,
	InteractiveSessionCommand_SetShmRootFolderPath,
	InteractiveSessionCommand_Search,
	InteractiveSessionCommand_ResetSearchParam,
	InteractiveSessionCommand_SetStockSupportResistanceFilepath,
	InteractiveSessionCommand_SetStockSupportResistanceVerbose,
	InteractiveSessionCommand_SeteStockSupportResistanceDateFilter,
	InteractiveSessionCommand_SetStockSupportResistanceVolumeFilter,
	InteractiveSessionCommand_GetStockSupportResistanceParameters,
	InteractiveSessionCommand_CleanupStockSupportResistanceParameters,
	InteractiveSessionCommand_FindStockSupportResistance,
	InteractiveSessionCommand_Help,
	InteractiveSessionCommand_Exit,
	InteractiveSessionCommandSize
};

static const char *interactive_session_command[InteractiveSessionCommandSize] = 
{
	"get_finance_mode",
	"set_finance_mode",
	"get_method",
	"set_method",
	"get_time_range",
	"set_time_range",
	"get_company",
	"set_company",
	"get_data_type",
	"set_data_type",
	"get_continue_when_non_exist",
	"set_continue_when_non_exist",
	"get_csv_root_folderpath",
	"set_csv_root_folderpath",
	"get_shm_root_folderpath",
	"set_shm_root_folderpath",
	"search",
	"reset_search_param",
	"set_stock_support_resistance_filepath",
	"set_stock_support_resistance_verbose",
	"set_stock_support_resistance_date_filter",
	"set_stock_support_resistance_volume_filter",
	"get_stock_support_resistance_parameters",
	"cleanup_stock_support_resistance_parameters",
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

const int InteractiveSession::REQ_BUF_SIZE = 1024;
const int InteractiveSession::RSP_BUF_VERY_SHORT_SIZE = 32;
const int InteractiveSession::RSP_BUF_SHORT_SIZE = 64;
const int InteractiveSession::RSP_BUF_SIZE = 256;
const int InteractiveSession::RSP_BUF_LONG_SIZE = 1024;
const int InteractiveSession::RSP_BUF_VERY_LONG_SIZE = 4096;
const int InteractiveSession::MAX_ARGC = 20;
MgrFactory InteractiveSession::mgr_factory;

void InteractiveSession::init_command_map()
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
			// static MgrFactory mgr_factory;
			REGISTER_CLASS(FinanceAnalyzerMarketMgr, FinanceAnalysis_Market);
			REGISTER_CLASS(FinanceAnalyzerStockMgr, FinanceAnalysis_Stock);
			init_map = true;
		}
		pthread_mutex_unlock(&mtx);
	}
}

InteractiveSession::InteractiveSession(int client_fd, sockaddr_in& client_sockaddress, int interactive_session_id, PIEVENT_NOTIFY parent) :
	pid(0),
	exit(0),
	sock_fd(client_fd),
	session_id(interactive_session_id),
	event_notify(parent),
	user_exit(false),
	// sql_reader(NULL),
	finance_analysis_mode(FinanceAnalysis_None),
	manager(NULL),
	source_string_param(NULL),
	time_range_string_param(NULL),
	company_string_param(NULL),
	finance_data_type(FinanceData_SQL),
	continue_when_non_exist(DEFAULT_CONTINUE_WHEN_NOT_EXIST),
	csv_root_folderpath(NULL),
	shm_root_folderpath(NULL),
	// source_param(NULL),
	// data_sql_reader_param(NULL),
	// data_csv_reader_param(NULL),
	search_rule_changed(false),
	source_param_not_default(false),
	show_stock_support_resistance_verbose(false),
	stock_support_resistance_date_filter(NULL),
	stock_support_resistance_volume_filter(NULL),
	// search_rule_set(NULL),
	result_set_map(NULL),
	stock_support_and_resistance_root_folderpath(NULL)
{
	IMPLEMENT_MSG_DUMPER()
	IMPLEMENT_DATA_READER()
	init_command_map();
	memcpy(&client_sock, &client_sockaddress, sizeof(sockaddr_in));
	memset(session_tag, 0x0, sizeof(char) * 64);
	snprintf(session_tag, 64, "%s:%d", inet_ntoa(client_sock.sin_addr), htons(client_sock.sin_port));
	// data_source_param_array[FinanceData_SQL] = (void*)data_sql_reader_param;
	// data_source_param_array[FinanceData_CSV] = (void*)data_csv_reader_param;
	// data_sql_reader_param = {.continue_when_non_exist = false};
	// data_csv_reader_param = {.root_folderpath = NULL, .continue_when_non_exist = false};
}
	
InteractiveSession::~InteractiveSession()
{
	// if (data_csv_reader_param.root_folderpath != NULL)
	// {
	// 	delete[] data_csv_reader_param.root_folderpath;
	// 	data_csv_reader_param.root_folderpath = NULL;
	// }
	if (stock_support_and_resistance_root_folderpath != NULL)
	{
		delete[] stock_support_and_resistance_root_folderpath;
		stock_support_and_resistance_root_folderpath = NULL;
	}
	if (stock_support_resistance_volume_filter)
	{
		delete[] stock_support_resistance_volume_filter;
		stock_support_resistance_volume_filter = NULL;
	}
	if (stock_support_resistance_date_filter)
	{
		delete[] stock_support_resistance_date_filter;
		stock_support_resistance_date_filter = NULL;
	}
	// if (sql_reader != NULL)
	// {
	// 	delete sql_reader;
	// 	sql_reader = NULL;
	// }
	if (manager != NULL)
	{
		delete manager;
		manager = NULL;
	}
	reset_search_param();
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
	RELEASE_DATA_READER()
	RELEASE_MSG_DUMPER()
}

unsigned short InteractiveSession::initialize()
{
	if (pthread_create(&pid, NULL, thread_handler, this) != 0)
	{
		WRITE_FORMAT_ERROR("Fail to create a handler thread of interactive session, due to: %s", strerror(errno));
		return RET_FAILURE_HANDLE_THREAD;
	}
	return RET_SUCCESS;
}

void InteractiveSession::notify_exit()
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

void* InteractiveSession::thread_handler(void* void_ptr)
{
	if (void_ptr == NULL)
		throw invalid_argument(string("void_ptr should NOT be NULL"));
	PINTERACTIVE_SESSION this_ptr = (PINTERACTIVE_SESSION)void_ptr;
	this_ptr->thread_handler_internal();
// STATIC_WRITE_FORMAT_DEBUG can NOT called in this place, since the vfprintf function throws the exception for unknown reason
	// STATIC_WRITE_FORMAT_DEBUG("The result in the worker thread of interactive session[%s]: %s", this_ptr->session_tag, get_ret_description(ret));
	// pthread_exit((CHECK_SUCCESS(ret) ? NULL : (void*)get_ret_description(ret)));
	pthread_exit(NULL);
}

unsigned short InteractiveSession::thread_handler_internal()
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

unsigned short InteractiveSession::print_single_stock_support_resistance_string(const char* stock_support_resistance_entry, int stock_support_resistance_entry_len)
{
	assert(stock_support_resistance_entry != NULL && "stock_support_resistance_entry should NOT be NULL");
	static const int ENTRY_BUF_SIZE = 32;
	static char entry_buf[ENTRY_BUF_SIZE];
	memset(entry_buf, 0x0, sizeof(char) * ENTRY_BUF_SIZE);
	memcpy(entry_buf, stock_support_resistance_entry, sizeof(char) * stock_support_resistance_entry_len);
	string stock_support_resistance_result;
// Parse the data 
	const char* comma_pos = strchr(entry_buf, ':');
	if (comma_pos == NULL)
	{
		static char rsp_buf[RSP_BUF_SIZE];
		snprintf(rsp_buf, RSP_BUF_SIZE, "Incorrect argument format for finding stock support and resistance: %s", entry_buf);
		WRITE_WARN(rsp_buf);
		print_to_console(rsp_buf);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Find the company number and its close price
	int comma_pos_index = comma_pos - entry_buf;
	const int BUF_SIZE = 8;
	char company_number_buf[BUF_SIZE];
	char stock_close_price_buf[BUF_SIZE];
	memset(company_number_buf, 0x0, sizeof(char) * BUF_SIZE);
	memset(stock_close_price_buf, 0x0, sizeof(char) * BUF_SIZE);
	memcpy(company_number_buf, entry_buf, sizeof(char) * comma_pos_index);
	memcpy(stock_close_price_buf, &entry_buf[comma_pos_index + 1], sizeof(char) * (strlen(entry_buf) - (comma_pos_index + 1)));
	float stock_close_price = atof(stock_close_price_buf);
	unsigned short ret = RET_SUCCESS;
// Analyze data from each stock
	stock_support_resistance_result += (string("==================") + string(company_number_buf) + string("==================\nClose Price: ") + string(stock_close_price_buf) + string("\n"));
	string stock_support_resistance_string;
	ret = manager->get_stock_support_resistance_string(string(company_number_buf), stock_close_price, stock_support_resistance_string, stock_support_and_resistance_root_folderpath, show_stock_support_resistance_verbose, stock_support_resistance_date_filter, stock_support_resistance_volume_filter);
	if (CHECK_FAILURE(ret))
	{
		if (FAILURE_IS_NOT_FOUND(ret))
		{
			stock_support_resistance_result += "No Data\n\n";
			WRITE_FORMAT_WARN("WARNING: The %s support resistance file does NOT exist in %s", company_number_buf, stock_support_and_resistance_root_folderpath);
		}
		else
		{
			WRITE_FORMAT_ERROR("Fail to find the support and resistance of the company: %s, due to: %s", company_number_buf, get_ret_description(ret));
			return ret;
		}
	}
	else
		stock_support_resistance_result += (stock_support_resistance_string + string("\n")) ;
	ret = print_to_console(stock_support_resistance_result);
	return ret;
}

void InteractiveSession::reset_search_param()
{
	// for (int i = 0 ; i < FinanceDataSize ; i++)
	// {
	// 	if (data_source_param_array[i] != NULL)
	// 	{
	// 		delete data_source_param_array[i];
	// 		data_source_param_array[i] = NULL;	
	// 	}	
	// }
	// data_source_param_array[FinanceData_SQL] = NULL;
	// data_source_param_array[FinanceData_CSV] = NULL;
	// if (data_sql_reader_param != NULL)
	// {
	// 	delete data_sql_reader_param;
	// 	data_sql_reader_param = NULL;
	// }
	// if (data_csv_reader_param != NULL)
	// {
	// 	delete data_csv_reader_param;
	// 	data_csv_reader_param = NULL;
	// }
	// if (source_param != NULL)
	// {
	// 	delete source_param;
	// 	source_param = NULL;
	// }
	source_param_not_default = false;
	search_rule_changed = false;
	if (shm_root_folderpath != NULL)
	{
		delete[] shm_root_folderpath;
		shm_root_folderpath = NULL;
	}
	if (csv_root_folderpath != NULL)
	{
		delete[] csv_root_folderpath;
		csv_root_folderpath = NULL;
	}
	continue_when_non_exist = DEFAULT_CONTINUE_WHEN_NOT_EXIST;
	if (company_string_param != NULL)
	{
		delete[] company_string_param;
		company_string_param = NULL;
	}
	if (time_range_string_param != NULL)
	{
		delete[] time_range_string_param;
		time_range_string_param = NULL;
	}
	if (source_string_param != NULL)
	{
		delete[] source_string_param;
		source_string_param = NULL;
	}
}

unsigned short InteractiveSession::print_to_console(string response)const
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

unsigned short InteractiveSession::print_prompt_to_console()const
{
	static string prompt(INTERACTIVE_PROMPT);
	return print_to_console(prompt);
}

unsigned short InteractiveSession::init_finance_manager(FinanceAnalysisMode new_finance_analysis_mode)
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

unsigned short InteractiveSession::handle_command(int argc, char **argv)
{
	typedef unsigned short (InteractiveSession::*handle_command_func_ptr)(int argc, char**argv);
	static handle_command_func_ptr handle_command_func_array[] =
	{
		&InteractiveSession::handle_get_finance_mode_command,
		&InteractiveSession::handle_set_finance_mode_command,
		&InteractiveSession::handle_get_method_command,
		&InteractiveSession::handle_set_method_command,
		&InteractiveSession::handle_get_time_range_command,
		&InteractiveSession::handle_set_time_range_command,
		&InteractiveSession::handle_get_company_command,
		&InteractiveSession::handle_set_company_command,
		&InteractiveSession::handle_get_data_type_command,
		&InteractiveSession::handle_set_data_type_command,
		&InteractiveSession::handle_get_continue_when_non_exist_command,
		&InteractiveSession::handle_set_continue_when_non_exist_command,
		&InteractiveSession::handle_get_csv_root_folderpath_command,
		&InteractiveSession::handle_set_csv_root_folderpath_command,
		&InteractiveSession::handle_get_shm_root_folderpath_command,
		&InteractiveSession::handle_set_shm_root_folderpath_command,
		&InteractiveSession::handle_search_command,
		&InteractiveSession::handle_reset_search_param_command,
		&InteractiveSession::handle_set_stock_support_resistance_filepath_command,
		&InteractiveSession::handle_set_stock_support_resistance_verbose_command,
		&InteractiveSession::handle_set_stock_support_resistance_date_filter_command,
		&InteractiveSession::handle_set_stock_support_resistance_volume_filter_command,
		&InteractiveSession::handle_get_stock_support_resistance_parameters_command,
		&InteractiveSession::handle_cleanup_stock_support_resistance_parameters_command,
		&InteractiveSession::handle_find_stock_support_resistance_command,
		&InteractiveSession::handle_help_command,
		&InteractiveSession::handle_exit_command
	};
	// assert (iter != command_map.end() && "Unknown command");
	COMMAND_MAP::iterator iter = command_map.find(string(argv[0]));
	int command_type = (int)iter->second;
	return (this->*(handle_command_func_array[command_type]))(argc, argv);
}

unsigned short InteractiveSession::handle_get_finance_mode_command(int argc, char **argv)
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

unsigned short InteractiveSession::handle_set_finance_mode_command(int argc, char **argv)
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

unsigned short InteractiveSession::handle_get_method_command(int argc, char **argv)
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

unsigned short InteractiveSession::handle_set_method_command(int argc, char **argv)
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
	search_rule_changed = true;
	int source_string_param_size = strlen(argv[1]) + 1;
	source_string_param = new char[source_string_param_size];
	if (source_string_param == NULL)
		throw bad_alloc();
	memcpy(source_string_param, argv[1], sizeof(char) * source_string_param_size);
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_time_range_command(int argc, char **argv)
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

unsigned short InteractiveSession::handle_set_time_range_command(int argc, char **argv)
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
	search_rule_changed = true;
	int time_range_string_param_size = strlen(argv[1]) + 1;
	time_range_string_param = new char[time_range_string_param_size];
	if (time_range_string_param == NULL)
		throw bad_alloc();
	memcpy(time_range_string_param, argv[1], sizeof(char) * time_range_string_param_size);
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_company_command(int argc, char **argv)
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

unsigned short InteractiveSession::handle_set_company_command(int argc, char **argv)
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
	search_rule_changed = true;
	int company_string_param_size = strlen(argv[1]) + 1;
	company_string_param = new char[company_string_param_size];
	if (company_string_param == NULL)
		throw bad_alloc();
	memcpy(company_string_param, argv[1], sizeof(char) * company_string_param_size);
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_data_type_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get the source data type
	static char rsp_buf[RSP_BUF_SIZE];
	snprintf(rsp_buf, RSP_BUF_SIZE, "\nData Type: %d(%s)\n", finance_data_type, FINANCE_DATA_DESCRIPTION[finance_data_type]);
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_data_type_command(int argc, char **argv)
{
	// unsigned short ret = RET_SUCCESS;
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Set the source type
	int new_finance_data_type_value = FinanceDataSize;
	try
	{
		new_finance_data_type_value = atoi(argv[1]);
		if (new_finance_data_type_value < 0 || new_finance_data_type_value >= FinanceDataSize)
			throw invalid_argument(string("Unsupported data reader type"));
	}
	catch(exception &e)
	{
		char rsp_buf[RSP_BUF_SIZE];
		snprintf(rsp_buf, RSP_BUF_SIZE, INCORRECT_COMMAND_ARGUMENT_FORMAT, argv[0], argv[1]);
		WRITE_ERROR(rsp_buf);
		print_to_console(string(rsp_buf) + string("\n"));
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (finance_data_type != (FinanceDataType)new_finance_data_type_value)
	{
		finance_data_type = (FinanceDataType)new_finance_data_type_value;
		search_rule_changed = true;
	}
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_continue_when_non_exist_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get the flag of continuing when non-exist
	static char rsp_buf[RSP_BUF_SIZE];
	snprintf(rsp_buf, RSP_BUF_SIZE, "\nContinue When Non-Exist: %s\n", (continue_when_non_exist ? "True" : "False"));
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_continue_when_non_exist_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	bool new_continue_when_non_exist;
	try
	{		
		int continue_when_non_exist_int = atoi(argv[1]);
		if (continue_when_non_exist_int < 0 || continue_when_non_exist_int >= 1)
			throw invalid_argument(string("The value of continue_when_non_exist should be 0 or 1"));
		new_continue_when_non_exist = (bool)continue_when_non_exist_int;
	}
	catch(exception &e)
	{
		char rsp_buf[RSP_BUF_SIZE];
		snprintf(rsp_buf, RSP_BUF_SIZE, INCORRECT_COMMAND_ARGUMENT_FORMAT, argv[0], argv[1]);
		WRITE_ERROR(rsp_buf);
		print_to_console(string(rsp_buf) + string("\n"));
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (continue_when_non_exist != new_continue_when_non_exist)
	{
		continue_when_non_exist = new_continue_when_non_exist;
		search_rule_changed = true;
	}
// Check if default value
	if (continue_when_non_exist != DEFAULT_CONTINUE_WHEN_NOT_EXIST)
		source_param_not_default = true;
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_csv_root_folderpath_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get CSV root folerpath
	static char rsp_buf[RSP_BUF_SIZE];
	snprintf(rsp_buf, RSP_BUF_SIZE, "\nCSV Root Folder Path: %s\n", csv_root_folderpath);
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_csv_root_folderpath_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (finance_data_type != FinanceData_CSV)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect source type, expected: %s, actual: %s", FINANCE_DATA_DESCRIPTION[FinanceData_CSV], FINANCE_DATA_DESCRIPTION[finance_data_type]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (csv_root_folderpath != NULL)
	{
		delete[] csv_root_folderpath;
		csv_root_folderpath = NULL;
	}
// Set CSV root folerpath
	int csv_root_folderpath_len = strlen(argv[1]);
	csv_root_folderpath = new char[csv_root_folderpath_len + 1];
	if (csv_root_folderpath == NULL)
		throw bad_alloc();
	memcpy(csv_root_folderpath, argv[1], sizeof(char) * (csv_root_folderpath_len + 1));
	WRITE_FORMAT_DEBUG("The new CSV root folderpath: %s", csv_root_folderpath);
	search_rule_changed = true;
// Check if default value
	if (strcmp(csv_root_folderpath, DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH) != 0)
		source_param_not_default = true;
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_shm_root_folderpath_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
// Get SHM root folerpath
	static char rsp_buf[RSP_BUF_SIZE];
	snprintf(rsp_buf, RSP_BUF_SIZE, "\nSHM Root Folder Path: %s\n", shm_root_folderpath);
	print_to_console(string(rsp_buf));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_shm_root_folderpath_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (finance_data_type != FinanceData_SHM)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect source type, expected: %s, actual: %s", FINANCE_DATA_DESCRIPTION[FinanceData_SHM], FINANCE_DATA_DESCRIPTION[finance_data_type]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (shm_root_folderpath != NULL)
	{
		delete[] shm_root_folderpath;
		shm_root_folderpath = NULL;
	}
// Set SHM root folerpath
	int shm_root_folderpath_len = strlen(argv[1]);
	shm_root_folderpath = new char[shm_root_folderpath_len + 1];
	if (shm_root_folderpath == NULL)
		throw bad_alloc();
	memcpy(shm_root_folderpath, argv[1], sizeof(char) * (shm_root_folderpath_len + 1));
	WRITE_FORMAT_DEBUG("The new SHM root folderpath: %s", shm_root_folderpath);
	search_rule_changed = true;
// Check if default value
	if (strcmp(shm_root_folderpath, DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH) != 0)
		source_param_not_default = true;
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_search_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (search_rule_changed)
	{
		// if (sql_reader == NULL)
		// {
		// 	sql_reader = new SqlReader();
		// 	if (sql_reader == NULL)
		// 		throw bad_alloc();
		// }
		if (result_set_map != NULL)
			delete result_set_map;
		result_set_map = new ResultSetMap(finance_data_type);
		if (result_set_map == NULL)
			throw bad_alloc();
		unsigned short ret = RET_SUCCESS;
		SearchRuleSet search_rule_set;
// Set search rule
// The create_instance_from_string() function probably returns the WARN code
		ret = SearchRuleSet::create_instance_from_string(finance_analysis_mode, finance_data_type, source_string_param, time_range_string_param, company_string_param, search_rule_set);
		if (!CHECK_SUCCESS(ret))
			return ret;
// Query the data
		// ret = SqlReader::query(&search_rule_set, sql_reader, result_set_map);
		if (source_param_not_default)
		{
// Setup the parameters before search
			PISOURCE_PARAM source_param = NULL;
			switch(finance_data_type)
			{
				case FinanceData_SQL:
				{
					source_param = new SqlSourceParam(continue_when_non_exist);
				}
				break;
				case FinanceData_CSV:
				{
					source_param = new CsvSourceParam(continue_when_non_exist, csv_root_folderpath);
				}
				break;
				case FinanceData_SHM:
				{
					source_param = new ShmSourceParam(continue_when_non_exist, shm_root_folderpath);
				}
				break;
				default:
				{
					static int ERRMSG_SIZE = 256;
					char errmsg[ERRMSG_SIZE];
					snprintf(errmsg, ERRMSG_SIZE, "Unknown finance data type: %d", finance_data_type);
					throw runtime_error(string(errmsg));
				}
				break;
			}
			if (source_param == NULL)
				throw bad_alloc();
			ret = DATA_READ_BY_PARAM(finance_data_type, &search_rule_set, source_param, result_set_map);
			delete source_param;
		}
		else
		{
			ret = DATA_READ_BY_DEFAULT(finance_data_type, &search_rule_set, result_set_map);
		}
		if (CHECK_FAILURE(ret))
			return ret;
		search_rule_changed = false;
	}
	// assert(result_set_map != NULL && "result_set_map should NOT be NULL");
	if (result_set_map != NULL)
		print_to_console(result_set_map->to_string());
	else
		print_to_console("Empty");
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_reset_search_param_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	reset_search_param();
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_stock_support_resistance_filepath_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (stock_support_and_resistance_root_folderpath != NULL)
	{
		delete[] stock_support_and_resistance_root_folderpath;
		stock_support_and_resistance_root_folderpath = NULL;
	}
	int stock_support_and_resistance_root_folderpath_len = strlen(argv[1]);
	stock_support_and_resistance_root_folderpath = new char[stock_support_and_resistance_root_folderpath_len + 1];
	if (stock_support_and_resistance_root_folderpath == NULL)
		throw bad_alloc();
	memcpy(stock_support_and_resistance_root_folderpath, argv[1], sizeof(char) * (stock_support_and_resistance_root_folderpath_len + 1));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_stock_support_resistance_verbose_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (strcasecmp("True", argv[1]) == 0)
		show_stock_support_resistance_verbose = true;
	else if (strcasecmp("False", argv[1]) == 0)
		show_stock_support_resistance_verbose = false;
	else
	{
		static char rsp_buf[RSP_BUF_SIZE];
		snprintf(rsp_buf, RSP_BUF_SIZE, "Unknown parameter for enabling stock support resistance verbose: %s", argv[1]);
		WRITE_WARN(rsp_buf);
		print_to_console(rsp_buf);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_stock_support_resistance_date_filter_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (stock_support_resistance_date_filter != NULL)
	{
		delete[] stock_support_resistance_date_filter;
		stock_support_resistance_date_filter = NULL;
	}
	int stock_support_resistance_date_filter_len = strlen(argv[1]);
	stock_support_resistance_date_filter = new char[stock_support_resistance_date_filter_len + 1];
	if (stock_support_resistance_date_filter == NULL)
		throw bad_alloc();
	memcpy(stock_support_resistance_date_filter, argv[1], sizeof(char) * (stock_support_resistance_date_filter_len + 1));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_set_stock_support_resistance_volume_filter_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (stock_support_resistance_volume_filter != NULL)
	{
		delete[] stock_support_resistance_volume_filter;
		stock_support_resistance_volume_filter = NULL;
	}
	int stock_support_resistance_volume_filter_len = strlen(argv[1]);
	stock_support_resistance_volume_filter = new char[stock_support_resistance_volume_filter_len + 1];
	if (stock_support_resistance_volume_filter == NULL)
		throw bad_alloc();
	memcpy(stock_support_resistance_volume_filter, argv[1], sizeof(char) * (stock_support_resistance_volume_filter_len + 1));
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_get_stock_support_resistance_parameters_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	static const int BUF_SIZE = 256;
	static char buf[BUF_SIZE];
	string parameters_string = "";
	parameters_string += string("*** Stock Support and Resistance Parameters ***\n");
	const char* stock_support_and_resistance_root_folderpath_param = ((stock_support_and_resistance_root_folderpath == NULL) ? DEFAULT_STOCK_SUPPORT_RESISTANCE_ROOT_FOLDERPATH : stock_support_and_resistance_root_folderpath);
	snprintf(buf, BUF_SIZE, " Company data root folder path: %s\n", stock_support_and_resistance_root_folderpath_param);
	parameters_string += string(buf);
	snprintf(buf, BUF_SIZE, " Show verbose: %s\n", (show_stock_support_resistance_verbose ? "True" : "False"));
	parameters_string += string(buf);
	const char* stock_support_resistance_date_filter_param = ((stock_support_resistance_date_filter == NULL) ? "Not set" : stock_support_resistance_date_filter);
	snprintf(buf, BUF_SIZE, " Date filter: %s\n", stock_support_resistance_date_filter_param);
	parameters_string += string(buf);
	const char* stock_support_resistance_volume_filter_param = ((stock_support_resistance_volume_filter == NULL) ? "Not set" : stock_support_resistance_volume_filter);
	snprintf(buf, BUF_SIZE, " Volume filter: %s\n", stock_support_resistance_volume_filter_param);
	parameters_string += string(buf);
	print_to_console(parameters_string.c_str());
	
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_cleanup_stock_support_resistance_parameters_command(int argc, char **argv)
{
	if (argc != 1)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	if (stock_support_and_resistance_root_folderpath != NULL)
	{
		delete[] stock_support_and_resistance_root_folderpath;
		stock_support_and_resistance_root_folderpath = NULL;
	}
	if (stock_support_resistance_volume_filter)
	{
		delete[] stock_support_resistance_volume_filter;
		stock_support_resistance_volume_filter = NULL;
	}
	if (stock_support_resistance_date_filter)
	{
		delete[] stock_support_resistance_date_filter;
		stock_support_resistance_date_filter = NULL;
	}
	show_stock_support_resistance_verbose = false;
	
	return RET_SUCCESS;
}

unsigned short InteractiveSession::handle_find_stock_support_resistance_command(int argc, char **argv)
{
	if (argc != 2)
	{
		WRITE_FORMAT_WARN("WANRING!! Incorrect command: %s", argv[0]);
		print_to_console(incorrect_command_phrases);
		return RET_WARN_INTERACTIVE_COMMAND;
	}
	char *stock_support_resistance_entry = argv[1];
	unsigned short ret = RET_SUCCESS;
	int start_index = 0;
	int str_len;
	const char* pos = NULL;
	while ((pos = strchr(&stock_support_resistance_entry[start_index], ',')) != NULL)
	{
		str_len = pos - stock_support_resistance_entry - start_index;
		ret = print_single_stock_support_resistance_string(&stock_support_resistance_entry[start_index], str_len);
		if (CHECK_FAILURE(ret))
			goto OUT;
		start_index = pos - stock_support_resistance_entry + 1;
	}
	str_len = strlen(stock_support_resistance_entry) - start_index;
	ret = print_single_stock_support_resistance_string(&stock_support_resistance_entry[start_index], str_len);
OUT:
	return ret;
}

unsigned short InteractiveSession::handle_help_command(int argc, char **argv)
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
	usage_string += string("* get_method\nDescription: Get list of method\n");
	usage_string += string("* set_method\nDescription: Set list of method\n");
	int method_index_start, method_index_end;
	get_method_index_range(method_index_start, method_index_end, finance_analysis_mode);
	for (int i = method_index_start ; i < method_index_end ; i++)
	{
		snprintf(buf, BUF_SIZE, "  %s: %d\n", FINANCE_METHOD_DESCRIPTION_LIST[i], i);
		usage_string += string(buf);
	}
	usage_string += string("  Format 1: All source types/fields (ex. all)\n");	
	usage_string += string("  Format 2: Source type index/index range (ex. 1,2-4,6)\n");
	usage_string += string("  Format 3: Source type index/index range with field index/index range (ex. 1(1-2;4),2-4(2-4;5),5,6(1;3;5-7))\n");
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
	}
// Data Type
	usage_string += string("* get_data_type\nDescription: Get data type of the reader\n");
	usage_string += string("* set_data_type\nDescription: Set data type of the reader\n");
	usage_string += string(" Data type of the reader:\n");
	for(int i = 0 ; i < FinanceDataSize ; i++)
	{
		snprintf(buf, BUF_SIZE, "  %s: %d\n", FINANCE_DATA_DESCRIPTION[i], i);
		usage_string += string(buf);
	}
// Search param in each data type
	snprintf(buf, BUF_SIZE, "* set_continue_when_non_exist\nDescription: Set to continue searching other data even if some are missing\nDefault: %s\n", (DEFAULT_CONTINUE_WHEN_NOT_EXIST ? "True" : "False"));
	usage_string += string(buf);
	snprintf(buf, BUF_SIZE, "* get_continue_when_non_exist\nDescription: Get the flag of continuing searching other data even if some are missing\nDefault: %s\n", (DEFAULT_CONTINUE_WHEN_NOT_EXIST ? "True" : "False"));
	usage_string += string(buf);	
	snprintf(buf, BUF_SIZE, "* set_csv_root_folderpath\nDescription: Set CSV root folerpath\nDefault: %s\n", DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH);
	usage_string += string(buf);
	snprintf(buf, BUF_SIZE, "* get_csv_root_folderpath\nDescription: Get CSV root folerpath\nDefault: %s\n", DEFAULT_CSV_ROOT_FINANCE_FOLDERPATH);
	usage_string += string(buf);
	snprintf(buf, BUF_SIZE, "* set_shm_root_folderpath\nDescription: Set SHM root folerpath\nDefault: %s\n", DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH);
	usage_string += string(buf);
	snprintf(buf, BUF_SIZE, "* get_shm_root_folderpath\nDescription: Get SHM root folerpath\nDefault: %s\n", DEFAULT_SHM_ROOT_FINANCE_FOLDERPATH);
	usage_string += string(buf);
// Reset Search Param
	if (finance_analysis_mode == FinanceAnalysis_Market)
		usage_string += string("* reset_search_param\n Description: Reset the search param: source/time_range/data_sql_reader/data_csv_reader\n");
	else if (finance_analysis_mode == FinanceAnalysis_Stock)
		usage_string += string("* reset_search_param\n Description: Reset the search param: source/time_range/company/data_sql_reader/data_csv_reader\n");
// Search
	if (finance_analysis_mode == FinanceAnalysis_Market)
		usage_string += string("* search\n Description: Search the database under the rule of source type and time range\n");
	else if (finance_analysis_mode == FinanceAnalysis_Stock)
		usage_string += string("* search\n Description: Search the database under the rule of source type, time range and company number\n");
// Calcuate the support and resistance of the stock price
	if (finance_analysis_mode == FinanceAnalysis_Stock)
	{
		usage_string += string("* set_stock_support_resistance_verbose\nDescription: Enable/Disable to show the stock support and resistance of a specific company in detail\nDefault: False\n");
		usage_string += string("* set_stock_support_resistance_date_filter\nDescription: Set the filter to ignore the data which is eariler than a specific date\n");
		usage_string += string("  Format: Date(ex. 170801)\n");
		usage_string += string("* set_stock_support_resistance_volume_filter\nDescription: Set the filter to ignore the data whose volume is smaller than a specific value\n");
		usage_string += string("  Format: Value(ex. 5000)\n");
		usage_string += string("* get_stock_support_resistance_parameters\nDescription: Get the parametres for finding the stock support and resistance of a specific company\n");
		usage_string += string("* cleanup_stock_support_resistance_parameters\nDescription: Clean up the parametres for finding the stock support and resistance of a specific company\n");
		usage_string += string("* find_stock_support_resistance\nDescription: Find the stock support and resistance of a specific company\n");
		usage_string += string("  Format 1: Company code number:Stock close price Pair(ex. 1560:77.8)\n");
		usage_string += string("  Format 2: Company code number:Stock close price Pair List(ex. 1560:77.8,1589:81.9,1215:67)\n");
	}
	usage_string += string("===================================================\n");

	ret = print_to_console(usage_string);
	return ret;
}

unsigned short InteractiveSession::handle_exit_command(int argc, char **argv)
{
	print_to_console(string("Bye bye !!!"));
	event_notify->notify(InteractiveSessionEvent_Exit, (void*)&session_id);
	return RET_SUCCESS;
}