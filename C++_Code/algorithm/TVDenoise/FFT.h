//
// MATLAB Compiler: 8.4 (R2022a)
// Date: Wed Apr 19 14:12:12 2023
// Arguments: "-B""macro_default""-W""cpplib:FFT""-T""link:lib""FFT"
//

#ifndef FFT_h
#define FFT_h 1

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
#ifndef LIB_FFT_C_API 
#define LIB_FFT_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_FFT_C_API 
bool MW_CALL_CONV FFTInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_FFT_C_API 
bool MW_CALL_CONV FFTInitialize(void);

extern LIB_FFT_C_API 
void MW_CALL_CONV FFTTerminate(void);

extern LIB_FFT_C_API 
void MW_CALL_CONV FFTPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_FFT_C_API 
bool MW_CALL_CONV mlxFFT(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_FFT
#define PUBLIC_FFT_CPP_API __declspec(dllexport)
#else
#define PUBLIC_FFT_CPP_API __declspec(dllimport)
#endif

#define LIB_FFT_CPP_API PUBLIC_FFT_CPP_API

#else

#if !defined(LIB_FFT_CPP_API)
#if defined(LIB_FFT_C_API)
#define LIB_FFT_CPP_API LIB_FFT_C_API
#else
#define LIB_FFT_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_FFT_CPP_API void MW_CALL_CONV FFT(int nargout, mwArray& Xk, const mwArray& mu, const mwArray& lambda, const mwArray& f, const mwArray& dx, const mwArray& bx, const mwArray& dy, const mwArray& by);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
