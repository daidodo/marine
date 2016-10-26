MY_INCLUDE := -Iinner_inc
MY_FLAGS := -fno-strict-aliasing

sinclude ../pf_inc.mak

attr_stats_pftest.time: attr_stats_pftest.time.o
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -D__TIME -o $@ $^ $(LIB)
	@strip $@

attr_old_pftest.time: attr_old_pftest.time.o sv_attr.time.o
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -D__TIME -o $@ $^ $(LIB)
	@strip $@

attr_hash_pftest.time: attr_hash_pftest.time.o Attr_API.time.o
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -D__TIME -o $@ $^ $(LIB)
	@strip $@

attr_stats_pftest.gprof: attr_stats_pftest.gprof.o
	$(CXX) -g -pg $(CXXFLAGS) -D__GPROF -o $@ $^ $(LIB)

attr_old_pftest.gprof: attr_old_pftest.gprof.o sv_attr.gprof.o
	$(CXX) -g -pg $(CXXFLAGS) -D__GPROF -o $@ $^ $(LIB)

attr_hash_pftest.gprof: attr_hash_pftest.gprof.o Attr_API.gprof.o
	$(CXX) -g -pg $(CXXFLAGS) -D__GPROF -o $@ $^ $(LIB)

attr_stats_pftest.gperf: attr_stats_pftest.gperf.o
	$(CXX) -g $(CXXFLAGS) -D__GPERF -o $@ $^ $(LIB) -lprofiler

attr_old_pftest.gperf: attr_old_pftest.gperf.o sv_attr.gperf.o
	$(CXX) -g $(CXXFLAGS) -D__GPERF -o $@ $^ $(LIB) -lprofiler

attr_hash_pftest.gperf: attr_hash_pftest.gperf.o Attr_API.gperf.o
	$(CXX) -g $(CXXFLAGS) -D__GPERF -o $@ $^ $(LIB) -lprofiler

attr_stats_pftest.perf: attr_stats_pftest.perf.o
	$(CXX) -g $(CXXFLAGS) -D__PERF -o $@ $^ $(LIB)

attr_old_pftest.perf: attr_old_pftest.perf.o sv_attr.perf.o
	$(CXX) -g $(CXXFLAGS) -D__PERF -o $@ $^ $(LIB)

attr_hash_pftest.perf: attr_hash_pftest.perf.o Attr_API.perf.o
	$(CXX) -g $(CXXFLAGS) -D__PERF -o $@ $^ $(LIB)

