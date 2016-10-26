CXXFLAGS := -g

OBJ := main.o logger.o

all: test

test: $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ)

clean:
	$(RM) *.o test *.log.*

cleanall: clean
	$(RM) tags

.PHONY: clean cleanall
