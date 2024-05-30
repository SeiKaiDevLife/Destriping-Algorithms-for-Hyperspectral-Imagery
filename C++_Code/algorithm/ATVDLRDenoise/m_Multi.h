//
// MATLAB Compiler: 8.4 (R2022a)
// Date: Tue Nov 22 10:17:45 2022
// Arguments:
// "-B""macro_default""-W""cpplib:m_Multi""-T""link:lib""m_Multi.m""-C"
//

#ifndef m_Multi_h
#define m_Multi_h 1

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
#ifndef LIB_m_Multi_C_API 
#define LIB_m_Multi_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_m_Multi_C_API 
bool MW_CALL_CONV m_MultiInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_m_Multi_C_API 
bool MW_CALL_CONV m_MultiInitialize(void);

extern LIB_m_Multi_C_API 
void MW_CALL_CONV m_MultiTerminate(void);

extern LIB_m_Multi_C_API 
void MW_CALL_CONV m_MultiPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_m_Multi_C_API 
bool MW_CALL_CONV mlxM_Multi(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_m_Multi
#define PUBLIC_m_Multi_CPP_API __declspec(dllexport)
#else
#define PUBLIC_m_Multi_CPP_API __declspec(dllimport)
#endif

#define LIB_m_Multi_CPP_API PUBLIC_m_Multi_CPP_API

#else

#if !defined(LIB_m_Multi_CPP_API)
#if defined(LIB_m_Multi_C_API)
#define LIB_m_Multi_CPP_API LIB_m_Multi_C_API
#else
#define LIB_m_Multi_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_m_Multi_CPP_API void MW_CALL_CONV m_Multi(int nargout, mwArray& Z, const mwArray& X, const mwArray& Y);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
