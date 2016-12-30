#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <stdio.h>
#include <assert.h>
#include <map>
// #include <string>
#include "finance_analyzer_interactive_session.h"


using namespace std;

DECLARE_MSG_DUMPER_PARAM()

// Command type definition
enum InteractiveSessionCommandType
{
	InteractiveSessionCommand_Help,
	InteractiveSessionCommand_Exit,
	InteractiveSessionCommandSize
};

static const char *interactive_session_command[InteractiveSessionCommandSize] = 
{
	"help",
	"exit"
};

typedef map<string, InteractiveSessionCommandType> COMMAND_MAP;
typedef COMMAND_MAP::iterator COMMAND_MAP_ITER;
static COMMAND_MAP command_map;

const int FinanceAnalyzerInteractiveSession::BUF_SIZE = 1024;
const int FinanceAnalyzerInteractiveSession::MAX_ARGC = 20;

void FinanceAnalyzerInteractiveSession::init_command_map()
{
	static bool init_map = false;
	if (!init_map)
	{
		DECLARE_AND_IMPLEMENT_STATIC_MSG_DUMPER();
		DECLARE_MSG_DUMPER_PARAM();
		for (int i = 0 ; i < InteractiveSessionCommandSize ; i++)
		{
			command_map.insert(make_pair(string(interactive_session_command[i]), (InteractiveSessionCommandType)i));
		}
		for(COMMAND_MAP_ITER iter = command_map.begin() ; iter != command_map.end() ; iter++)
		{
			string command_description = (string)iter->first;
			int command_type = (int)iter->second;
			WRITE_FORMAT_DEBUG("Command %d: %s", command_type, command_description.c_str());
		}
		RELEASE_MSG_DUMPER();
		init_map = true;
	}
}

FinanceAnalyzerInteractiveSession::FinanceAnalyzerInteractiveSession(int client_fd, sockaddr_in& client_sockaddress, int interactive_session_id, PIEVENT_NOTIFY parent) :
	pid(0),
	exit(0),
	sock_fd(client_fd),
	session_id(interactive_session_id),
	event_notify(parent),
	user_exit(false)
{
	IMPLEMENT_MSG_DUMPER()
	init_command_map();
	memcpy(&client_sock, &client_sockaddress, sizeof(sockaddr_in));
	memset(session_tag, 0x0, sizeof(char) * 64);
	snprintf(session_tag, 64, "%s:%d", inet_ntoa(client_sock.sin_addr), htons(client_sock.sin_port));
}
	
FinanceAnalyzerInteractiveSession::~FinanceAnalyzerInteractiveSession()
{
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
	if (status == NULL)
		WRITE_FORMAT_DEBUG("The worker thread of interactive session[%s]'s is dead......", session_tag);
	else
		WRITE_FORMAT_ERROR("Error occur in the worker thread of interactive session[%s], due to: %s", session_tag, (char*)status);
	pid = 0;
}

void* FinanceAnalyzerInteractiveSession::thread_handler(void* void_ptr)
{
	if (void_ptr == NULL)
		throw invalid_argument(string("void_ptr should NOT be NULL"));
	PFINANCE_ANALYZER_INTERACTIVE_SESSION this_ptr = (PFINANCE_ANALYZER_INTERACTIVE_SESSION)void_ptr;
	unsigned short ret = this_ptr->thread_handler_internal();
	pthread_exit((CHECK_SUCCESS(ret) ? NULL : (void*)get_ret_description(ret)));
}

