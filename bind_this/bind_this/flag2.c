#include "flag2.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void copy_flags(const char* flag_string_start, const char* flag_string_end, char *destination) {
    if (!flag_ok(flag_string_start) || !flag_ok(flag_string_end)){
        fprintf(stderr, "The flags are not good!\n");
        return;
    }
    strcpy(destination, flag_string_start);
    strcpy(&destination[strlen(flag_string_start)], flag_string_end);
}

bool flag_ok(const char* flag_piece) {
    return strlen(flag_piece) == 8;
}