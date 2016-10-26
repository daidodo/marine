TARGET = $(SRC:.cc=)

CXXFLAGS := -O2 -DNDEBUG -I../..
LIBS :=

SRC := $(wildcard *.cc)
OBJ := $(SRC:.cc=.o)
DEP := $(OBJ:.o=.d)

CXXFLAGS += -MD

all: $(TARGET)
	strip $(TARGET)

debug: all

release: all

%_test: %_test.o
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

clean:
	$(RM) $(TARGET) *.o *.d

cleanall: clean
	$(RM) tags

.PHONY: all debug release clean cleanall

sinclude $(DEP)
