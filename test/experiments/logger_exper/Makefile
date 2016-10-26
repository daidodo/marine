CXXFLAGS := -g

OBJ := main.o logger.o

test: $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ)

clean:
	$(RM) *.o test *.log.*

cleanall: clean
	$(RM) tags

.PHONY: clean cleanall
