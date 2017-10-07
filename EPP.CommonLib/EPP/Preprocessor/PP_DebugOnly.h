#pragma once

#ifdef _DEBUG
#define PP_DEBUG_ONLY(...) __VA_ARGS__
#else
#define PP_DEBUG_ONLY(...)
#endif