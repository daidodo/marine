#include <marine/sysv_shm.hh>

#include "shm_comm.h"

const size_t kSize = 1;

typedef CSysvShm<CTest> __Shm;

const char * const PATH = "Makefile";

#include "sysv_shm_rw_test.h"
