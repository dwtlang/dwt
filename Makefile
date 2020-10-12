MAKEFLAGS += --no-print-directory --keep-going

LIB_DIR := lib
BIN_DIR := bin
INC_DIR := include
SRC_DIR := src/dwt
CPP_SRC := $(shell find $(SRC_DIR) -name "*.cpp")

FFI_DIR := test/ffi/ffi_tc_1
FFI_SRC := $(shell find $(FFI_DIR) -name "ffi_tc_1.cpp")
FFI_OBJ := $(FFI_SRC:%.cpp=%.o)

CPP_COMPILER = $(CXX)
CPP_COMPILER ?= g++

USE_FLAGS ?= -DUSE_STRICT_IEEE_754=1 -DUSE_COMPUTED_GOTO=1 -DUSE_DOUBLE_PRECISION_FP=1

CPP_FLAGS := -pipe -std=gnu++17 -fPIC -pthread -Wall -Wno-unused-parameter -fno-rtti -march=native -D_FILE_OFFSET_BITS=64 $(USE_FLAGS) $(CXXFLAGS)

ifeq "$(CPP_COMPILER)" "clang++"
PROF_GEN_FLAG = -fprofile-instr-generate=default.profdata
else
PROF_GEN_FLAG = -fprofile-generate
endif

ifeq "$(CPP_COMPILER)" "clang++"
PROF_USE_FLAG = -fprofile-instr-use=default.profile
else
PROF_USE_FLAG = -fprofile-use
endif

check: CPP_FLAGS += -fsyntax-only
debug: CPP_FLAGS += -O0 -g -DDEBUG=1
release: CPP_FLAGS += -O3 -flto -DNDEBUG=1
profile: CPP_FLAGS += $(PROF_GEN_FLAG)
pgo: CPP_FLAGS += $(PROF_USE_FLAG)

V ?= @

OBJ_FILES := $(CPP_SRC:%.cpp=%.o)
PGO_FILES := $(CPP_SRC:%.cpp=%.gcda)
ASM_FILES := $(CPP_SRC:%.cpp=%.s)
BINARY := $(BIN_DIR)/dwt
LIBRARY := $(LIB_DIR)/libdwt.so
FFI_BIN := $(BIN_DIR)/ffi

FUZZER_SRC := tools/fuzzer.cpp
FUZZER_OBJ := $(FUZZER_SRC:%.cpp=%.o)
FUZZER := tools/fuzzer

DEPS := $(OBJ_FILES:.o=.d)

.PHONY: debug
debug: all

.PHONY: release
release: all

.PHONY: check
check: all

.PHONY: profile
profile: release

.PHONY: merge
merge:
ifeq "$(CPP_COMPILER)" "clang++"
	llvm-profdata merge --output default.profile *.profdata
endif

.PHONY: pgo
pgo: merge clean release

.PHONY: all
all: $(LIBRARY) $(BINARY) $(FUZZER) $(FFI_BIN)

.PHONY: asm
asm: $(ASM_FILES)

$(FFI_BIN): $(FFI_OBJ)
	@echo "   $(CPP_COMPILER)      $(FFI_BIN)"
	$(V)$(CPP_COMPILER) -I$(INC_DIR) $(FFI_OBJ) $(OBJ_FILES) $(CPP_FLAGS) -L$(LIB_DIR) -ldwt -lpthread -o $@

$(FUZZER): $(FUZZER_OBJ) $(OBJ_FILES)
	@echo "   $(CPP_COMPILER)      $(FUZZER)"
	$(V)$(CPP_COMPILER) $(FUZZER_OBJ) $(filter-out %/main.o,$(OBJ_FILES)) $(CPP_FLAGS) -lpthread -o $@

$(LIBRARY): $(OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	@echo "   $(CPP_COMPILER)      $(LIBRARY)"
	$(V)$(CPP_COMPILER) $(OBJ_FILES) $(CPP_FLAGS) -shared -fPIC -lpthread -o $(LIBRARY)

$(BINARY): $(LIBRARY)
	@mkdir -p $(BIN_DIR)
	@echo "   $(CPP_COMPILER)      $(BINARY)"
	$(V)$(CPP_COMPILER) $(SRC_DIR)/../main.cpp -I$(INC_DIR) $(OBJ_FILES) $(CPP_FLAGS) -L$(LIB_DIR) -ldwt -lpthread -o $@

%.o: %.cpp
	@echo "   $(CPP_COMPILER)      $(patsubst src/%.cpp,%.o,$<)"
	$(V)$(CPP_COMPILER) $(CPP_FLAGS) -I$(SRC_DIR) -I$(INC_DIR) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(V)$(CPP_COMPILER) -c $< -I$(SRC_DIR) -I$(INC_DIR) $(CPP_FLAGS) -o $@

%.s: %.cpp
	@echo "   $(CPP_COMPILER)      $(patsubst src/%.cpp,%.o,$<)"
	$(V)$(CPP_COMPILER) -S $< -I$(SRC_DIR) -I$(INC_DIR) $(CPP_FLAGS) -o $@

.PHONY: clean
clean:
	$(V)rm -rf $(DEPS)
	$(V)rm -rf $(FUZZER_OBJ)
	$(V)rm -rf $(OBJ_FILES)
	$(V)rm -rf $(ASM_FILES)
	$(V)rm -rf $(BINARY)
	$(V)rm -rf $(FUZZER)
	$(V)rm -rf $(FFI_BIN)
	$(V)rm -rf $(FFI_OBJ)

.PHONY: purge
purge: clean
	$(V)rm -rf $(PGO_FILES)

-include $(DEPS)

