#pragma once

#ifndef POC_ASSERTS_ENABLED
#define POC_ASSERTS_ENABLED 1
#endif

// asserts
#if POC_ASSERTS_ENABLED
#include <cassert>
#define POC_ASSERT(condition) assert(condition)
#define POC_ASSERT_CODE(...) __VA_ARGS__
#define POC_VERIFY(condition) POC_ASSERT(condition)
#else
#define POC_ASSERT(condition) ((void)0)
#define POC_ASSERT_CODE(...) ((void)0)
#define POC_VERIFY(condition) condition
#endif
