######## User-Define ########
export MARINE_DIR
export MY_MACRO
export MY_INCLUDE
export MY_FLAGS
export MY_LIBS
#############################

MARINE_DIR := ../marine

# tcmalloc
#MY_LIBS += /usr/local/lib/libtcmalloc_minimal.a

TARGET = $(TARGET_OBJ:.o=)

ENGINE_DIR := ../reaper_engine
LIB_ENGINE := $(ENGINE_DIR)/libreaper.a

ifeq ($(findstring release, $(MAKECMDGOALS)), release)
CF_FLAGS := -DNDEBUG -O2
CF_LIB :=
else
CF_FLAGS := -g
CF_LIB :=
endif

INCLUDE := $(MY_INCLUDE) -I$(MARINE_DIR) -I$(ENGINE_DIR)

CXXFLAGS := $(MY_MACRO) $(CF_FLAGS) $(MY_FLAGS) -Wall -Werror -pthread $(INCLUDE)
CFLAGS := $(CXXFLAGS)
LIB := $(MY_LIBS) $(CF_LIB) -lprotobuf -lrt
PROTOC := protoc
PROTOCFLAGS := --cpp_out=.

TARGET_SRC := $(wildcard *_main.cc)
PB_SRC := $(wildcard *.proto)
C_SRC := $(wildcard *.c)
CC_SRC := $(wildcard *.cc) $(PB_SRC:.proto=.pb.cc)

TARGET_OBJ := $(TARGET_SRC:.cc=.o)
C_OBJ := $(C_SRC:.c=.o)
CC_OBJ := $(CC_SRC:.cc=.o)
ALL_OBJ := $(C_OBJ) $(CC_OBJ)
OBJ := $(filter-out $(TARGET_OBJ),$(ALL_OBJ))

DEP := $(ALL_OBJ:.o=.d)

CXXFLAGS += -MD
CFLAGS += -MD

all: debug

debug: reaper_debug $(TARGET)

release: reaper_release $(TARGET)

reaper_debug:
	$(MAKE) -e -C $(ENGINE_DIR) debug

reaper_release:
	$(MAKE) -e -C $(ENGINE_DIR) release

%_main: %_main.o $(OBJ) $(LIB_ENGINE)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

clean:
	$(RM) *.o *.d $(TARGET)

cleanall: clean reaper_clean
	$(RM) tags *.log *.pb.h *.pb.cc

reaper_clean:
	$(MAKE) -C $(ENGINE_DIR) cleanall

%.pb.cc: %.proto
	$(PROTOC) $(PROTOCFLAGS) $<

%.pb.h: %.proto
	$(PROTOC) $(PROTOCFLAGS) $<

.PHONY: all debug release reaper_debug reaper_release clean cleanall reaper_clean

.SECONDARY: $(ALL_OBJ)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
ifneq ($(MAKECMDGOALS),hsha_clean)
sinclude $(DEP)
endif
endif
endif
