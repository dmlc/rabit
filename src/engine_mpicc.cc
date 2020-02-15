/*!
 *  Copyright (c) 2014 by Contributors
 * \file engine_mpicc.cc
 * \brief this file gives an implementation of engine interface using MPI,
 *   this will allow rabit program to run with MPI, but do not comes with fault tolerant
 *
 * \author Tianqi Chen
 *
 * \brief MPICXX would not be supported at all.
 * \author Rewrited by WCC in MPICC.
 */
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

/*!
 * \brief Get rid of MPI C++ calls/libraries and add supports
 *        for R, pbdMPI, and Windows.
 */
#ifndef PBDR_SKIP_MPICXX
  #define PBDR_SKIP_MPICXX  //WCC Force to skip CXX from "mpi.h".
#endif
#ifndef MPICH_SKIP_MPICXX
  #define MPICH_SKIP_MPICXX
#endif
#ifndef OMPI_SKIP_MPICXX
  #define OMPI_SKIP_MPICXX
#endif
#ifdef WIN
  #include <_mingw.h>
#endif
#ifdef _WIN64
  #include <stdint.h>
#endif
#ifdef _HAVE_R_
  #include <R.h>
  #include <Rinternals.h>
#endif

#include <mpi.h>
#include <cstdio>
#include "rabit/internal/engine.h"
#include "rabit/internal/utils.h"

namespace rabit {

namespace utils {
    bool STOP_PROCESS_ON_ERROR = true;
}

namespace engine {
/*! \brief implementation of engine using MPI */
class MPIEngine : public IEngine {
 public:
  MPIEngine(void) {
    version_number = 0;
  }
  virtual void Allgather(void *sendrecvbuf_,
                             size_t total_size,
                             size_t slice_begin,
                             size_t slice_end,
                             size_t size_prev_slice,
                             const char* _file,
                             const int _line,
                             const char* _caller) {
    utils::Error("MPIEngine:: Allgather is not supported");
  }
  virtual void Allreduce(void *sendrecvbuf_,
                         size_t type_nbytes,
                         size_t count,
                         ReduceFunction reducer,
                         PreprocFunction prepare_fun,
                         void *prepare_arg,
                         const char* _file,
                         const int _line,
                         const char* _caller) {
    utils::Error("MPIEngine:: Allreduce is not supported,"\
                 "use Allreduce_ instead");
  }
  virtual int GetRingPrevRank(void) const {
    utils::Error("MPIEngine:: GetRingPrevRank is not supported");
    return -1;
  }
  virtual void Broadcast(void *sendrecvbuf_, size_t size, int root,
    const char* _file, const int _line,
    const char* _caller) {
    MPI_Bcast(sendrecvbuf_, size, MPI_CHAR, root, MPI_COMM_WORLD);
  }
  virtual void InitAfterException(void) {
    utils::Error("MPI is not fault tolerant");
  }
  virtual int LoadCheckPoint(Serializable *global_model,
                             Serializable *local_model = NULL) {
    return 0;
  }
  virtual void CheckPoint(const Serializable *global_model,
                          const Serializable *local_model = NULL) {
    version_number += 1;
  }
  virtual void LazyCheckPoint(const Serializable *global_model) {
    version_number += 1;
  }
  virtual int VersionNumber(void) const {
    return version_number;
  }
  /*! \brief get rank of current node */
  virtual int GetRank(void) const {
    int comm_world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_world_rank);
    return comm_world_rank;
  }
  /*! \brief get total number of */
  virtual int GetWorldSize(void) const {
    int comm_world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_world_size);
    return comm_world_size;
  }
  /*! \brief whether it is distributed */
  virtual bool IsDistributed(void) const {
    return true;
  }
  /*! \brief get the host name of current node */
  virtual std::string GetHost(void) const {
    int len;
    char name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(name, &len);
    name[len] = '\0';
    return std::string(name);
  }
  virtual void TrackerPrint(const std::string &msg) {
    // simply print information into the tracker
    if (GetRank() == 0) {
      utils::Printf("%s", msg.c_str());
    }
  }

 private:
  int version_number;
};

// singleton sync manager
MPIEngine manager;

/*! \brief initialize the synchronization module */
bool Init(int argc, char *argv[]) {
  try {
    MPI_Init(&argc, &argv);
    return true;
  } catch (const std::exception& e) {
#ifndef _HAVE_R_
    fprintf(stderr, " failed in MPI Init %s\n", e.what());
#else
    REprintf(" failed in MPI Init %s\n", e.what());
#endif
    return false;
  }
}
/*! \brief finalize syncrhonization module */
bool Finalize(void) {
  int flag;
  MPI_Finalized(&flag);
  if (!flag) {
    try {
      MPI_Finalize();
      return true;
    } catch (const std::exception& e) {
#ifndef _HAVE_R_
      fprintf(stderr, "failed in MPI shutdown %s\n", e.what());
#else
      REprintf("failed in MPI shutdown %s\n", e.what());
#endif
      return false;
    }
  } else {
    return true;
  }
}

