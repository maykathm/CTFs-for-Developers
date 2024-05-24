#ifndef FLAG2_FLAG_H
#define FLAG2_FLAG_H

#include <stdbool.h>

void copy_flags(const char* flag_string_start, const char* flag_string_end, char *destination);

bool flag_ok(const char* flag_piece);

#endif