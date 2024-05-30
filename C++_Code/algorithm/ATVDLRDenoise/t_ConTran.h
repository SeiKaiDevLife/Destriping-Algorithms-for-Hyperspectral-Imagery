//
// MATLAB Compiler: 8.4 (R2022a)
// Date: Mon Nov 21 14:04:14 2022
// Arguments:
// "-B""macro_default""-W""cpplib:t_ConTran""-T""link:lib""t_ConTran.m""-C"
//

#ifndef t_ConTran_h
#define t_ConTran_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_t_ConTran_C_API 
#define LIB_t_ConTran_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_t_ConTran_C_API 
bool MW_CALL_CONV t_ConTranInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_t_ConTran_C_API 
bool MW_CALL_CONV t_ConTranInitialize(void);

extern LIB_t_ConTran_C_API 
void MW_CALL_CONV t_ConTranTerminate(void);

extern LIB_t_ConTran_C_API 
void MW_CALL_CONV t_ConTranPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_t_ConTran_C_API 
bool MW_CALL_CONV mlxT_ConTran(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_t_ConTran
#define PUBLIC_t_ConTran_CPP_API __declspec(dllexport)
#else
#define PUBLIC_t_ConTran_CPP_API __declspec(dllimport)
#endif

#define LIB_t_ConTran_CPP_API PUBLIC_t_ConTran_CPP_API

#else

#if !defined(LIB_t_ConTran_CPP_API)
#if defined(LIB_t_ConTran_C_API)
#define LIB_t_ConTran_CPP_API LIB_t_ConTran_C_API
#else
#define LIB_t_ConTran_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_t_ConTran_CPP_API void MW_CALL_CONV t_ConTran(int nargout, mwArray& Xt, const mwArray& X);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
