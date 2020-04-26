// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// s2BuildPolygonsLayer
List s2BuildPolygonsLayer(List ptrs, SEXP b_ptr);
RcppExport SEXP _libs2_s2BuildPolygonsLayer(SEXP ptrsSEXP, SEXP b_ptrSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< List >::type ptrs(ptrsSEXP);
    Rcpp::traits::input_parameter< SEXP >::type b_ptr(b_ptrSEXP);
    rcpp_result_gen = Rcpp::wrap(s2BuildPolygonsLayer(ptrs, b_ptr));
    return rcpp_result_gen;
END_RCPP
}
// s2MakePolygon
SEXP s2MakePolygon(List mat, bool oriented);
RcppExport SEXP _libs2_s2MakePolygon(SEXP matSEXP, SEXP orientedSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< List >::type mat(matSEXP);
    Rcpp::traits::input_parameter< bool >::type oriented(orientedSEXP);
    rcpp_result_gen = Rcpp::wrap(s2MakePolygon(mat, oriented));
    return rcpp_result_gen;
END_RCPP
}
// s2Intersects
List s2Intersects(List x, List y);
RcppExport SEXP _libs2_s2Intersects(SEXP xSEXP, SEXP ySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< List >::type x(xSEXP);
    Rcpp::traits::input_parameter< List >::type y(ySEXP);
    rcpp_result_gen = Rcpp::wrap(s2Intersects(x, y));
    return rcpp_result_gen;
END_RCPP
}
// s2ReleasePolygons
List s2ReleasePolygons(List ptrs);
RcppExport SEXP _libs2_s2ReleasePolygons(SEXP ptrsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< List >::type ptrs(ptrsSEXP);
    rcpp_result_gen = Rcpp::wrap(s2ReleasePolygons(ptrs));
    return rcpp_result_gen;
END_RCPP
}
// libs2_cpp_test_indexing
void libs2_cpp_test_indexing();
RcppExport SEXP _libs2_libs2_cpp_test_indexing() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    libs2_cpp_test_indexing();
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_libs2_s2BuildPolygonsLayer", (DL_FUNC) &_libs2_s2BuildPolygonsLayer, 2},
    {"_libs2_s2MakePolygon", (DL_FUNC) &_libs2_s2MakePolygon, 2},
    {"_libs2_s2Intersects", (DL_FUNC) &_libs2_s2Intersects, 2},
    {"_libs2_s2ReleasePolygons", (DL_FUNC) &_libs2_s2ReleasePolygons, 1},
    {"_libs2_libs2_cpp_test_indexing", (DL_FUNC) &_libs2_libs2_cpp_test_indexing, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_libs2(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
