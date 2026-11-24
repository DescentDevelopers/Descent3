/* Test shim: C runtime declarations for building the level1 test data as a Win32 DLL. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
double sin(double); double cos(double); double tan(double); double asin(double); double acos(double);
double atan(double); double atan2(double, double); double sqrt(double); double pow(double, double);
double exp(double); double log(double); double log10(double); double fabs(double); double floor(double);
double ceil(double); double fmod(double, double);
#ifdef __cplusplus
}
#endif
