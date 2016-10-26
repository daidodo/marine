#include <marine/posix_shm.hh>

#include "shm_comm.h"

const size_t kSize = 1;

typedef CPosixShm<CTest, true, false> __Shm;

typedef CPosixShm<CTest, false, false> __RwShm;

const char * const PATH = "shm_test.bin";

#include "posix_shm_ro_test.h"
