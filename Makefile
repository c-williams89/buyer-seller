.PHONY: all run clean check debug client server

CC := gcc-9

# compile flags
CFLAGS += -std=c18 -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings
CFLAGS += -Wvla -Wfloat-equal -lm
CFLAGS += -I./include

DFLAGS := -g3
VFLAGS += --leak-check=full --track-origins=yes
PFLAGS := -pg 

SRC_DIR := src
OBJ_DIR := obj
C_OBJ_DIR := cl_obj 
S_OBJ_DIR := serv_obj
DOCS_DIR := docs
TEST_DIR := test
HDR_DIR := include
LIB_DIR := lib
DATA_DIR := data
CLIENT_DIR := client_src
SERVER_DIR := server_src
SHARED_DIR := shared

LIB := $(wildcard $(LIB_DIR)/*.a)

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

CLIENT_SRCS := $(wildcard $(CLIENT_DIR)/*.c)
CLIENT_OBJS := $(patsubst $(CLIENT_DIR)/*.c, $(C_OBJ_DIR)/%.o, $(CLIENT_SRCS))

SERVER_SRCS := $(wildcard $(SERVER_DIR)/*.c)
SERVER_OBJS := $(patsubst $(SERVER_DIR)/*.c, $(S_OBJ_DIR)/%.o, $(SERVER_SRCS))

# executable
BIN := buyer
CLIENT := client
SERVER := server
CHECK := $(BIN)_check

TESTS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(filter-out $(OBJ_DIR)/minorly.o, $(OBJS))
TEST_OBJS += $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TESTS))
TEST_LIBS := -lcheck -lm -pthread -lrt -lsubunit

# all: $(BIN)
all: $(CLIENT) $(SERVER)
# client: $(CLIENT)

debug: CFLAGS += $(DFLAGS)
debug: $(BIN)

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
	indent -linux $(SRC_DIR)/*.c
	@rm $(SRC_DIR)/*.c~
	indent -linux $(TEST_DIR)/*.c
	@rm $(TEST_DIR)/*.c~
	indent -linux $(HDR_DIR)/*.h
	@rm $(HDR_DIR)/*.h~

# creates object files from .c files

$(C_OBJ_DIR)/%.o: 
	$(CLIENT_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(S_OBJ_DIR)/%.o: $(SERVER_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

# $(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
# 	@$(CC) $(CFLAGS) -c $< -o $@

$(C_OBJ_DIR):
	@mkdir -p $@
$(CLIENT_OBJS): | $(C_OBJ_DIR)
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@


# Server Side creation
$(SERVER_OBJS): | $(S_OBJ_DIR)
$(S_OBJ_DIR):
	@mkdir -p $@
$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(CHECK): $(TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS) $(LIB)
	./$(CHECK)

print:
	$(info $$SRCS is [$(SRCS)])
	$(info $$OBJS is [$(OBJS)])
	$(info $$TSTS is [$(TSTS)])
	$(info $$TST_OBJS is [$(TST_OBJS)])
	$(info $$TST_LIBS is [$(TST_LIBS)])
	$(info $$CL_SRC is [$(CLIENT_SRCS)])
	$(info $$CL_OBJ is [$(CLIENT_OBJS)])
