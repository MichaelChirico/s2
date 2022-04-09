
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include "wk-v1.h"
#include "s2-geography/constructor.hpp"
#include "geography-shim.h"


#define CPP_START                         \
    char cpp_exception_error[8096];       \
    memset(cpp_exception_error, 0, 8096); \
    try {

#define CPP_END                                           \
    } catch (std::exception& e) {                         \
        strncpy(cpp_exception_error, e.what(), 8096 - 1); \
    }                                                     \
    Rf_error("%s", cpp_exception_error);                  \
    return R_NilValue;


// The other versions of CPP_START and CPP_END stack-allocate the
// error message buffer, which takes a non-trivial amount of time
// when done at this scale (at worst 4 times per coordinate). By
// keeping the buffer in the handler_data struct, we can call C++
// from every handler method without measurable overhead.
#define WK_METHOD_CPP_START                     \
    try {

#define WK_METHOD_CPP_END                                 \
    } catch (std::exception& e) {                         \
        strncpy(data->cpp_exception_error, e.what(), 8096 - 1); \
    }                                                     \
    Rf_error("%s", data->cpp_exception_error);            \
    return R_NilValue;

#define WK_METHOD_CPP_END_INT                                 \
    } catch (std::exception& e) {                         \
        strncpy(data->cpp_exception_error, e.what(), 8096 - 1); \
    }                                                     \
    Rf_error("%s", data->cpp_exception_error);            \
    return WK_ABORT;


typedef struct {
    s2geography::VectorConstructor* builder;
    SEXP result;
    R_xlen_t feat_id;
    int coord_size;
    char cpp_exception_error[8096];
} builder_handler_t;


// TODO: Both of these allocate in a way that could longjmp and possibly leak memory
static inline void builder_result_append(builder_handler_t* data, SEXP value) {
    R_xlen_t current_size = Rf_xlength(data->result);
    if (data->feat_id >= current_size) {
        SEXP new_result = PROTECT(Rf_allocVector(VECSXP, current_size * 2 + 1));
        for (R_xlen_t i = 0; i < current_size; i++) {
            SET_VECTOR_ELT(new_result, i, VECTOR_ELT(data->result, i));
        }
        R_ReleaseObject(data->result);
        data->result = new_result;
        R_PreserveObject(data->result);
        UNPROTECT(1);
    }

    SET_VECTOR_ELT(data->result, data->feat_id, value);
    data->feat_id++;
}

static inline void builder_result_finalize(builder_handler_t* data) {
    R_xlen_t current_size = Rf_xlength(data->result);
    if (data->feat_id != current_size) {
        SEXP new_result = PROTECT(Rf_allocVector(VECSXP, data->feat_id));
        for (R_xlen_t i = 0; i < data->feat_id; i++) {
            SET_VECTOR_ELT(new_result, i, VECTOR_ELT(data->result, i));
        }
        R_ReleaseObject(data->result);
        data->result = new_result;
        R_PreserveObject(data->result);
        UNPROTECT(1);
    }
}

int builder_vector_start(const wk_vector_meta_t* meta, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;

  if (data->result != R_NilValue) {
      Rf_error("Destination vector was already allocated"); // # nocov
  }

  if (meta->size == WK_VECTOR_SIZE_UNKNOWN) {
      data->result = PROTECT(Rf_allocVector(VECSXP, 1024));
  } else {
      data->result = PROTECT(Rf_allocVector(VECSXP, meta->size));
  }
  R_PreserveObject(data->result);
  UNPROTECT(1);

  data->feat_id = 0;

  return WK_CONTINUE;
}

SEXP builder_vector_end(const wk_vector_meta_t* meta, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  builder_result_finalize(data);
  SEXP cls = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(cls, 0, Rf_mkChar("s2_geography"));
  SET_STRING_ELT(cls, 1, Rf_mkChar("s2_xptr"));
  Rf_setAttrib(data->result, R_ClassSymbol, cls);
  UNPROTECT(1);
  return data->result;
}

