CC := g++
MAKE := make

CXXFLAGS := -g -Wall -std=c++0x

CMN_FOLDER := common
CMN_SOURCES := $(CMN_FOLDER)/msg_dumper_wrapper.cpp $(CMN_FOLDER)/finance_analyzer_common_definition_base.cpp $(CMN_FOLDER)/finance_analyzer_common_definition_market_sql.cpp $(CMN_FOLDER)/finance_analyzer_common_definition_stock_sql.cpp $(CMN_FOLDER)/finance_analyzer_common_function.cpp $(CMN_FOLDER)/finance_analyzer_common_class_base.cpp $(CMN_FOLDER)/finance_analyzer_common_class_company_profile.cpp $(CMN_FOLDER)/finance_analyzer_common_class_set.cpp $(CMN_FOLDER)/finance_analyzer_common_class_smart_pointer.cpp $(CMN_FOLDER)/finance_analyzer_common_class_output.cpp
SOURCES := $(CMN_SOURCES) finance_analyzer.cpp finance_analyzer_mgr.cpp finance_analyzer_mgr_factory.cpp finance_analyzer_sql_reader.cpp finance_analyzer_workday_canlendar.cpp finance_analyzer_database_time_range.cpp finance_analyzer_algorithm.cpp finance_analyzer_filter.cpp finance_analyzer_data_collector.cpp finance_analyzer_data_statistics.cpp finance_analyzer_test.cpp finance_analyzer_output.cpp finance_analyzer_interactive_server.cpp finance_analyzer_interactive_session.cpp
OBJS := $(SOURCES:.cpp=.o)
LIB_MSG_DUMPER := libmsg_dumper.so
LIB_MSG_DUMPER_HEADER := msg_dumper.h
LIB_MSG_DUMPER_PATH := ../msg_dumper
LIB_MSG_DUMPER_WRAPPER_FOLDER := wrapper
LIB_MSG_DUMPER_WRAPPER_HEADER := msg_dumper_wrapper.h
LIB_MSG_DUMPER_WRAPPER_SOURCE := msg_dumper_wrapper.cpp

OUTPUT := finance_analyzer
	
build: $(LIB_MSG_DUMPER_HEADER) $(LIB_MSG_DUMPER_WRAPPER_HEADER) $(LIB_MSG_DUMPER_WRAPPER_SOURCE) $(LIB_MSG_DUMPER) $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@ -Wl,--start-group $(LIB_MSG_DUMPER) -Wl,--end-group -lrt -ldl -lpthread -lmysqlclient

$(LIB_MSG_DUMPER_WRAPPER_HEADER):
	cp $(LIB_MSG_DUMPER_PATH)/$(LIB_MSG_DUMPER_WRAPPER_FOLDER)/$(LIB_MSG_DUMPER_WRAPPER_HEADER) $(CMN_FOLDER)

$(LIB_MSG_DUMPER_WRAPPER_SOURCE):
	cp $(LIB_MSG_DUMPER_PATH)/$(LIB_MSG_DUMPER_WRAPPER_FOLDER)/$(LIB_MSG_DUMPER_WRAPPER_SOURCE) $(CMN_FOLDER)

$(LIB_MSG_DUMPER_HEADER):
	cp $(LIB_MSG_DUMPER_PATH)/$(LIB_MSG_DUMPER_HEADER) $(CMN_FOLDER)

$(LIB_MSG_DUMPER):
	$(MAKE) -C $(LIB_MSG_DUMPER_PATH)
	cp $(LIB_MSG_DUMPER_PATH)/$(LIB_MSG_DUMPER) .
	$(MAKE) -C $(LIB_MSG_DUMPER_PATH) clean

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER) 2> /dev/null
	rm -f $(OUTPUT) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER_HEADER) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER_WRAPPER_HEADER) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER_WRAPPER_SOURCE) 2> /dev/null
