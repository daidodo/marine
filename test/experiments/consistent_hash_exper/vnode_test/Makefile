TARGET = $(CC_SRC:.cc=)

CXXFLAGS := -O2 -DNDEBUG -I../../../..
LIBS :=
CFLAGS := $(CXXFLAGS)

CC_SRC := $(wildcard *.cc)
C_SRC := $(wildcard *.c)
CC_OBJ := $(CC_SRC:.cc=.o)
C_OBJ := $(C_SRC:.c=.o)
OBJ := $(CC_OBJ) $(C_OBJ)
DEP := $(OBJ:.o=.d)

CXXFLAGS += -MD
CFLAGS += -MD

all: $(TARGET)
	strip $(TARGET)

debug: all

release: all

%_test: %_test.o $(C_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) $(TARGET) *.o *.d

cleanall: clean
	$(RM) tags

.PHONY: all debug release clean cleanall

sinclude $(DEP)
