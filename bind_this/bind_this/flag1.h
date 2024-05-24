#ifndef FLAG1_FLAG_H
#define FLAG1_FLAG_H

#include <stdbool.h>

struct Flag {
    char string[24];
};

struct Flag create_struct_flag(char* flagStringStart, char* flagStringEnd);

bool flag_ok(const char* flag_piece);

#endif