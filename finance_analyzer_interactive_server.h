#ifndef FINANCE_ANALYZER_INTERACTIVE_SERVER_H
#define FINANCE_ANALYZER_INTERACTIVE_SERVER_H

// #include "finance_analyzer_interactive_session.h"
#include <deque>
#include "finance_analyzer_common.h"


#define DECLARE_INTERACTIVE_SERVER() PFINANCE_ANALYZER_INTERACTIVE_SERVER interactive_server;
#define IMPLEMENT_INTERACTIVE_SERVER() interactive_server = FinanceAnalyzerInteractiveServer::get_instance();
#define RELEASE_INTERACTIVE_SERVER() SAFE_RELEASE(interactive_server)

#define DECLARE_AND_IMPLEMENT_STATIC_INTERACTIVE_SERVER()\
static PFINANCE_ANALYZER_INTERACTIVE_SERVER interactive_server = FinanceAnalyzerInteractiveServer::get_instance();


class FinanceAnalyzerInteractiveSession;

typedef std::deque<FinanceAnalyzerInteractiveSession*> INTERACTIVE_SESSION_DEQUE;
typedef INTERACTIVE_SESSION_DEQUE* PINTERACTIVE_SESSION_DEQUE;
typedef std::deque<FinanceAnalyzerInteractiveSession*>::iterator INTERACTIVE_SESSION_ITER;

class FinanceAnalyzerInteractiveServer : public IEventNotify
{
	DECLARE_MSG_DUMPER()

private:
	static FinanceAnalyzerInteractiveServer* instance;
	volatile int ref_cnt;
	INTERACTIVE_SESSION_DEQUE interactive_session_deque;
	int server_fd;

	FinanceAnalyzerInteractiveServer();
	FinanceAnalyzerInteractiveServer(const FinanceAnalyzerInteractiveServer&);
	const FinanceAnalyzerInteractiveServer& operator=(const FinanceAnalyzerInteractiveServer&);
	~FinanceAnalyzerInteractiveServer();

	unsigned short init_server();
	unsigned short wait_for_connection();
	unsigned short initialize();

public:
	static FinanceAnalyzerInteractiveServer* get_instance();
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
typedef FinanceAnalyzerInteractiveServer* PFINANCE_ANALYZER_INTERACTIVE_SERVER;

#endif
