#include "debug.h"

void game_print(FILE* outstream, const char* fmt, ...)
{
  va_list ap;

  // print prefix
  fprintf(outstream, "game: ");

  // initialize va_list and call vfprintf to print the formatted message
  va_start(ap, fmt);
  vfprintf(outstream, fmt, ap);
  va_end(ap);
}
