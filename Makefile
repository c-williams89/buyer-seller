.PHONY: all run clean check debug client server

CC := gcc-9

# compile flags
CFLAGS += -std=c18 -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings
CFLAGS += -Wvla -Wfloat-equal -lm -pthread
CFLAGS += -I./include

CHECK_FLAGS += -I./include -lcheck -lm -pthread -lrt -lsubunit

DFLAGS := -g3
VFLAGS += --leak-check=full --track-origins=yes
PFLAGS := -pg 

C_OBJ_DIR := cl_obj
S_OBJ_DIR := serv_obj
DOCS_DIR := docs
TEST_DIR := test
HDR_DIR := include
DATA_DIR := profile_data
CLIENT_DIR := client_src
SERVER_DIR := server_src

CLIENT_SRCS := $(wildcard $(CLIENT_DIR)/*.c)
CLIENT_OBJS := $(patsubst $(CLIENT_DIR)/%.c, $(C_OBJ_DIR)/%.o, $(CLIENT_SRCS))

SERVER_SRCS := $(wildcard $(SERVER_DIR)/*.c)
SERVER_OBJS := $(patsubst $(SERVER_DIR)/%.c, $(S_OBJ_DIR)/%.o, $(SERVER_SRCS))

# executables
CLIENT := client
SERVER := server
CHECK := buyer_check

all: $(CLIENT) $(SERVER)

debug: CFLAGS += $(DFLAGS)
debug: $(SERVER) $(CLIENT)

check:
	$(CC) test/test_client_helper.c client_src/client_helper.c $(CHECK_FLAGS)
	./a.out 

valgrind: CFLAGS += $(DFLAGS)
valgrind: clean $(BIN)
	@valgrind $(VFLAGS) ./$(BIN) $(ARGS)

profile: CFLAGS += $(PFLAGS)
profile: $(BIN)
	@mkdir $(DATA_DIR)

clean: 
	@rm -rf $(CLIENT) $(SERVER) $(C_OBJ_DIR) $(S_OBJ_DIR) $(CHECK) $(DATA_DIR) gmon.out server_unix_domain_socket a.out
	
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

print:
	$(info $$TSTS is [$(TSTS)])
	$(info $$TST_OBJS is [$(TST_OBJS)])
	$(info $$TST_LIBS is [$(TST_LIBS)])
	$(info $$CL_SRC is [$(CLIENT_SRCS)])
	$(info $$CL_OBJ is [$(CLIENT_OBJS)])
	$(info $$S_SRC is [$(SERVER_SRCS)])
	$(info $$S_OBJ is [$(SERVER_OBJS)])