/*! \brief singleton method to get engine */
IEngine *GetEngine(void) {
  return &manager;
}
// transform enum to MPI data type
inline MPI_Datatype GetType(mpi::DataType dtype) {
  using namespace mpi;
  //WCC Note MPI_* below are of type "MPI_Datatype" assuming in "C struct".
  switch (dtype) {
    case kChar: return MPI_CHAR;
    case kUChar: return MPI_BYTE;
    case kInt: return MPI_INT;
    case kUInt: return MPI_UNSIGNED;
    case kLong: return MPI_LONG;
    case kULong: return MPI_UNSIGNED_LONG;
    case kFloat: return MPI_FLOAT;
    case kDouble: return MPI_DOUBLE;
    case kLongLong: return MPI_LONG_LONG;
    case kULongLong: return MPI_UNSIGNED_LONG_LONG;
  }
  utils::Error("unknown mpi::DataType");
  return MPI_CHAR;
}
// transform enum to MPI OP
inline MPI_Op GetOp(mpi::OpType otype) {
  using namespace mpi;
  switch (otype) {
    //WCC Note MPI_* below are of type "MPI_Op" assuming in "C struct".
    case kMax: return MPI_MAX;
    case kMin: return MPI_MIN;
    case kSum: return MPI_SUM;
    case kBitwiseOR: return MPI_BOR;
  }
  utils::Error("unknown mpi::OpType");
  return MPI_MAX;
}
// perform in-place allreduce, on sendrecvbuf
void Allreduce_(void *sendrecvbuf,
                size_t type_nbytes,
                size_t count,
                IEngine::ReduceFunction red,
                mpi::DataType dtype,
                mpi::OpType op,
                IEngine::PreprocFunction prepare_fun,
                void *prepare_arg,
                const char* _file,
                const int _line,
                const char* _caller) {
  if (prepare_fun != NULL) prepare_fun(prepare_arg);
   MPI_Allreduce(MPI_IN_PLACE, sendrecvbuf,
                 count, GetType(dtype), GetOp(op), MPI_COMM_WORLD);
}

// code for reduce handle
ReduceHandle::ReduceHandle(void)
    : handle_(NULL), redfunc_(NULL), htype_(NULL) {
}
ReduceHandle::~ReduceHandle(void) {
  int flag;
  MPI_Finalized(&flag);
  if (!flag) {
    if (handle_ != NULL) {
      MPI_Op_free((MPI_Op*) handle_);
      free(handle_);
    }
    if (htype_ != NULL) {
      MPI_Type_free((MPI_Datatype*) htype_);
      free(htype_);
    }
  }
}
int ReduceHandle::TypeSize(MPI_Datatype dtype) {
  int dtype_size;
  MPI_Type_size(dtype, &dtype_size);
  return dtype_size;
}
void ReduceHandle::Init(IEngine::ReduceFunction redfunc, size_t type_nbytes) {
  utils::Assert(handle_ == NULL, "cannot initialize reduce handle twice");
  if (type_nbytes != 0) {
    MPI_Datatype *pbdr_mpi_dtype;
    pbdr_mpi_dtype = (MPI_Datatype*) malloc(sizeof(MPI_Datatype));
    if (type_nbytes % 8 == 0) {
      MPI_Type_contiguous(type_nbytes / sizeof(long), MPI_LONG, pbdr_mpi_dtype);
    } else if (type_nbytes % 4 == 0) {
      MPI_Type_contiguous(type_nbytes / sizeof(int), MPI_INT, pbdr_mpi_dtype);
    } else {
      MPI_Type_contiguous(type_nbytes, MPI_CHAR, pbdr_mpi_dtype);
    }
    MPI_Type_commit(pbdr_mpi_dtype);
    created_type_nbytes_ = type_nbytes;
    htype_ = pbdr_mpi_dtype;
  } else {
    if (htype_ != NULL) {
      MPI_Type_free((MPI_Datatype*) htype_);
      free(htype_);
    }
  }
  MPI_Op *pbdr_mpi_op;
  pbdr_mpi_op = (MPI_Op*) malloc(sizeof(MPI_Op));
  MPI_Op_create((MPI_User_function*) redfunc, true, pbdr_mpi_op);
  handle_ = pbdr_mpi_op;
}
void ReduceHandle::Allreduce(void *sendrecvbuf,
                             size_t type_nbytes, size_t count,
                             IEngine::PreprocFunction prepare_fun,
                             void *prepare_arg,
                             const char* _file,
                             const int _line,
                             const char* _caller) {
  utils::Assert(handle_ != NULL, "must initialize handle to call AllReduce");
  MPI_Datatype *pbdr_mpi_dtype;
  MPI_Op *pbdr_mpi_op;
  pbdr_mpi_dtype = (MPI_Datatype*) htype_;
  pbdr_mpi_op = (MPI_Op*) handle_;
  if (created_type_nbytes_ != type_nbytes || pbdr_mpi_dtype == NULL) {
    if (pbdr_mpi_dtype == NULL) {
      pbdr_mpi_dtype = (MPI_Datatype*) malloc(sizeof(MPI_Datatype));
    } else {
      //WCC Sets it to MPI_DATATYPE_NULL, but not free the struct, reuse it.
      MPI_Type_free(pbdr_mpi_dtype);
      // free(htype_);
    }

    if (type_nbytes % 8 == 0) {
      MPI_Type_contiguous(type_nbytes / sizeof(long), MPI_LONG, pbdr_mpi_dtype);
    } else if (type_nbytes % 4 == 0) {
      MPI_Type_contiguous(type_nbytes / sizeof(int), MPI_INT, pbdr_mpi_dtype);
    } else {
      MPI_Type_contiguous(type_nbytes, MPI_CHAR, pbdr_mpi_dtype);
    }
    MPI_Type_commit(pbdr_mpi_dtype);
    created_type_nbytes_ = type_nbytes;
    htype_ = pbdr_mpi_dtype;
  }
  if (prepare_fun != NULL) prepare_fun(prepare_arg);
  MPI_Allreduce(MPI_IN_PLACE, sendrecvbuf, count, *pbdr_mpi_dtype, *pbdr_mpi_op, MPI_COMM_WORLD);
}
}  // namespace engine
}  // namespace rabit
