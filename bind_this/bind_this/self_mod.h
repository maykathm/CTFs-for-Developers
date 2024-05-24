#ifndef SELF_MOD_H
#define SELF_MOD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FLAG_PIECE_LENGTH 8

struct flag_pieces {
    char root_part1[FLAG_PIECE_LENGTH+1];
    char root_part2[FLAG_PIECE_LENGTH+1];
    char root_part3[FLAG_PIECE_LENGTH+1];
    char root_part4[FLAG_PIECE_LENGTH+1];
};

struct malloc_info {
    char *ptr;
    size_t size;
};

void replace_flags_in_bin(const struct flag_pieces *flags);

void overwrite_flags_in_heap(const struct flag_pieces *flags, struct malloc_info alloc);

void overwrite_this_bin_with_heap(struct malloc_info alloc, const char *this_file_name);

void fill_this_filename(char *filename, size_t size);

void write_flag_to_contents(char* contents, const char* flagName);

size_t get_file_size(const char *filename);

struct malloc_info copy_file_to_heap(const char* filename);

void write_data_to_file(struct malloc_info alloc, const char *filename);

void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen);

void swap_flag(const char *old_flag, const char *new_flag, struct malloc_info alloc);


#endif