MAJOR_VER       := 0
MINOR_VER       := 0
PATCH_VER       := 0
BUILD_TAG       :=
BUILD_YEAR      := $(shell date +"%Y")

USE_FLAGS       ?= -DUSE_STRICT_IEEE_754=1 \
                   -DUSE_COMPUTED_GOTO=1 \
                   -DUSE_DOUBLE_PRECISION_FP=1 \
                   -DMAJOR_VER=$(MAJOR_VER) \
                   -DMINOR_VER=$(MINOR_VER) \
                   -DPATCH_VER=$(PATCH_VER) \
                   -DBUILD_TAG=\"$(BUILD_TAG)\" \
                   -DBUILD_YEAR=\"$(BUILD_YEAR)\"

VERBOSE         := @
V               := $(VERBOSE)

MAKEFLAGS       += --no-print-directory --keep-going
LIB_DIR         := lib
BIN_DIR         := bin
INC_DIR         := include
SRC_DIR         := src
LIB_SRC_DIR     := $(SRC_DIR)/lib
CLI_SRC_DIR     := $(SRC_DIR)/cli
LIB_SRC         := $(shell find $(LIB_SRC_DIR) -name "*.cpp")
CLI_SRC         := $(shell find $(CLI_SRC_DIR) -name "*.cpp")
LIB_OBS         := $(LIB_SRC:%.cpp=%.o)
CLI_OBS         := $(CLI_SRC:%.cpp=%.o)
LIB_BASENAME    := libdwt.so
LIB_NAME        := $(LIB_BASENAME).$(MAJOR_VER).$(MINOR_VER).$(PATCH_VER)
LIB_LNK         := $(LIB_DIR)/$(LIB_BASENAME).$(MAJOR_VER)
DWT_LIB         := $(LIB_DIR)/$(LIB_NAME)
AR_BASENAME     := libdwt.a
AR_NAME         := $(AR_BASENAME).$(MAJOR_VER).$(MINOR_VER).$(PATCH_VER)
AR_LNK          := $(LIB_DIR)/$(AR_BASENAME).$(MAJOR_VER)
DWT_AR          := $(LIB_DIR)/$(AR_NAME)
DWT_CLI         := $(BIN_DIR)/dwt
TOOLS_DIR       := tools
FUZZ_DIR        := $(TOOLS_DIR)
FUZZ_SRC        := $(FUZZ_DIR)/fuzzer.cpp
FUZZ_OBS        := $(FUZZ_SRC:%.cpp=%.o)
FUZZ_BIN        := $(FUZZ_DIR)/fuzzer
TEST_DIR        := test
FFI_TEST_DIR    := $(TEST_DIR)/ffi
FFI_TEST_BIN    := $(BIN_DIR)/ffi
FFI_TEST_SRC    := $(shell find $(FFI_TEST_DIR) -name "*.cpp")
FFI_TEST_OBS    := $(FFI_TEST_SRC:%.cpp=%.o)
COMPILER         = $(CXX)
COMPILER        ?= g++
COMPILER_FLAGS   = -pipe -pthread -Wall -Wno-unused-parameter \
                      -fno-rtti -march=native \
                      -D_FILE_OFFSET_BITS=64 -fPIC \
                      $(CXXFLAGS) $(USE_FLAGS)

ifneq "$(findstring USE_COMPUTED_GOTO=1,$(USE_FLAGS))" ""
# Only use gnu++17 for computed goto support
COMPILER_FLAGS  += -std=gnu++17
else
# Otherwise stay with strict c++17 compliance
COMPILER_FLAGS  += -std=c++17 -pedantic
endif

COMPILER_INCL   := -I$(INC_DIR) -I$(LIB_SRC_DIR) -I$(CLI_SRC_DIR) \
                   -I$(FFI_TEST_DIR)
LIB_DEPS        := $(LIB_OBS:%.o=%.d)
CLI_DEPS        := $(CLI_OBS:%.o=%.d)
FFI_TEST_DEPS   := $(FFI_TEST_OBS:%.o=%.d)
ALL_DEPS        := $(LIB_DEPS) $(CLI_DEPS) $(FFI_TEST_DEPS)
GCDA_FILES      := $(shell find -name "*.gcda")

optimised: COMPILER_FLAGS += -O2 -flto -DNDEBUG=1
debug: COMPILER_FLAGS += -O0 -g -DDEBUG=1
profile: COMPILER_FLAGS += -fprofile-generate -O2 -flto -DNDEBUG=1
pgo: COMPILER_FLAGS += -fprofile-use
pgo: MAKEFLAGS += --always-make

