.PHONY: all run clean check debug client server

CC := gcc-9

# compile flags
CFLAGS += -std=c18 -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings
CFLAGS += -Wvla -Wfloat-equal -lm -pthread
CFLAGS += -I./include

DFLAGS := -g3
VFLAGS += --leak-check=full --track-origins=yes
PFLAGS := -pg 

C_OBJ_DIR := cl_obj
S_OBJ_DIR := serv_obj
DOCS_DIR := docs
TEST_DIR := test
HDR_DIR := include
DATA_DIR := data
CLIENT_DIR := client_src
SERVER_DIR := server_src

CLIENT_SRCS := $(wildcard $(CLIENT_DIR)/*.c)
CLIENT_OBJS := $(patsubst $(CLIENT_DIR)/%.c, $(C_OBJ_DIR)/%.o, $(CLIENT_SRCS))

SERVER_SRCS := $(wildcard $(SERVER_DIR)/*.c)
SERVER_OBJS := $(patsubst $(SERVER_DIR)/%.c, $(S_OBJ_DIR)/%.o, $(SERVER_SRCS))

# executables
CLIENT := client
SERVER := server
# CHECK := $(CLIENT)_check
CHECK := buyer_check
# CHECK += $(SERVER)_check

TESTS := $(wildcard $(TEST_DIR)/*.c)
# TEST_OBJS := $(filter-out $(C_OBJ_DIR/client.o), $(CLIENT_OBJS))
# TEST_OBJS += $(filter-out $(S_OBJ_DIR/server.o), $(SERVER_OBJS))
# TEST_OBJS += $(patsubst $(TEST_DIR)/%.c, $())

# TEST_OBJS := $(filter-out $(OBJ_DIR)/minorly.o, $(OBJS))
# TEST_OBJS += $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TESTS))
TEST_LIBS := -lcheck -lm -pthread -lrt -lsubunit

CL_TEST_OBJS := $(filter-out $(C_OBJ_DIR)/client.o, $(CLIENT_OBJS))
CL_TEST_OBJS += $(patsubst $(TEST_DIR)/%.c, $(C_OBJ_DIR)/%.o, $(TESTS))

S_TEST_OBJS := $(filter-out $(S_OBJ_DIR)/server.o, $(SERVER_OBJS))
S_TEST_OBJS += $(patsubst $(TEST_DIR)/%.c, $(S_OBJ_DIR)/%.o, $(TESTS))

TEST_OBJS := $(CL_TEST_OBJS)
TEST_OBJS += $(S_TEST_OBJS)

all: $(CLIENT) $(SERVER)

debug: CFLAGS += $(DFLAGS)
debug: $(SERVER) $(CLIENT)

check: $(CHECK)

valgrind: CFLAGS += $(DFLAGS)
valgrind: clean $(BIN)
	@valgrind $(VFLAGS) ./$(BIN) $(ARGS)

profile: CFLAGS += $(PFLAGS)
profile: $(BIN)
	@mkdir $(DATA_DIR)

clean: 
	@rm -rf $(CLIENT) $(SERVER) $(C_OBJ_DIR) $(S_OBJ_DIR) $(CHECK) gmon.out server_unix_domain_socket
	
indent:
	indent -linux $(CLIENT_DIR)/*.c
	@rm $(CLIENT_DIR)/*.c~
	indent -linux $(SERVER_DIR)/*.c
	@rm $(SERVER_DIR)/*.c~
	# indent -linux $(TEST_DIR)/*.c
	# @rm $(TEST_DIR)/*.c~
	indent -linux $(HDR_DIR)/*.h
	@rm $(HDR_DIR)/*.h~

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(C_OBJ_DIR):
	@mkdir -p $@

$(S_OBJ_DIR):
	@mkdir -p $@

$(C_OBJ_DIR)/%.o: $(CLIENT_DIR)/%.c | $(C_OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(S_OBJ_DIR)/%.o: $(SERVER_DIR)/%.c | $(S_OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

# $(CHECK): $(TEST_OBJS)
# 	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS) $(LIB)
# 	./$(CHECK)

$(CHECK): $(TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS) $(LIB)
	./$(CHECK)

print:
	$(info $$TSTS is [$(TSTS)])
	$(info $$TST_OBJS is [$(TST_OBJS)])
	$(info $$TST_LIBS is [$(TST_LIBS)])
	$(info $$CL_SRC is [$(CLIENT_SRCS)])
	$(info $$CL_OBJ is [$(CLIENT_OBJS)])
	$(info $$S_SRC is [$(SERVER_SRCS)])
	$(info $$S_OBJ is [$(SERVER_OBJS)])