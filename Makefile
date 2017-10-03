CC := g++
MAKE := make

CXXFLAGS := -g -Wall -std=c++0x

CMN_FOLDER := common
CMN_SOURCES := $(CMN_FOLDER)/msg_dumper_wrapper.cpp $(CMN_FOLDER)/common_definition_base.cpp $(CMN_FOLDER)/common_definition_data_market_field.cpp $(CMN_FOLDER)/common_definition_data_stock_field.cpp $(CMN_FOLDER)/common_function.cpp $(CMN_FOLDER)/common_class_base.cpp $(CMN_FOLDER)/common_class_company_profile.cpp $(CMN_FOLDER)/common_class_time.cpp $(CMN_FOLDER)/common_class_set.cpp $(CMN_FOLDER)/common_class_smart_pointer.cpp $(CMN_FOLDER)/common_class_output.cpp
SOURCES := $(CMN_SOURCES) finance_analyzer.cpp finance_analyzer_mgr.cpp mgr_factory.cpp data_sql_reader.cpp data_csv_reader.cpp data_reader.cpp workday_canlendar.cpp database_time_range.cpp data_algorithm.cpp data_filter.cpp data_collector.cpp data_calculator.cpp testcase_mgr.cpp data_output.cpp interactive_server.cpp interactive_session.cpp stock_support_resistance.cpp
OBJS := $(SOURCES:.cpp=.o)
LIB_MSG_DUMPER := libmsg_dumper.so
LIB_MSG_DUMPER_HEADER := msg_dumper.h
LIB_MSG_DUMPER_PATH := ../msg_dumper
LIB_MSG_DUMPER_WRAPPER_FOLDER := wrapper
LIB_MSG_DUMPER_WRAPPER_HEADER := msg_dumper_wrapper.h
LIB_MSG_DUMPER_WRAPPER_SOURCE := msg_dumper_wrapper.cpp

OUTPUT := finance_analyzer

build: $(OUTPUT)

deep_build: $(LIB_MSG_DUMPER_HEADER) $(LIB_MSG_DUMPER_WRAPPER_HEADER) $(LIB_MSG_DUMPER_WRAPPER_SOURCE) $(LIB_MSG_DUMPER) $(OUTPUT)

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
	rm -f $(OUTPUT) 2> /dev/null

deep_clean: $(clean)
	rm -f $(OBJS) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER) 2> /dev/null
	rm -f $(OUTPUT) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER_HEADER) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER_WRAPPER_HEADER) 2> /dev/null
	rm -f $(LIB_MSG_DUMPER_WRAPPER_SOURCE) 2> /dev/null