#include <marine/posix_shm.hh>

#include "shm_comm.h"

const size_t kSize = 100;

typedef CPosixShm<CTest, false, false> __Shm;

const char * const PATH = "shm_test.bin";

#include "posix_shm_rw_test.h"
