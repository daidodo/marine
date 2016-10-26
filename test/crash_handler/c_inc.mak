######## Pre-Defined ########
# MY_MACRO
# MY_INCLUDE
# MY_FLAGS
# MY_LIBS
# MY_SRC
#############################

SO_TARGET := ../libmy.so
TARGET = $(TARGET_OBJ:.o=)

MAIN_DIR := ../..
MARINE_DIR := $(MAIN_DIR)/marine

ifeq ($(findstring release, $(MAKECMDGOALS)), release)
RELEASE := -DNDEBUG -O2
else
DEBUG := -g
endif

INCLUDE := $(MY_INCLUDE) -I$(MAIN_DIR)
CXXFLAGS := $(MY_MACRO) $(MY_FLAGS) $(DEBUG) $(RELEASE) -Wall -Werror $(INCLUDE)
LIBS := $(MY_LIBS) $(SO_TARGET) -lpthread -lrt

ALL_SRC := $(MY_SRC) $(wildcard *.cc)
ALL_OBJ := $(ALL_SRC:.cc=.o)

TARGET_SRC := $(wildcard *_coretest.cc)
TARGET_OBJ := $(TARGET_SRC:.cc=.o)
OBJ := $(filter-out $(TARGET_OBJ), $(ALL_OBJ))
DEP := $(ALL_OBJ:.o=.d)

CXXFLAGS += -MD

all: debug

debug: so $(TARGET)

release: so $(TARGET)

so: $(SO_TARGET)

$(SO_TARGET): ../my.cc
	$(CXX) -shared -fPIC $(CXXFLAGS) $^ -o $@

%_coretest: %_coretest.o $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	$(RM) $(TARGET) *.o *.d ../*.d ../*.so ../*.o

clean_marine:
	$(RM) $(MARINE_DIR)/*.d $(MARINE_DIR)/*.o

cleanall: clean clean_marine
	$(RM) *.log core core.*

.PHONY: all debug release so clean clean_marine cleanall

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean_marine)
ifneq ($(MAKECMDGOALS),cleanall)
sinclude $(DEP)
endif
endif
endif
