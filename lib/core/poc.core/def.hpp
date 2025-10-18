#pragma once

// disable function coverage
#if defined(__clang__)
#define POC_COVERAGE_DISABLED __attribute__((no_profile_instrument_function))
#else
#define POC_COVERAGE_DISABLED
#endif

// indicate that a function must be tail call optimized
#if defined(__clang__) && __has_attribute(musttail)
#define POC_MUSTTAIL [[clang::musttail]]
#else
#define POC_MUSTTAIL
#endif

#if __has_cpp_attribute(no_unique_address)
#define POC_NO_UNIQUE_ADDRESS [[no_unique_address]]
#elif __has_cpp_attribute(msvc::no_unique_address)
#define POC_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define POC_NO_UNIQUE_ADDRESS
#endif
