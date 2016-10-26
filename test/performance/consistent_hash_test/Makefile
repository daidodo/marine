#MY_FLAGS := -fno-strict-aliasing

sinclude ../pf_inc.mak

consistent_hash_pftest.time: consistent_hash_pftest.time.o
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -D__TIME -o $@ $^ $(LIB)
	@strip $@

sv_consistent_hash_pftest.time: sv_consistent_hash_pftest.time.o sv_consistent_hash.time.o sv_md5.time.o
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -D__TIME -o $@ $^ $(LIB)
	@strip $@

consistent_hash_pftest.gprof: consistent_hash_pftest.gprof.o
	$(CXX) -g -pg $(CXXFLAGS) -D__GPROF -o $@ $^ $(LIB)

sv_consistent_hash_pftest.gprof: sv_consistent_hash_pftest.gprof.o sv_consistent_hash.gprof.o sv_md5.gprof.o
	$(CXX) -g -pg $(CXXFLAGS) -D__GPROF -o $@ $^ $(LIB)

consistent_hash_pftest.gperf: consistent_hash_pftest.gperf.o
	$(CXX) -g $(CXXFLAGS) -D__GPERF -o $@ $^ $(LIB) -lprofiler

sv_consistent_hash_pftest.gperf: sv_consistent_hash_pftest.gperf.o sv_consistent_hash.gperf.o sv_md5.gperf.o
	$(CXX) -g $(CXXFLAGS) -D__GPERF -o $@ $^ $(LIB) -lprofiler

consistent_hash_pftest.perf: consistent_hash_pftest.perf.o
	$(CXX) -g $(CXXFLAGS) -D__PERF -o $@ $^ $(LIB)

sv_consistent_hash_pftest.perf: sv_consistent_hash_pftest.perf.o sv_consistent_hash.perf.o sv_md5.perf.o
	$(CXX) -g $(CXXFLAGS) -D__PERF -o $@ $^ $(LIB)

