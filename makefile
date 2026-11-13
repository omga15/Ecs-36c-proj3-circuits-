CXX 				= g++
LCOV				= lcov
GENHTML				= genhtml

BIN_DIR				= ./bin
INC_DIR				= ./include
OBJ_DIR				= ./obj
SRC_DIR				= ./src
TESTS_DIR			= ./tests
TESTS_BIN_DIR		= $(TESTS_DIR)/bin
TESTS_OBJ_DIR		= $(TESTS_DIR)/obj
TESTS_HTMLCOV_DIR	= $(TESTS_DIR)/htmlconv
TESTS_SRC_DIR		= $(TESTS_DIR)/src

COVERAGE_INFO_FILE  = $(TESTS_HTMLCOV_DIR)/lcov.info

INCLUDES			= -I $(INC_DIR)
CFLAGS				= $(INCLUDES)
CPPFLAGS			= $(CFLAGS) -O0 -std=c++20
LDFLAGS				= 

TEST_CFLAGS			= -I $(TESTS_SRC_DIR)
TEST_CPPFLAGS		= $(TEST_CFLAGS) $(CPPFLAGS) -fno-inline -ftest-coverage -g --coverage
TEST_LDFLAGS		= -lgtest -lgtest_main -lpthread


TEST_GRAPH_OBJ_FILES		= $(TESTS_OBJ_DIR)/AdjacencyListGraph.o 			\
						  	  $(TESTS_OBJ_DIR)/GraphTest.o 	

TEST_GRAPH_TARGET			= $(TESTS_BIN_DIR)/testgraph

TEST_MUTABLE_GRAPH_OBJ_FILES	= $(TESTS_OBJ_DIR)/AdjacencyListGraph.o 		\
						  	  		$(TESTS_OBJ_DIR)/MutableGraphTest.o 	

TEST_MUTABLE_GRAPH_TARGET		= $(TESTS_BIN_DIR)/testmutablegraph

TEST_CIRCUIT_TRACE_OBJ_FILES 	= $(TESTS_OBJ_DIR)/CircuitTrace.o 				\
									$(TESTS_OBJ_DIR)/CircuitTraceTest.o
						
TEST_CIRCUIT_TRACE_TARGET 		= $(TESTS_BIN_DIR)/testtrace

TEST_CIRCUIT_NET_OBJ_FILES 		= $(TESTS_OBJ_DIR)/CircuitNet.o					\
									$(TESTS_OBJ_DIR)/CircuitNetTest.o

TEST_CIRCUIT_NET_TARGET 		= $(TESTS_BIN_DIR)/testnet


all: make_directories 			\
	run_testgraph				\
	run_testmutablregraph		\
	run_testcircuit_trace		\
	run_testcircuit_net 		\
	generate_coverage 			\
	$(ANALYSIS_TARGET)


run_testgraph: $(TEST_GRAPH_TARGET)
	$(TEST_GRAPH_TARGET)

run_testmutablregraph: $(TEST_MUTABLE_GRAPH_TARGET)
	$(TEST_MUTABLE_GRAPH_TARGET)

run_testcircuit_trace: $(TEST_CIRCUIT_TRACE_TARGET)
	$(TEST_CIRCUIT_TRACE_TARGET)

run_testcircuit_net: $(TEST_CIRCUIT_NET_TARGET)
	$(TEST_CIRCUIT_NET_TARGET)

generate_coverage:
	$(LCOV) --capture --directory `pwd` --output-file $(COVERAGE_INFO_FILE) --ignore-errors inconsistent,source 2> lcov.err && \
	grep -v "mismatched end line" lcov.err >&2 && rm -f lcov.err
	$(LCOV) --remove $(COVERAGE_INFO_FILE) '/usr/*' '*/tests/src/*'  --output-file $(COVERAGE_INFO_FILE)
	$(GENHTML) $(COVERAGE_INFO_FILE) --output-directory $(TESTS_HTMLCOV_DIR)


$(TEST_GRAPH_TARGET): $(TEST_GRAPH_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(TEST_MUTABLE_GRAPH_TARGET): $(TEST_MUTABLE_GRAPH_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(TEST_CIRCUIT_TRACE_TARGET): $(TEST_CIRCUIT_TRACE_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(TEST_CIRCUIT_NET_TARGET): $(TEST_CIRCUIT_NET_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(TESTS_OBJ_DIR)/%.o: $(TESTS_SRC_DIR)/%.cpp
	$(CXX) $(TEST_CPPFLAGS) -c $< -o $@

$(TESTS_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(TEST_CPPFLAGS) -c $< -o $@

make_directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TESTS_BIN_DIR)
	mkdir -p $(TESTS_OBJ_DIR)
	mkdir -p $(TESTS_HTMLCOV_DIR)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(TESTS_BIN_DIR)
	rm -rf $(TESTS_OBJ_DIR)
	rm -rf $(TESTS_HTMLCOV_DIR)