unsigned short FinanceAnalyzerInteractiveSession::thread_handler_internal()
{
	// static const int INTERACTIVE_PROMPT_LEN = strlen(INTERACTIVE_PROMPT);
	WRITE_FORMAT_INFO("The worker thread of interactive session[%s] is running", session_tag);
	unsigned short ret = RET_SUCCESS;

	struct pollfd pollfds[1];
	char buf[BUF_SIZE];
	// int n;
	FILE* sock_fp = NULL;
	if ((sock_fp = fdopen(sock_fd, "a+")) == 0) 
	{
		WRITE_FORMAT_ERROR("Fail to transform FD to FP, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}

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
    	if (fgets(buf, BUF_SIZE, sock_fp) == NULL)
    		break;
     	WRITE_FORMAT_DEBUG("Command Line: %s", buf);
// Parse the command
		char *command_line_outer =  buf;
		char *rest_command_line_outer =  NULL;
		char *argv_outer[MAX_ARGC];
		int cur_argc_outer = 0;
		while ((argv_outer[cur_argc_outer] = strtok_r(command_line_outer, ";\t\n\r", &rest_command_line_outer)) != NULL)
		{
			WRITE_FORMAT_DEBUG("Command Argument[Outer]: %s, rest: %s", argv_outer[cur_argc_outer], rest_command_line_outer);
			char *command_line_inner =  argv_outer[cur_argc_outer];
			char *rest_command_line_inner =  NULL;
			char *argv_inner[MAX_ARGC];
			int cur_argc_inner = 0;
			bool can_execute = true;
			while ((argv_inner[cur_argc_inner] = strtok_r(command_line_inner, " ", &rest_command_line_inner)) != NULL)
			{
				WRITE_FORMAT_DEBUG("Command Argument[Inner]: %s, rest: %s", argv_inner[cur_argc_inner], rest_command_line_inner);
				if (command_line_inner != NULL)
				{
// Check if the command exist
					COMMAND_MAP::iterator iter = command_map.find(string(argv_inner[cur_argc_inner]));
					if (iter == command_map.end())
					{
						WRITE_FORMAT_ERROR("Error!! Unknown command: %s", argv_inner[0]);
						static char unknown_command_error[64];
						snprintf(unknown_command_error, 64, "Unknown command: %s", argv_inner[0]);
						write(sock_fd, unknown_command_error, strlen(unknown_command_error));
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
				if (ret == RET_FAILURE_COMMAND_IMCOMPLETE)
				{
// Show warning if the command is incomplete
					WRITE_FORMAT_ERROR("Warning!! Incomplete command: %s,", argv_inner[0]);
					static char incomplete_command_waring[64];
					snprintf(incomplete_command_waring, 64, "Incomplete command: %s", argv_inner[0]);
					write(sock_fd, incomplete_command_waring, strlen(incomplete_command_waring));
				}	
			}
			if (command_line_outer != NULL)
				command_line_outer = NULL;
			cur_argc_outer++;
		}
		if (CHECK_FAILURE(ret))
			return ret;
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

unsigned short FinanceAnalyzerInteractiveSession::handle_command(int argc, char **argv)
{
	typedef unsigned short (FinanceAnalyzerInteractiveSession::*handle_command_func_ptr)(int argc, char**argv);
	static handle_command_func_ptr handle_command_func_array[] =
	{
		&FinanceAnalyzerInteractiveSession::handle_help_command,
		&FinanceAnalyzerInteractiveSession::handle_exit_command
	};

	// assert (iter != command_map.end() && "Unknown command");
	COMMAND_MAP::iterator iter = command_map.find(string(argv[0]));
	int command_type = (int)iter->second;
	// WRITE_FORMAT_DEBUG("Inner command type: %s, %d", ((string)iter->first).c_str(), (int)iter_fuck->second);
	return (this->*(handle_command_func_array[command_type]))(argc, argv);
}

unsigned short FinanceAnalyzerInteractiveSession::handle_help_command(int argc, char **argv)
{
	unsigned short ret = RET_SUCCESS;
	// WRITE_DEBUG("Command: Help");
	// n = write(sock_fd, INTERACTIVE_PROMPT, INTERACTIVE_PROMPT_LEN);
	ret = print_to_console(string("\nTBD\n"));
	return ret;
}
unsigned short FinanceAnalyzerInteractiveSession::handle_exit_command(int argc, char **argv)
{
	print_to_console(string("Bye bye !!!"));
	event_notify->notify(InteractiveSessionEvent_Exit, (void*)&session_id);
	return RET_SUCCESS;
}