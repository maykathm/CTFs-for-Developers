#include "flag1.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct Flag create_struct_flag(char* flag_string_start, char* flag_string_end) {
    if (!flag_ok(flag_string_start) || !flag_ok(flag_string_end)){
        fprintf(stderr, "The flags are not good!\n");
        exit(EXIT_FAILURE);
    }
    struct Flag flag;
    strcpy(&flag.string[0], flag_string_start);
    strcpy(&flag.string[8], flag_string_end);
    return flag;
}

bool flag_ok(const char* flag_piece) {
    return strlen(flag_piece) == 8;
}