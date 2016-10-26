######## Pre-Defined ########
# MY_MACRO
# MY_INCLUDE
# MY_FLAGS
# MY_LIBS
#############################

TARGET = $(TARGET_OBJ:.o=)

MAIN_DIR := ../../..
GTEST_DIR := ../gtest
RUN := ../run_test.sh

ifeq ($(findstring release, $(MAKECMDGOALS)), release)
RELEASE := -DNDEBUG -O2
else
DEBUG := -g
endif

INCLUDE := $(MY_INCLUDE) -I$(MAIN_DIR) -I$(GTEST_DIR) -I$(GTEST_DIR)/include

CXXFLAGS := $(MY_MACRO) $(RELEASE) $(DEBUG) $(MY_FLAGS) -Wall -Werror -pthread $(INCLUDE)
LIB := $(MY_LIBS) -lrt
PROTOC := protoc
PROTOCFLAGS := --cpp_out=.

GTEST_SRC := $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
PB_SRC := $(wildcard *.proto)
PB_CC_SRC := $(PB_SRC:.proto=.pb.cc)
TARGET_SRC := $(wildcard *_unittest.cc)
ALL_SRC := $(wildcard *.cc) $(PB_CC_SRC) $(GTEST_SRC)

TARGET_OBJ := $(TARGET_SRC:.cc=.o)
ALL_OBJ := $(ALL_SRC:.cc=.o)
OBJ := $(filter-out $(TARGET_OBJ), $(ALL_OBJ))

DEP := $(ALL_OBJ:.o=.d)

CXXFLAGS += -MD

all: debug

debug: proto $(TARGET)

release: proto $(TARGET)
#	@strip $(TARGET)

proto: $(PB_CC_SRC)

%_unittest: %_unittest.o $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

clean:
	$(RM) *_unittest *.o *.d

clean_gtest:
	$(RM) $(GTEST_DIR)/src/*.o $(GTEST_DIR)/src/*.d

cleanall: clean clean_gtest
	$(RM) tags *.pb.cc *.pb.h *.log*

run: all
	@$(RUN) $(TARGET)

%.pb.cc: %.proto
	$(PROTOC) $(PROTOCFLAGS) $<

.PHONY: all debug release proto clean clean_gtest cleanall run

.SECONDARY: $(ALL_OBJ)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_gtest)
ifneq ($(MAKECMDGOALS),cleanall)
sinclude $(DEP)
endif
endif
endif