int builder_feature_start(const wk_vector_meta_t* meta, R_xlen_t feat_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START
  data->builder->start_feature();
  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_feature_null(void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START
  // Append NULL
  return WK_ABORT_FEATURE;
  WK_METHOD_CPP_END_INT
}

int builder_feature_end(const wk_vector_meta_t* meta, R_xlen_t feat_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START
  std::unique_ptr<s2geography::S2Geography> feat = data->builder->finish_feature();
  auto geog = MakeOldGeography(*feat);
  builder_result_append(data, Rcpp::XPtr<Geography>(geog.release()));
  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_geometry_start(const wk_meta_t* meta, uint32_t part_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START

  auto geometry_type = static_cast<s2geography::util::GeometryType>(meta->geometry_type);

  int32_t size;
  if (meta->size == WK_SIZE_UNKNOWN) {
      size = -1;
  } else {
      size = meta->size;
  }

  if (meta->flags & WK_FLAG_HAS_Z && meta->flags & WK_FLAG_HAS_M) {
    data->coord_size = 4;
  } else if (meta->flags & WK_FLAG_HAS_Z) {
    data->coord_size = 3;
  } else if (meta->flags & WK_FLAG_HAS_M) {
    data->coord_size = 3;
  } else {
    data->coord_size = 2;
  }

  data->builder->geom_start(geometry_type, size);
  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_geometry_end(const wk_meta_t* meta, uint32_t part_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START
  data->builder->geom_end();
  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_ring_start(const wk_meta_t* meta, uint32_t size, uint32_t ring_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START

  if (size == WK_SIZE_UNKNOWN) {
      data->builder->ring_start(-1);
  } else {
      data->builder->ring_start(size);
  }

  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_ring_end(const wk_meta_t* meta, uint32_t size, uint32_t ring_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START
  data->builder->ring_end();
  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_coord(const wk_meta_t* meta, const double* coord, uint32_t coord_id, void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  WK_METHOD_CPP_START
  data->builder->coords(coord, 1, data->coord_size);
  return WK_CONTINUE;
  WK_METHOD_CPP_END_INT
}

int builder_error(const char* message, void* handler_data) {
  Rf_error("%s", message);
  return WK_ABORT;
}

void builder_deinitialize(void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  if (data->result != R_NilValue) {
    R_ReleaseObject(data->result);
    data->result = R_NilValue;
  }
}

void builder_finalize(void* handler_data) {
  builder_handler_t* data = (builder_handler_t*) handler_data;
  if (data != nullptr) {
    free(data);
  }
}

void delete_vector_constructor(SEXP xptr) {
    auto ptr = reinterpret_cast<s2geography::VectorConstructor*>(R_ExternalPtrAddr(xptr));
    if (ptr != nullptr) {
        delete ptr;
    }
}

extern "C" SEXP c_s2_geography_writer_new(SEXP oriented_sexp, SEXP check_sexp) {
  CPP_START

  auto builder = new s2geography::VectorConstructor();
  SEXP builder_xptr = PROTECT(R_MakeExternalPtr(builder, R_NilValue, R_NilValue));
  R_RegisterCFinalizer(builder_xptr, &delete_vector_constructor);

  wk_handler_t* handler = wk_handler_create();

  handler->vector_start = &builder_vector_start;
  handler->vector_end = &builder_vector_end;

  handler->feature_start = &builder_feature_start;
  handler->null_feature = &builder_feature_null;
  handler->feature_end = &builder_feature_end;

  handler->geometry_start = &builder_geometry_start;
  handler->geometry_end = &builder_geometry_end;

  handler->ring_start = &builder_ring_start;
  handler->ring_end = &builder_ring_end;

  handler->coord = &builder_coord;

  handler->error = &builder_error;

  handler->deinitialize = &builder_deinitialize;
  handler->finalizer = &builder_finalize;

  builder_handler_t* data = (builder_handler_t*) malloc(sizeof(builder_handler_t));
  if (data == NULL) {
    wk_handler_destroy(handler); // # nocov
    Rf_error("Failed to alloc handler data"); // # nocov
  }

  data->coord_size = 2;
  data->builder = builder;
  data->result = R_NilValue;
  memset(data->cpp_exception_error, 0, 8096);

  handler->handler_data = data;

  // include the builder pointer as a tag for this external pointer
  // which guarnatees that it will not be garbage collected until
  // this object is garbage collected
  SEXP handler_xptr = wk_handler_create_xptr(handler, builder_xptr, R_NilValue);
  UNPROTECT(1);
  return handler_xptr;

  CPP_END
}
