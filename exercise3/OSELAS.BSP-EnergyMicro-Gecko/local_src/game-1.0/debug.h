// Include guard
#ifndef _DEBUG_H
#define _DEBUG_H

// Headers
#include <stdio.h>
#include <stdarg.h>

// This header provides debug functionality for the game module.
// NOTE: To turn on debugging, use #define DEBUG before including this header.

#ifdef DEBUG

  // Prints formatted output for debugging.
  // NOTE: Do not use directly, use the macros game_debug() and game_error() instead.
  void game_print(FILE* outstream, const char* fmt, ...);

  #define game_debug(...) game_print(stdout, __VA_ARGS__)
  #define game_error(...) game_print(stderr, __VA_ARGS__)
#else
  #define game_debug(...)
  #define game_error(...)
#endif

#endif // !_DEBUG_H
