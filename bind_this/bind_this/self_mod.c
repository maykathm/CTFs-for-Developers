#include "self_mod.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void replace_flags_in_bin(const struct flag_pieces *flags) {
    char filename[1024];
    fill_this_filename(filename, sizeof(filename));
    struct malloc_info alloc = copy_file_to_heap(filename);
    overwrite_flags_in_heap(flags, alloc);
    overwrite_this_bin_with_heap(alloc, filename);
}

void overwrite_flags_in_heap(const struct flag_pieces *flags, struct malloc_info alloc) {
    char flag_replacement[8];
    write_flag_to_contents(flag_replacement, "/root/root_part1.txt");
    swap_flag(flags->root_part1, flag_replacement, alloc);
    write_flag_to_contents(flag_replacement, "/root/root_part2.txt");
    swap_flag(flags->root_part2, flag_replacement, alloc);
    write_flag_to_contents(flag_replacement, "/root/root_part3.txt");
    swap_flag(flags->root_part3, flag_replacement, alloc);
    write_flag_to_contents(flag_replacement, "/root/root_part4.txt");
    swap_flag(flags->root_part4, flag_replacement, alloc);
}

void overwrite_this_bin_with_heap(struct malloc_info alloc, const char *this_file_name) {
    int s = strlen(this_file_name);
    char filename_bis[] = "/tmp/bis";
    write_data_to_file(alloc, filename_bis);
    free(alloc.ptr);
    char mode[] = "0711";
    int mode_i = strtol(mode, 0, 8);
    if (chmod(filename_bis, mode_i) < 0) {
        int e = errno;
        fprintf(stderr, "Error encountered in changing permissions for executable %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int result = rename(filename_bis, this_file_name);
    if (result == -1) {
        int e = errno;
        fprintf(stderr, "An error occured in substituting this executable: %s.\n", strerror(e));
        exit(EXIT_FAILURE);
    }
}

void fill_this_filename(char *filename, size_t size) {
    int len_path = readlink("/proc/self/exe", filename, size-1);
    if(len_path == -1) {
        int e = errno;
        fprintf(stderr, "Couldn't get file path of this executable due to error %s.\n", strerror(e));
        exit(EXIT_FAILURE);
    }
    filename[len_path] = '\x00';
}

void write_flag_to_contents(char* contents, const char* flagName) {
    FILE *readF;
    readF = fopen(flagName, "rb");
    if (readF == NULL) {
        int e = errno;
        fprintf(stderr, "Unable to open file of name %s due to error %s.\n", flagName, strerror(e));
        exit(EXIT_FAILURE);
    }
    unsigned char buffer[FLAG_PIECE_LENGTH];
    int numRead = fread(buffer,sizeof(buffer),1,readF);
    if (numRead != 1) {
        int e = errno;
        fprintf(stderr, "Unable to read file contents of file %s due to error %s.\n", flagName, strerror(e));
        exit(EXIT_FAILURE);
    }
    fclose(readF);
    strncpy(contents, buffer, FLAG_PIECE_LENGTH);
}

size_t get_file_size(const char *filename) {
    struct stat info;
    if (stat(filename, &info) != 0) {
        int e = errno;
        fprintf(stderr, "Unable to stat file %s due to error %s\n", filename, strerror(e));
        exit(EXIT_FAILURE);
    }   
    return info.st_size;
}

struct malloc_info copy_file_to_heap(const char* filename) {
    struct malloc_info alloc;
    alloc.size = get_file_size(filename);
    alloc.ptr = malloc(alloc.size);
    if (alloc.ptr == NULL) {
        fprintf(stderr, "Unable to perform malloc for file size of %lu\n", alloc.size);
        exit(EXIT_FAILURE);
    }   
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        int e = errno;
        fprintf(stderr, "Unable to open file of name %s due to error %s.\n", filename, strerror(e));
        free(alloc.ptr);
        exit(EXIT_FAILURE);
    }
    size_t blocks_read = fread(alloc.ptr, alloc.size, 1, fp);
    if (blocks_read != 1) {
        free(alloc.ptr);
        fprintf(stderr, "Could not read file into heap.\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    return alloc;
}

void write_data_to_file(struct malloc_info alloc, const char *filename) {
    FILE *writeF;
    writeF = fopen(filename, "wb");
    if (writeF == NULL) {
        int e = errno;
        fprintf(stderr, "Unable to open file of name %s due to error %s.\n", filename, strerror(e));
        exit(EXIT_FAILURE);
    }
    size_t blocks_written = fwrite(alloc.ptr, alloc.size, 1, writeF);
    if (blocks_written != 1) {
        fprintf(stderr, "Could not write file.\n");
        exit(EXIT_FAILURE);
    }
    fclose(writeF);
}

// https://stackoverflow.com/questions/2188914/c-searching-for-a-string-in-a-file
void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen) {
    int needle_first;
    const void *p = haystack;
    size_t plen = hlen;
    if (!nlen)
        return NULL;
    needle_first = *(unsigned char *)needle;
    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1))) {
        if (!memcmp(p, needle, nlen))
            return (void *)p;
        p++;
        plen = hlen - (p - haystack);
    }
    return NULL;
}

void swap_flag(const char *old_flag, const char *new_flag, struct malloc_info alloc) {
    void *pos = memmem(alloc.ptr, alloc.size, old_flag, FLAG_PIECE_LENGTH);
    if (pos != NULL) {
        memcpy(pos, new_flag, FLAG_PIECE_LENGTH);
    }
}
