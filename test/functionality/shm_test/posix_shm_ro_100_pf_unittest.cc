#include <marine/posix_shm.hh>

#include "shm_comm.h"

const size_t kSize = 100;

typedef CPosixShm<CTest, true> __Shm;

typedef CPosixShm<CTest> __RwShm;

const char * const PATH = "shm_test.bin";

#include "posix_shm_ro_test.h"
