#include <marine/sysv_shm.hh>

#include "shm_comm.h"

const size_t kSize = 100;

typedef CSysvShm<CTest, kSize, true> __Shm;

typedef CSysvShm<CTest, kSize, false> __RwShm;

const char * const PATH = "Makefile";

#include "sysv_shm_ro_test.h"
