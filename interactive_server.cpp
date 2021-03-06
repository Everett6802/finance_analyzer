// #include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"
#include <stdexcept>
#include <string>
// #include <algorithm>
#include "interactive_server.h"
#include "interactive_session.h"


using namespace std;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

InteractiveServer::const_iterator::const_iterator(INTERACTIVE_SESSION_ITER iterator) : iter(iterator){}

InteractiveServer::const_iterator InteractiveServer::const_iterator::operator++()
{
	++iter;
	return *this;
}

bool InteractiveServer::const_iterator::operator==(const const_iterator& another)
{
	if (this == &another)
		return true;
	return iter == another.iter;
}
		
bool InteractiveServer::const_iterator::operator!=(const const_iterator& another)
{
	return !(*this == another);
}

const PINTERACTIVE_SESSION_DEQUE InteractiveServer::const_iterator::operator->()
{
	return (PINTERACTIVE_SESSION_DEQUE)(*iter);
}

const INTERACTIVE_SESSION_DEQUE& InteractiveServer::const_iterator::operator*()
{
	return *((PINTERACTIVE_SESSION_DEQUE)(*iter));
}

///////////////////////////////////////////////////////////////////////////////////////

InteractiveServer* InteractiveServer::instance = NULL;

InteractiveServer* InteractiveServer::get_instance()
{
	if (instance == NULL)
	{
		pthread_mutex_lock(&mtx);
		if (instance == NULL)
		{
			instance = new InteractiveServer();
			if (instance == NULL)
				throw bad_alloc();
			unsigned short ret = instance->initialize();
			if (CHECK_FAILURE(ret))
			{
				char errmsg[256];
				snprintf(errmsg, 256, "Fail to initialize the InteractiveServer object , due to: %s", get_ret_description(ret));
				throw runtime_error(string(errmsg));
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	instance->add_ref();
	return instance;
}

InteractiveServer::InteractiveServer()
{
	IMPLEMENT_MSG_DUMPER()
	ref_cnt = 0;
	server_fd = 0;
}

InteractiveServer::~InteractiveServer()
{
// Notify each session to exit and Delete all the sessions
	INTERACTIVE_SESSION_ITER iter = interactive_session_deque.begin();
	while(iter != interactive_session_deque.end())
	{
		PINTERACTIVE_SESSION interactive_session = (PINTERACTIVE_SESSION)(*iter);
		iter++;
		if (interactive_session != NULL)
		{
			interactive_session->notify_exit();
			delete interactive_session;
		}
	}
	interactive_session_deque.clear();
	if (server_fd != -1)
	{
		close(server_fd);
		server_fd = -1;
	}
	RELEASE_MSG_DUMPER()
}

unsigned short InteractiveServer::init_server()
{
	// unsigned short ret = RET_SUCCESS;
	WRITE_DEBUG("Initailize Finance Analysis Interactive Server......");
	sockaddr_in server_sock;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		WRITE_FORMAT_ERROR("socket() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	int val = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));
	bzero((char*)&server_sock, sizeof(server_sock));
	server_sock.sin_family = AF_INET;
	server_sock.sin_addr.s_addr = INADDR_ANY;
	server_sock.sin_port = htons(INTERACTIVE_SERVER_PORT);
	if(bind(server_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0)
	{
		WRITE_FORMAT_ERROR("bind() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	if (listen(server_fd, INTERACTIVE_SERVER_BACKLOG) < 0)
	{
		WRITE_FORMAT_ERROR("listen() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}

	return RET_SUCCESS;
}

unsigned short InteractiveServer::wait_for_connection()
{
	WRITE_DEBUG("Finance Analysis Interactive Server is Ready, Wait for connection......");
	unsigned short ret = RET_SUCCESS;
	int client_fd;
	sockaddr_in client_sock;
	socklen_t client_sock_len;
	int session_cnt = 0;
	while (true)
	{
		client_sock_len = sizeof(client_sock);
		client_fd = accept(server_fd, (struct sockaddr *)&client_sock, &client_sock_len);
		WRITE_FORMAT_DEBUG("Connection request from %s:%d", inet_ntoa(client_sock.sin_addr), htons(client_sock.sin_port));
		if (client_fd == -1)
		{
			WRITE_FORMAT_ERROR("listen() fails, due to: %s", strerror(errno));
			return RET_FAILURE_SYSTEM_API;
		}
		PINTERACTIVE_SESSION interactive_session = new InteractiveSession(client_fd, client_sock, session_cnt++, this);
		if (interactive_session == NULL)
		{
			WRITE_ERROR("Fail to allocate memory: interactive_session");
			return RET_FAILURE_INSUFFICIENT_MEMORY;
		}
		ret = interactive_session->initialize();
		if (CHECK_FAILURE(ret))
			return ret;
		interactive_session_deque.push_back(interactive_session);
	}
	return RET_SUCCESS;
}

unsigned short InteractiveServer::initialize()
{
	unsigned short ret = RET_SUCCESS;
	ret = init_server();
	if (CHECK_FAILURE(ret))
		return ret;
	ret = wait_for_connection();
	if (CHECK_FAILURE(ret))
		return ret;
	return ret;
}

int InteractiveServer::add_ref()
{
	__sync_fetch_and_add(&ref_cnt, 1);
	return ref_cnt;
}

int InteractiveServer::release()
{
	__sync_fetch_and_sub(&ref_cnt, 1);
	if (ref_cnt == 0)
	{
		instance = NULL;
		delete this;
		return 0;
	}
	return ref_cnt;
}

InteractiveServer::const_iterator InteractiveServer::begin() 
{
	return const_iterator(interactive_session_deque.begin());
}

InteractiveServer::const_iterator InteractiveServer::end() 
{
	return const_iterator(interactive_session_deque.end());
}

unsigned short InteractiveServer::notify(int event_type, void* event_data)
{
	switch(event_type)
	{
		case InteractiveSessionEvent_Exit:
		{
			int session_id = *(int*)event_data;
			WRITE_FORMAT_DEBUG("Session[%d] notify the parent to exit", session_id);
			PINTERACTIVE_SESSION interactive_session = interactive_session_deque[session_id];
			interactive_session_deque[session_id] = NULL;
			interactive_session->notify_exit();
			delete interactive_session;
		}
		break;
		default:
		{
			throw std::invalid_argument(string("Unknown event type: %d", event_type));
		}
		break;
	}
	return RET_SUCCESS;
}