##########
# MY_INCLUDE
# MY_MACRO
# MY_FLAGS
# MY_LIBS
#########

TARGET_TIME = $(TARGET_SRC:_pftest.cc=_pftest.time)
TARGET_GPROF = $(TARGET_SRC:_pftest.cc=_pftest.gprof)
TARGET_GPERF = $(TARGET_SRC:_pftest.cc=_pftest.gperf)
TARGET_PERF = $(TARGET_SRC:_pftest.cc=_pftest.perf)

MAIN_DIR := ../../..
MARINE_DIR := $(MAIN_DIR)/marine

INCLUDE := $(MY_INCLUDE) -I$(MAIN_DIR)

CXXFLAGS := $(MY_MACRO) $(MY_FLAGS) -Wall -pthread $(INCLUDE)
LIB := $(MY_LIBS) -lrt
CFLAGS := $(CXXFLAGS)
PROTOC := protoc
PROTOCFLAGS := --cpp_out=.

TARGET_SRC := $(wildcard *_pftest.cc)
PB_SRC := $(wildcard *.proto)
PB_CC_SRC := $(PB_SRC:.proto=.pb.cc)
ALL_CC_SRC := $(wildcard *.cc) $(PB_CC_SRC)
CC_SRC := $(filter-out $(TARGET_SRC),$(ALL_CC_SRC))
C_SRC := $(wildcard *.c)

TIME_OBJ := $(CC_SRC:.cc=.time.o) $(C_SRC:.c=.time.o)
GPROF_OBJ := $(CC_SRC:.cc=.gprof.o) $(C_SRC:.c=.gprof.o)
GPERF_OBJ := $(CC_SRC:.cc=.gperf.o) $(C_SRC:.c=.gperf.o)
PERF_OBJ := $(CC_SRC:.cc=.perf.o) $(C_SRC:.c=.perf.o)
TARGET_TIME_OBJ := $(TARGET_SRC:.cc=.time.o)
TARGET_GPROF_OBJ := $(TARGET_SRC:.cc=.gprof.o)
TARGET_GPERF_OBJ := $(TARGET_SRC:.cc=.gperf.o)
TARGET_PERF_OBJ := $(TARGET_SRC:.cc=.perf.o)
ALL_OBJ := $(TIME_OBJ) $(GPROF_OBJ) $(GPERF_OBJ) $(PERF_OBJ) $(TARGET_TIME_OBJ) $(TARGET_GPROF_OBJ) $(TARGET_GPERF_OBJ) $(TARGET_PERF_OBJ)

DEP := $(ALL_OBJ:.o=.d)

CXXFLAGS += -MD
CFLAGS += -MD

TIME_FLAGS := -O2 -DNDEBUG
GPROF_FLAGS := -g -pg
GPERF_FLAGS := -g -O2 -DNDEBUG
PERF_FLAGS := -g -O2 -DNDEBUG

all: time gprof gperf perf

time: $(TARGET_TIME)

gprof: $(TARGET_GPROF)

gperf: $(TARGET_GPERF)

perf: $(TARGET_PERF)

%.time: %.time.o $(TIME_OBJ)
	$(CXX) $(TIME_FLAGS) $(CXXFLAGS) -D__TIME -o $@ $^ $(LIB)
	@strip $@

%.gprof: %.gprof.o $(GPROF_OBJ)
	$(CXX) $(GPROF_FLAGS) $(CXXFLAGS) -D__GPROF -o $@ $^ $(LIB)

%.gperf: %.gperf.o $(GPERF_OBJ)
	$(CXX) $(GPERF_FLAGS) $(CXXFLAGS) -D__GPERF -o $@ $^ $(LIB) -lprofiler

%.perf: %.perf.o $(PERF_OBJ)
	$(CXX) $(PERF_FLAGS) $(CXXFLAGS) -D__PERF -o $@ $^ $(LIB)

%.time.o: %.cc
	$(CXX) $(TIME_FLAGS) $(CXXFLAGS) -D__TIME -c -o $@ $<

%.time.o: %.c
	$(CC) $(TIME_FLAGS) $(CFLAGS) -D__TIME -c -o $@ $<

%.gprof.o: %.cc
	$(CXX) $(GPROF_FLAGS) $(CXXFLAGS) -D__GPROF -c -o $@ $<

%.gprof.o: %.c
	$(CC) $(GPROF_FLAGS) $(CFLAGS) -D__GPROF -c -o $@ $<

%.gperf.o: %.cc
	$(CXX) $(GPERF_FLAGS) $(CXXFLAGS) -D__GPERF -c -o $@ $<

%.gperf.o: %.c
	$(CC) $(GPERF_FLAGS) $(CFLAGS) -D__GPERF -c -o $@ $<

%.perf.o: %.cc
	$(CXX) $(PERF_FLAGS) $(CXXFLAGS) -D__PERF -c -o $@ $<

%.perf.o: %.c
	$(CC) $(PERF_FLAGS) $(CFLAGS) -D__PERF -c -o $@ $<

%.pb.h: %.proto
	$(PROTOC) $(PROTOCFLAGS) $<

%.pb.cc: %.proto
	$(PROTOC) $(PROTOCFLAGS) $<

clean:
	$(RM) *.time *.gprof *.gperf *.perf perf.data* gmon.out *.pprof
	@find . -name "*.o" | xargs rm -f
	@find . -name "*.d" | xargs rm -f

cleanall: clean
	$(RM) *.log tags
	@find . -name "*.pb.h" | xargs rm -f
	@find . -name "*.pb.cc" | xargs rm -f

.PHONY: all time gprof gperf perf clean cleanall

sinclude $(DEP)
