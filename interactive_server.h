#ifndef INTERACTIVE_SERVER_H
#define INTERACTIVE_SERVER_H

// #include "interactive_session.h"
#include <deque>
#include "common.h"


#define DECLARE_INTERACTIVE_SERVER() PINTERACTIVE_SERVER interactive_server;
#define IMPLEMENT_INTERACTIVE_SERVER() interactive_server = InteractiveServer::get_instance();
#define RELEASE_INTERACTIVE_SERVER() SAFE_RELEASE(interactive_server)

#define DECLARE_AND_IMPLEMENT_STATIC_INTERACTIVE_SERVER()\
static PINTERACTIVE_SERVER interactive_server = InteractiveServer::get_instance();


class InteractiveSession;

typedef std::deque<InteractiveSession*> INTERACTIVE_SESSION_DEQUE;
typedef INTERACTIVE_SESSION_DEQUE* PINTERACTIVE_SESSION_DEQUE;
typedef std::deque<InteractiveSession*>::iterator INTERACTIVE_SESSION_ITER;

class InteractiveServer : public IEventNotify
{
	DECLARE_MSG_DUMPER()

private:
	static InteractiveServer* instance;
	volatile int ref_cnt;
	INTERACTIVE_SESSION_DEQUE interactive_session_deque;
	int server_fd;

	InteractiveServer();
	InteractiveServer(const InteractiveServer&);
	const InteractiveServer& operator=(const InteractiveServer&);
	~InteractiveServer();

	unsigned short init_server();
	unsigned short wait_for_connection();
	unsigned short initialize();

public:
	static InteractiveServer* get_instance();
	int add_ref();
	int release();

	class const_iterator
	{
	private:
		INTERACTIVE_SESSION_ITER iter;

	public:
		const_iterator(INTERACTIVE_SESSION_ITER iterator);
		const_iterator operator++();
		bool operator==(const const_iterator& another);
		bool operator!=(const const_iterator& another);
		const PINTERACTIVE_SESSION_DEQUE operator->();
		const INTERACTIVE_SESSION_DEQUE& operator*();
	};

	const_iterator begin();
	const_iterator end();

	unsigned short notify(int event_type, void* event_data);
};
typedef InteractiveServer* PINTERACTIVE_SERVER;

#endif
