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
# current files

TEST_CIRCUIT_TRACE_OBJ_TEST 	= $(TESTS_OBJ_DIR)/CircuitTraceTest.o

TEST_CIRCUIT_TRACE_TARGET 		= $(TESTS_BIN_DIR)/testtrace

TEST_CIRCUIT_NET_OBJ_TEST 		= $(TESTS_OBJ_DIR)/CircuitNetTest.o

TEST_CIRCUIT_NET_TARGET 		= $(TESTS_BIN_DIR)/testnet

TEST_GRAPH_OBJ_FILES			= $(TESTS_OBJ_DIR)/GraphTest.o	\
									$(TESTS_OBJ_DIR)/GridGraphTest.o 
								
TEST_GRAPH_TARGET				= $(TESTS_BIN_DIR)/testgraph

TEST_MUTABLE_GRAPH_OBJ_
# prev files

TEST_UNION_FIND_OBJ_FILES	= $(TESTS_OBJ_DIR)/UnionFindTest.o 			\
						  	  $(TESTS_OBJ_DIR)/MyImplUnionFind.o 	

TEST_UNION_FIND_TARGET		= $(TESTS_BIN_DIR)/testunionfind

TEST_MAP_OBJ_FILES			= $(TESTS_OBJ_DIR)/MapTest.o

TEST_MAP_TARGET				= $(TESTS_BIN_DIR)/testmap

TEST_FRIEND_GROUP_OBJ_FILES	= $(TESTS_OBJ_DIR)/FriendGroupsTest.o	\
								$(TESTS_OBJ_DIR)/FriendGroups.o		\
								$(TESTS_OBJ_DIR)/MyImplUnionFind.o

TEST_FRIEND_GROUP_TARGET	= $(TESTS_BIN_DIR)/testfriendgroups

MAKE_FRIENDS_OBJ_FILES		= 	$(OBJ_DIR)/makefriends.o \
                       			$(OBJ_DIR)/FriendGroups.o \
                        		$(OBJ_DIR)/MyImplUnionFind.o

MAKE_FRIENDS_TARGET    = $(BIN_DIR)/makefriends
DATA_DIR              = ./data


ANALYSIS_OBJ_FILES		= $(OBJ_DIR)/analysis.o   				\
							$(OBJ_DIR)/FriendGroups.o  			\
							  

ANALYSIS_TARGET			= $(BIN_DIR)/analysis

VALGRIND            = valgrind
VALGRIND_FLAGS      = --leak-check=full --errors-for-leak-kinds=definite,indirect --error-exitcode=1


all: make_directories 			\
	$(TEST_UNION_FIND_TARGET) 	\
	run_testunionfind			\
	run_testmap					\
	run_testfriendgroups		\
	generate_coverage 			
					

run_valgrind_makefriends: $(MAKE_FRIENDS_TARGET)
	$(VALGRIND) $(VALGRIND_FLAGS) $(MAKE_FRIENDS_TARGET) -f $(DATA_DIR)/friendsmed.csv 

run_makefriends: run_valgrind_makefriends
	for f in $(DATA_DIR)/*.csv; do 		\
	    echo "== $$f"; 					\
	    $(MAKE_FRIENDS_TARGET) -f $$f;	\
	done

run_testunionfind: $(TEST_UNION_FIND_TARGET)
	$(TEST_UNION_FIND_TARGET)

run_testmap: $(TEST_MAP_TARGET)
	$(TEST_MAP_TARGET)

run_testfriendgroups: $(TEST_FRIEND_GROUP_TARGET)
	$(TEST_FRIEND_GROUP_TARGET)

generate_coverage:
	$(LCOV) --capture --directory `pwd` --output-file $(COVERAGE_INFO_FILE) --ignore-errors inconsistent,source 2> lcov.err && \
	grep -v "mismatched end line" lcov.err >&2 && rm -f lcov.err
	$(LCOV) --remove $(COVERAGE_INFO_FILE) '/usr/*' '*/tests/src/*'  --output-file $(COVERAGE_INFO_FILE)
	$(GENHTML) $(COVERAGE_INFO_FILE) --output-directory $(TESTS_HTMLCOV_DIR)

$(ANALYSIS_TARGET): $(ANALYSIS_OBJ_FILES)
	$(CXX) $(CPPFLAGS) $(ANALYSIS_OBJ_FILES) $(LDFLAGS) -o $@

## New
$(TEST_GRAPH_TARGET): $(TEST_GRAPH_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@
	
$(TEST_CIRCUIT_TRACE_TARGET): $(TEST_CIRCUIT_TRACE_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(TEST_CIRCUIT_NET_TARGET): $(TEST_CIRCUIT_NET_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

## old
$(TEST_UNION_FIND_TARGET): $(TEST_UNION_FIND_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(TEST_MAP_TARGET): $(TEST_MAP_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(TEST_FRIEND_GROUP_TARGET): $(TEST_FRIEND_GROUP_OBJ_FILES)
	$(CXX) $(TEST_CPPFLAGS) $^  $(TEST_LDFLAGS) -o $@

$(MAKE_FRIENDS_TARGET): $(MAKE_FRIENDS_OBJ_FILES)
	$(CXX) $(CPPFLAGS) $^ $(LDFLAGS) -o $@

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