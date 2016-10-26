######## Pre-Defined ########
# MY_MACRO
# MY_INCLUDE
# MY_FLAGS
# MY_LIBS
#############################

TARGET = $(TARGET_OBJ:.o=)

MAIN_DIR := ../../..
MARINE_DIR := $(MAIN_DIR)/marine

ifeq ($(findstring release, $(MAKECMDGOALS)), release)
RELEASE := -DNDEBUG -O2
else
DEBUG := -g
endif

INCLUDE := $(MY_INCLUDE) -I$(MAIN_DIR) -I$(MARINE_DIR)

CXXFLAGS := $(MY_MACRO) $(RELEASE) $(DEBUG) $(MY_FLAGS) -Wall -Werror $(INCLUDE)
LIB := $(MY_LIBS) -lrt
PROTOC := protoc
PROTOCFLAGS := --cpp_out=.

PB_SRC := $(wildcard *.proto)
PB_CC_SRC := $(PB_SRC:.proto=.pb.cc)
TARGET_SRC := $(wildcard *_exper.cc)
ALL_SRC := $(wildcard *.cc) $(PB_CC_SRC)

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

%_exper: %_exper.o $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

clean:
	$(RM) *_exper *.o *.d

cleanall: clean
	$(RM) tags *.pb.cc *.pb.h *.log*

%.pb.cc: %.proto
	$(PROTOC) $(PROTOCFLAGS) $<

.PHONY: all debug release proto clean cleanall

.SECONDARY: $(ALL_OBJ)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
sinclude $(DEP)
endif
endif
