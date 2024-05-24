#include "flag1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "self_mod.h"
#include <dlfcn.h>


typedef void (*func)();

int main(int argc, char *argv[]) {
    struct flag_pieces flags;
    strcpy(flags.root_part1, "XXXXXXXX");
    strcpy(flags.root_part2, "YYYYYYYY");
    strcpy(flags.root_part3, "ZZZZZZZZ");
    strcpy(flags.root_part4, "QQQQQQQQ");

    printf("--------------------------------------------------\n");
    printf("Greetings! I am the self-modifying program. You see those strings directly above ");
    printf("in my source code? If you run me with an argument, I will find those strings in ");
    printf("my compiled file and overwrite them with the flags found at /root/root_part*.txt.\n");
    printf("\n");
    printf("That means that after I have modified myself, you just have to figure ");
    printf("out how to read those strings and the flags are yours.\n");
    printf("--------------------------------------------------\n");

    if (argc > 1) {
        replace_flags_in_bin(&flags);
    }

    struct Flag flag = create_struct_flag(flags.root_part1, flags.root_part2);


    void* library = dlopen("/usr/local/lib/libflag2.so", RTLD_LAZY);

    if (library == NULL) {
        fprintf(stderr, "The plugin library was not found.\n");
        exit(EXIT_FAILURE);
    }
    func func;
    *(void**)(&func) = dlsym(library, "copy_flags");
    if (func == NULL) {
        fprintf(stderr, "Couldn't find symbol in library.\n");
        dlclose(library);
        exit(EXIT_FAILURE);
    }

    char flag_part2[24];
    func(flags.root_part3, flags.root_part4, flag_part2);
    dlclose(library);
    return EXIT_SUCCESS;
}