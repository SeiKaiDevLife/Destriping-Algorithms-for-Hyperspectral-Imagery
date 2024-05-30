//
// MATLAB Compiler: 8.4 (R2022a)
// Date: Tue Nov 22 11:08:15 2022
// Arguments:
// "-B""macro_default""-W""cpplib:m_Lemma""-T""link:lib""m_Lemma.m""-C"
//

#ifndef m_Lemma_h
#define m_Lemma_h 1

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
#ifndef LIB_m_Lemma_C_API 
#define LIB_m_Lemma_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_m_Lemma_C_API 
bool MW_CALL_CONV m_LemmaInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_m_Lemma_C_API 
bool MW_CALL_CONV m_LemmaInitialize(void);

extern LIB_m_Lemma_C_API 
void MW_CALL_CONV m_LemmaTerminate(void);

extern LIB_m_Lemma_C_API 
void MW_CALL_CONV m_LemmaPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_m_Lemma_C_API 
bool MW_CALL_CONV mlxM_Lemma(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_m_Lemma
#define PUBLIC_m_Lemma_CPP_API __declspec(dllexport)
#else
#define PUBLIC_m_Lemma_CPP_API __declspec(dllimport)
#endif

#define LIB_m_Lemma_CPP_API PUBLIC_m_Lemma_CPP_API

#else

#if !defined(LIB_m_Lemma_CPP_API)
#if defined(LIB_m_Lemma_C_API)
#define LIB_m_Lemma_CPP_API LIB_m_Lemma_C_API
#else
#define LIB_m_Lemma_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_m_Lemma_CPP_API void MW_CALL_CONV m_Lemma(int nargout, mwArray& Y, const mwArray& X, const mwArray& thr);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
