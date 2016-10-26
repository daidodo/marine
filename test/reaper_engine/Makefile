######## Pre-Defined ########
# export MARINE_DIR
# export MY_MACRO
# export MY_INCLUDE
# export MY_FLAGS
#
# NOTE: use '$(MAKE) -e' to override local variables
#############################

MARINE_DIR := ../marine

TARGET = libreaper.a

ifeq ($(findstring release, $(MAKECMDGOALS)), release)
CF_FLAGS := -DNDEBUG -O2
else
CF_FLAGS := -g
endif

INCLUDE := $(MY_INCLUDE) -I$(MARINE_DIR)

CXXFLAGS := $(MY_MACRO) $(CF_FLAGS) $(MY_FLAGS) -Wall -Werror -pthread $(INCLUDE)
CFLAGS := $(CXXFLAGS)
ARFLAGS := r

C_SRC := $(wildcard *.c) $(wildcard impl/*.c)
CC_SRC := $(wildcard *.cc) $(wildcard impl/*.cc)

C_OBJ := $(C_SRC:.c=.o)
CC_OBJ := $(CC_SRC:.cc=.o)
ALL_OBJ := $(C_OBJ) $(CC_OBJ)
OBJ := $(ALL_OBJ)

DEP := $(ALL_OBJ:.o=.d)

CXXFLAGS += -MD
CFLAGS += -MD

all: debug

debug: $(TARGET)

release: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	$(RM) *.o *.d impl/*.o impl/*.d $(TARGET)

cleanall: clean
	$(RM) tags

.PHONY: all debug release clean cleanall

.SECONDARY: $(ALL_OBJ)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
sinclude $(DEP)
endif
endif