.PHONY: optimised
optimised: all

.PHONY: debug
debug: all

.PHONY: profile
.profile:
ifneq "$(COMPILER)" "g++"
	$(error use g++ for profile target)
endif

.PHONY: pgo
pgo:
ifneq "$(COMPILER)" "g++"
	$(error use g++ for pgo target)
endif

profile: optimised

pgo: clean optimised

.PHONY: clean
clean:
	$(V)rm -f $(ALL_DEPS)
	$(V)rm -f $(LIB_OBS)
	$(V)rm -f $(CLI_OBS)
	$(V)rm -f $(FUZZ_OBS)
	$(V)rm -f $(FUZZ_BIN)
	$(V)rm -f $(FFI_TEST_OBS)
	$(V)rm -f $(FFI_TEST_BIN)
	$(V)rm -f $(CLI_BIN)
	$(V)rm -rf $(LIB_DIR)/libdwt.*

.PHONY: purge
purge: clean
	$(V)rm -rf $(GCDA_FILES)

.PHONY: all
all: $(DWT_LIB) $(DWT_AR) $(DWT_CLI) $(FUZZ_BIN) $(FFI_TEST_BIN)

%.o: %.cpp
	@echo "   CC      $(patsubst src/%.cpp,%.o,$<)"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(COMPILER_INCL) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(V)$(COMPILER) -c $< $(COMPILER_INCL) $(COMPILER_FLAGS) -o $@

$(DWT_LIB): $(LIB_OBS)
	@mkdir -p $(LIB_DIR)
	@echo "   LD      $(DWT_LIB)"
	$(V)$(COMPILER) $(LIB_OBS) $(COMPILER_FLAGS) -shared -fPIC -o $(DWT_LIB)
	@echo "   LN      $(LIB_LNK) ~> $(DWT_LIB)"
	$(V)ln -nrs $(DWT_LIB) $(LIB_LNK)
	@echo "   LN      $(LIB_DIR)/$(LIB_BASENAME) ~> $(LIB_LNK)"
	$(V)ln -nrs $(LIB_LNK) $(PWD)/$(LIB_DIR)/$(LIB_BASENAME)

$(DWT_AR): $(LIB_OBS)
	@mkdir -p $(LIB_DIR)
	@echo "   AR      $(DWT_AR)"
	$(V)ar -rcs $(DWT_AR) $(LIB_OBS)
	$(V)ranlib $(DWT_AR)
	@echo "   LN      $(AR_LNK) ~> $(DWT_AR)"
	$(V)ln -nrs $(DWT_AR) $(AR_LNK)
	@echo "   LN      $(LIB_DIR)/$(AR_BASENAME) ~> $(AR_LNK)"
	$(V)ln -nrs $(AR_LNK) $(PWD)/$(LIB_DIR)/$(AR_BASENAME)

$(DWT_CLI): $(DWT_LIB) $(CLI_OBS)
	@mkdir -p $(BIN_DIR)
	@echo "   LD      $(DWT_CLI)"
	$(V)$(COMPILER) $(CLI_OBS) $(COMPILER_INCL) $(COMPILER_FLAGS) -L$(LIB_DIR) -Wl,-rpath='$$ORIGIN'/../$(LIB_DIR) -ldwt -lpthread -o $@

$(FUZZ_BIN): $(DWT_LIB) $(FUZZ_OBS)
	@mkdir -p $(FUZZ_DIR)
	@echo "   LD      $(FUZZ_BIN)"
	$(V)$(COMPILER) $(FUZZ_OBS) $(COMPILER_INCL) $(COMPILER_FLAGS) -L$(LIB_DIR) -Wl,-rpath='$$ORIGIN'/../$(LIB_DIR) -ldwt -lpthread -o $@

$(FFI_TEST_BIN): $(DWT_LIB) $(FFI_TEST_OBS)
	@mkdir -p $(BIN_DIR)
	@echo "   LD      $(FFI_TEST_BIN)"
	$(V)$(COMPILER) $(FFI_TEST_OBS) $(COMPILER_INCL) $(COMPILER_FLAGS) -L$(LIB_DIR) -Wl,-rpath='$$ORIGIN'/../$(LIB_DIR) -ldwt -lpthread -o $@

-include $(ALL_DEPS)
