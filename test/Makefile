VERBOSE         := @
V               := $(VERBOSE)

MAKEFLAGS       += --no-print-directory --keep-going

TESTER_BIN      := tester
TESTER_SRC      := tester.cpp
TESTER_OBJS     := $(TESTER_SRC:%.cpp=%.o)
FUZZER_BIN      := fuzzer
FUZZER_SRC      := fuzzer.cpp
FUZZER_OBJS     := $(FUZZER_SRC:%.cpp=%.o)

FFI_BIN         := ffi/dwt
FFI_SRC         := ffi/ffi.cpp
FFI_OBJS        := $(FFI_SRC:%.cpp=%.o)

DWT_LIB_DIR     := ../lib
LIB_SRC_DIR     := ../code/lib
API_INC_DIR     := ../code/api
API_INC_ARGS    := -I$(API_INC_DIR)
LIB_INC_ARGS    := -I$(API_INC_DIR) -I$(LIB_SRC_DIR)
LIB_NAME        := libdwt.a
DWT_LIB         := $(DWT_LIB_DIR)/$(LIB_NAME)

COMPILER         = $(CXX)
COMPILER        ?= g++
COMPILER_FLAGS  += -pipe -Wall -Wno-unused-parameter \
                   -fno-rtti -march=native -O3 \
                   -D_FILE_OFFSET_BITS=64 -fPIC -std=c++17 -pedantic -pthread \
                   $(CXXFLAGS)

EXT_LIBS += -lpthread

.PHONY: all
all: $(FUZZER_BIN) $(TESTER_BIN) $(FFI_BIN)

.PHONY: clean
clean:
	$(V)rm -f $(FUZZER_BIN) $(TESTER_BIN) $(FFI_BIN) $(FUZZER_OBJS) $(TESTER_OBJS) $(FFI_OBJS)

%.o: %.cpp
	@echo "Compiling $<"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(LIB_INC_ARGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(V)$(COMPILER) -c $< $(LIB_INC_ARGS) $(API_INC_ARGS) $(COMPILER_FLAGS) -o $@

$(FUZZER_BIN): $(FUZZER_OBJS) $(DWT_LIB)
	@echo "Linking $(FUZZER_BIN)"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(FUZZER_OBJS) $(DWT_LIB) -o $@
	$(V)strip $(FUZZER_BIN)

$(TESTER_BIN): $(TESTER_OBJS)
	@echo "Linking $(TESTER_BIN)"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(TESTER_OBJS) -o $@
	$(V)strip $(TESTER_BIN)

$(FFI_BIN): $(FFI_OBJS)
	@echo "Linking $(FFI_BIN)"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(FFI_OBJS) $(DWT_LIB) -o $@
	$(V)strip $(FFI_BIN)

-include $(LIB_DEPS) $(CLI_DEPS)
