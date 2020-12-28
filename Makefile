MAJOR_VER       := 0
MINOR_VER       := 0
PATCH_VER       := 0
BUILD_TAG       :=
BUILD_YEAR      := $(shell date +"%Y")

VERBOSE         := @
V               := $(VERBOSE)

MAKEFLAGS       += --no-print-directory --keep-going

profile ?= performance
include profiles/$(profile)/profile.mk

BUILD_PROF := $(profile)

EXE_BIN_DIR     := bin
EXE_LIB_DIR     := lib
LIB_SRC_DIR     := code/lib
API_INC_DIR     := code/api
CLI_SRC_DIR     := code/cli
LIB_SRCS        := $(shell find $(LIB_SRC_DIR) -name "*.cpp")
LIB_OBJS        := $(LIB_SRCS:%.cpp=%.o)
CLI_SRCS        := $(shell find $(CLI_SRC_DIR) -name "*.cpp")
CLI_OBJS        := $(CLI_SRCS:%.cpp=%.o)
API_INC_ARGS    := -I$(API_INC_DIR)
LIB_INC_ARGS    := -I$(API_INC_DIR) -I$(LIB_SRC_DIR)
LIB_NAME        := libdwt.a
CLI_NAME        := dwt
EXE_BIN         := $(EXE_BIN_DIR)/$(CLI_NAME)
EXE_LIB         := $(EXE_LIB_DIR)/$(LIB_NAME)
LIB_DEPS        := $(LIB_OBJS:%.o=%.d)
CLI_DEPS        := $(CLI_OBJS:%.o=%.d)

COMPILER         = $(CXX)
COMPILER        ?= g++
COMPILER_FLAGS  += -pipe -Wall -Wno-unused-parameter \
                   -fno-rtti -march=native \
                   -D_FILE_OFFSET_BITS=64 -fPIC \
                   $(CXXFLAGS) $(USE_FLAGS) \
                   -DMAJOR_VER=$(MAJOR_VER) \
                   -DMINOR_VER=$(MINOR_VER) \
                   -DPATCH_VER=$(PATCH_VER) \
                   -DBUILD_TAG=\"$(BUILD_TAG)\" \
                   -DBUILD_YEAR=\"$(BUILD_YEAR)\" \
                   -DBUILD_PROF=\"$(BUILD_PROF)\"

ifneq "$(findstring USE_THREADED_COMPILER=1,$(USE_FLAGS))" ""
EXT_LIBS += -lpthread
COMPILER_FLAGS += -pthread
endif

ifneq "$(findstring USE_COMPUTED_GOTO=1,$(USE_FLAGS))" ""
# Only use gnu++17 for computed goto support
COMPILER_FLAGS  += -std=gnu++17
else
# Otherwise stay with strict c++17 compliance
COMPILER_FLAGS  += -std=c++17 -pedantic
endif

release: COMPILER_FLAGS += -fomit-frame-pointer -flto -DNDEBUG=1
debug: COMPILER_FLAGS += -O0 -g -DDEBUG=1

# default target
.PHONY: release
release: all

.PHONY: debug
debug: all

.PHONY: clean
clean:
	$(V)rm -f $(CLI_DEPS) $(LIB_DEPS)
	$(V)rm -f $(LIB_OBJS)
	$(V)rm -f $(CLI_OBJS)
	$(V)rm -f $(EXE_BIN)
	$(V)rm -f $(EXE_LIB)

.PHONY: all
all: $(EXE_LIB) $(EXE_BIN)

$(LIB_SRC_DIR)/%.o: $(LIB_SRC_DIR)/%.cpp
	@echo "Compiling $<"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(LIB_INC_ARGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(V)$(COMPILER) -c $< $(LIB_INC_ARGS) $(COMPILER_FLAGS) -o $@

%.o: %.cpp
	@echo "Compiling $<"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(API_INC_ARGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(V)$(COMPILER) -c $< $(API_INC_ARGS) $(COMPILER_FLAGS) -o $@

$(EXE_LIB): $(LIB_OBJS)
	@mkdir -p $(EXE_LIB_DIR)
	@echo "Archiving $(EXE_LIB)"
	$(V)ar -rcs $(EXE_LIB) $(LIB_OBJS)
	$(V)ranlib $(EXE_LIB)

$(EXE_BIN): $(CLI_OBJS) $(EXE_LIB)
	@mkdir -p $(EXE_BIN_DIR)
	@echo "Linking $(EXE_BIN)"
	$(V)$(COMPILER) $(COMPILER_FLAGS) $(CLI_OBJS) $(EXE_LIB) -o $@
	$(V)strip $(EXE_BIN)

-include $(LIB_DEPS) $(CLI_DEPS)
