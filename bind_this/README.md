# CTFs for Developers: Bind This

## Objective: assemble the flag that has been fragmented into four files in `/root` as the user `binder`.

**NOTE: The Dockerfile doesn't contain any spoilers. I base64-ed the flags so that a casual glance won't give them away. Actually, checking out the Dockerfile might be a good way to orient oneself and see what's in the image.**


## How to build and run the environment

### From within the "CTFs-for-Developers/bind_this" directory...

First build the image:
```console
$ docker build -t bind_this -f bind_this.Dockerfile .
```
Then create and attach to a container (that will be destroyed upon exit thanks to the `--rm` flag):
```console
$ docker run --rm -ti bind_this
binder@83fbf6ad8938:/$
```

## Background information

### Installed libraries

Beyond some various dependencies installed from `apt` (you can always run a `apt list --installed` if you want to see exactly what or look in the Dockerfile), it also contains two dynamic libraries `libflag1.so` and `libflag2.so` and one executable `bind_this`, all of which are found in `/usr/local`.

You will also find in `/opt` a copy of the source code for the above-mentioned libraries and executable that one can also find in this directory under `bind_this`. 

### Working environment

When you attach to the box, you are the user `binder` and your objective is to get all the flag pieces found in `/root/root_part*.txt`.

## Hints

<details>
<summary>Hint 1</summary>

Dynamic analysis is sometimes faster than static. Try to just run the installed executable and see what happens. What is it doing? What sensitive data is it handling and how is it handling it? How can you get to that data?

</details>

<details>
<summary>Hint 2</summary>

`sudo` is installed. The command `sudo -l` will show you what the user can run as root.

Keep in mind that you can always destroy the container and make it anew.

</details>


</details>

<details>
<summary>Hint 3</summary>

One should familiarize oneself with how the dynamic linker works. How is it that Linux finds libraries to load? One can find some useful reading material by looking up the `ld.so` man pages. The dynamic linker also offers some very useful tools. Check out what the `LD_DEBUG` variable can do.

</details>


<details>
<summary>Hint 4</summary>

One should also familiarize oneself with symbol resolution. Check out this wonderful presentation from CppCon 2023: https://www.youtube.com/watch?v=_enXuIxuNV4 by Ofek Shilon called Linkers, Loaders, and Shared Libraries in Windows, Linux, and C++.

How can you use the symbol resolution process to leak out information?

</details>

## Solution Walkthrough

<details>
<summary>Show solution</summary>

## Setup

```console
user@machine bind_this % docker build -t bind_this -f bind_this.Dockerfile .

..<SNIP>..

user@machine bind_this % docker run --rm -ti bind_this                       
binder@83fbf6ad8938:/$
```

Personally, I'll use VS Code dev containers (and open up `/opt`) since that makes life so much easier.

## Code Footprinting `bind_this` executable

Running the program yields the following output:
```console
binder@83fbf6ad8938:/$ /usr/local/bin/bind_this 
--------------------------------------------------
Greetings! I am the self-modifying program. You see those strings directly above in my source code? If you run me with an argument, I will find those strings in my compiled file and overwrite them with the flags found at /root/root_part*.txt.

That means that after I have modified myself, you just have to figure out how to read those strings and the flags are yours.
--------------------------------------------------
binder@83fbf6ad8938:/$
```

Seems simple enough. The executable says it will embed the flag data in itself when run with an argument. Why not just try it?

```console
binder@83fbf6ad8938:/$ /usr/local/bin/bind_this modify

..<SNIP>..

Unable to open file of name /root/root_part1.txt due to error Permission denied.
binder@83fbf6ad8938:/$
```

Not surprising to get permission denied. Obviously the `binder` user doesn't have permission to read the flags otherwise `binder` could just `cat` the files and be done with it.

I'll take a quick peek at the source code just to see what's happening.

```c
int main(int argc, char *argv[]) {
    struct flag_pieces flags;
    strcpy(flags.root_part1, "XXXXXXXX");
    strcpy(flags.root_part2, "YYYYYYYY");
    strcpy(flags.root_part3, "ZZZZZZZZ");
    strcpy(flags.root_part4, "QQQQQQQQ");

    printf("--------------------------------------------------\n");
    printf("Greetings! I am the self-modifying program. You see those strings directly above ");

    ..<SNIP>..

    if (argc > 1) {
        replace_flags_in_bin(&flags);
    }

    ..<SNIP>..
```

I see where the flags will get replaced. It must be those Xs, Ys, Zs, and Qs. Judging from the name (and the fact that it only gets called if the executable was called with an argument), the function that does so must be `replace_flags_in_bin(&flags)`.

Obviously we know that compiled strings have to be present in the compiled binary. Just to check it out, I'll run strings.

```console
binder@83fbf6ad8938:/$ strings /usr/local/bin/bind_this | grep 'XX\|YY\|ZZ\|QQ'
XXXXXXXX
YYYYYYYYH
ZZZZZZZZH
QQQQQQQQH
binder@83fbf6ad8938:/$
```

I could verify it by looking at the `replace_flags_in_bin`, but I'm just going to assume the program will look through the binary file of itself and substitute those Xs, Ys, Zs, and Qs with the contents of the flags found under `/root`.

I need to run the program with elevated permissions otherwise I won't be able to get the executable to modify itself. I'll see if the `binder` user has any special permissions.

```console
binder@83fbf6ad8938:/$ sudo -l
Matching Defaults entries for binder on 6458cc3c3929:
    env_reset, mail_badpass, secure_path=/usr/local/sbin\:/usr/local/bin\:/usr/sbin\:/usr/bin\:/sbin\:/bin\:/snap/bin,
    use_pty

User binder may run the following commands on 6458cc3c3929:
    (ALL) NOPASSWD: /usr/local/bin/bind_this
binder@83fbf6ad8938:/$
```

Perfect. I can run the binary as root.

```console
binder@83fbf6ad8938:/$ sudo /usr/local/bin/bind_this modify
--------------------------------------------------
Greetings! I am the self-modifying program. You see those strings directly above in my source code? If you run me with an argument, I will find those strings in my compiled file and overwrite them with the flags found at /root/root_part*.txt.

That means that after I have modified myself, you just have to figure out how to read those strings and the flags are yours.
--------------------------------------------------
binder@83fbf6ad8938:/$
```

Assuming that worked, I'll just run `strings` and get the flags.

```console
binder@83fbf6ad8938:/$ strings /usr/local/bin/bind_this 
strings: /usr/local/bin/bind_this: Permission denied
binder@83fbf6ad8938:/$
```

I guess in overwriting the binary, the program changed the permissions to keep me from just looking at the file contents.

```console
binder@83fbf6ad8938:/$ ll /usr/local/bin/
total 36
drwxr-xr-x 1 root root  4096 May 24 16:31 ./
drwxr-xr-x 1 root root  4096 Apr 27 02:02 ../
-rwx--x--x 1 root root 21752 May 24 16:31 bind_this*
binder@83fbf6ad8938:/$
```

Yes, now I can only execute the file. I can no longer read it. Time to look at the source code a bit better.

The line of code after the self-modifying bit shows passing two parts of the flag to a function.

```c
struct Flag flag = create_struct_flag(flags.root_part1, flags.root_part2);
```

That function is defined in files `flag1.{h, c}`, and it looks like that file then becomes the `libflag1.so` library.

### Footprinting `libflag1.so`

I'll see how that library is built in the `CMakeLists.txt` file.
```cmake
add_library(flag1 SHARED flag1.c)
target_link_options(flag1 PRIVATE "-Wl,-Bsymbolic")
target_link_libraries(${PROJECT_NAME} flag1)
```

It has a linking flag `-Bsymbolic`. From a quick internet search, I see it means the library will favor its own symbols during the binding process. That means I won't be able to use Linux's interposition feature to inject my own code. However, the binary links dynamically to that library. I want to confirm by running an `ldd` but I no longer have permission to view that file. 

#### Time to throw away my container and make it again. I'll just have to remember that once I figure everything out I'll have to have it modify itself again.

... time passes and a container is destroyed and a new, pristine one takes its place...

```console
binder@83fbf6ad8938:/$ ldd /usr/local/bin/bind_this 
	linux-vdso.so.1 (0x00007fff06bea000)
	libflag1.so => /usr/local/lib/libflag1.so (0x00007f1439deb000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f1439bc2000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f1439dfb000)
binder@83fbf6ad8938:/$
```

The `libflag1.so` library is installed under `/usr/local/lib`. I imagine there's no way I'll be able to place my own version of that library in a folder that precedes `/usr/local` in the lookup order. I'll verify though.

First I'll check the library lookup folders.

```console
ibinder@83fbf6ad8938:/$ cat /etc/ld.so.conf.d/*.conf | grep -v '^#'
/usr/lib/x86_64-linux-gnu/libfakeroot
/usr/local/lib
/usr/local/lib/x86_64-linux-gnu
/lib/x86_64-linux-gnu
/usr/lib/x86_64-linux-gnu
binder@83fbf6ad8938:/$
```

I'll check to see if I have permission to write in any of those folders.

```console
binder@83fbf6ad8938:/$ cat /etc/ld.so.conf.d/*.conf | grep -v '^#' | while read line; do ll $(dirname $line) | grep $(basename $line); done
drwxr-xr-x  2 root root     4096 May 24 16:03 libfakeroot/
drwxr-xr-x 1 root root 4096 May 24 16:06 lib/
drwxr-xr-x  1 root root 12288 May 24 16:03 x86_64-linux-gnu/
binder@83fbf6ad8938:/$
```

I can't write to any of those folders so there's no way I can install my library in a folder higher up on the search path than `/usr/local/lib`. I can actually see how the executable searches for the library by using `LD_DEBUG`.

```console
binder@83fbf6ad8938:/$ LD_DEBUG=libs /usr/local/bin/bind_this 
       194:	find library=libflag1.so [0]; searching
       194:	 search cache=/etc/ld.so.cache
       194:	  trying file=/usr/local/lib/libflag1.so

..<SNIP>..
```

The correct path is the very first one it tries anyway. So that's a dead end. At this point, either one already has a certain familiarity with the dynamic linker or one starts looking at documentation with a nice `man ld.so`. After a pretty quick read about how the dynamic linker chooses its libraries, I identify a tool that sounds perfect, `LD_PRELOAD`. From the man pages under `LD_PRELOAD` it says, "A list of additional, user-specified, ELF shared objects to be loaded before all others." That's exactly what we're looking for. I'll do a quick test to see.

First I'll compile the library as-is under `/opt/bind_this`.

```console
binder@83fbf6ad8938:/$ mkdir /opt/bind_this/build
binder@83fbf6ad8938:/$ cmake -B /opt/bind_this/build -S /opt/bind_this
-- Configuring done
-- Generating done
-- Build files have been written to: /opt/bind_this/build
binder@83fbf6ad8938:/$ cmake --build /opt/bind_this/build/
[ 14%] Building C object CMakeFiles/flag1.dir/flag1.c.o
[ 28%] Linking C shared library libflag1.so
[ 28%] Built target flag1
[ 42%] Building C object CMakeFiles/bind_this.dir/main.c.o
[ 57%] Building C object CMakeFiles/bind_this.dir/self_mod.c.o
[ 71%] Linking C executable bind_this
[ 71%] Built target bind_this
[ 85%] Building C object CMakeFiles/flag2.dir/flag2.c.o
[100%] Linking C shared library libflag2.so
[100%] Built target flag2
```

Now time to test out if by using the `LD_PRELOAD` flag I can have the executable call my version of the library instead of the one installed in `/usr/local`.

```console
binder@83fbf6ad8938:/opt/bind_this/build$ LD_DEBUG=libs LD_PRELOAD=/opt/bind_this/build/libflag1.so /usr/local/bin/bind_this 
       281:	find library=libc.so.6 [0]; searching
       281:	 search cache=/etc/ld.so.cache
       281:	  trying file=/lib/x86_64-linux-gnu/libc.so.6
       281:	
       281:	
       281:	calling init: /lib64/ld-linux-x86-64.so.2
       281:	
       281:	
       281:	calling init: /lib/x86_64-linux-gnu/libc.so.6
       281:	
       281:	
       281:	calling init: /opt/bind_this/build/libflag1.so
```

It looks like it'll work. So I can get the first two fragments of the flag. Time to figure out how to get the last two.

### Footprinting `libflag2.so`

In looking at the `main.c` file, I see that the `libflag2.so` library gets loaded with an absolute address using `dlopen` and then a `copy_flags` function gets called where the last two fragments of the flag get passed.

```c
void* library = dlopen("/usr/local/lib/libflag2.so", RTLD_LAZY);

..<SNIP>..

*(void**)(&func) = dlsym(library, "copy_flags");

..<SNIP>..

char flag_part2[24];
func(flags.root_part3, flags.root_part4, flag_part2);

..<SNIP>..
```

This time, the same trick I used won't work. The `dlsym` gets called on that loaded library from an absolute path. I'll take a look at that function, `copy_flags`.

```c
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
```
The `copy_flags` function calls another function inside the same library: `flag_ok`. Looking at the `CMakeLists.txt` file, I don't see the same anti-interposition flag as with the first library. That means that all I need to do is ensure that I have my own library with an identical symbol loaded and it should call my function. 

>NOTE: If you are unclear how interposition works, I think Ofek Shilon in CppCon 2023 did a fantastic job with his presentation Linkers, Loaders, and Shared Libraries in Windows, Linux, and C++.

Once again I'll do a quick test to make sure it'll work. Just to confirm my analysis, I'll search for both symbols to see that the `copy_flags` symbol will resolve to the library indicated in the executable while the `flag_ok` symbol will resolve to the library I control.

```console
binder@83fbf6ad8938:/$ LD_DEBUG=bindings LD_PRELOAD=/opt/bind_this/build/libflag2.so /usr/local/bin/bind_this 2>&1 | grep 'flag_ok\|copy_flags'
       309:	binding file /usr/local/lib/libflag2.so [0] to /usr/local/lib/libflag2.so [0]: normal symbol `copy_flags'
       309:	binding file /usr/local/lib/libflag2.so [0] to /opt/bind_this/build/libflag2.so [0]: normal symbol flag_ok'
```

Confirmed. All I need to do is throw in some `printf`s and I should have the flags.

## Flag Retrieval

### Step 1: Have the binary modify itself and retrieve the flags

```console
binder@83fbf6ad8938:/$ sudo /usr/local/bin/bind_this modify

..<SNIP>..

binder@83fbf6ad8938:/$
```

### Step 2: Get the first half of flag

I add a `printf` to the `flag1.c` file in the `create_struct_flag` function.

```c
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
    printf("%s\n", flag.string);
    return flag;
}
```
I compile the code.

```console
binder@83fbf6ad8938:/$ cmake --build /opt/bind_this/build/
[ 14%] Building C object CMakeFiles/flag1.dir/flag1.c.o
[ 28%] Linking C shared library libflag1.so
[ 28%] Built target flag1
[ 42%] Linking C executable bind_this
[ 71%] Built target bind_this
[100%] Built target flag2
```

I run the binary (without sudo permissions because otherwise `LD_PRELOAD` won't get used) with the customized library in preload.

```console
binder@83fbf6ad8938:/$ LD_PRELOAD=/opt/bind_this/build/libflag1.so /usr/local/bin/bind_this 

..<SNIP>..

devCTF{1nt3rp0zi
binder@83fbf6ad8938:/$
```

There's the first half of the flag.

### Step 3: Get the second half of the flag

First I modify the `flag2.c` file so that the `flag_ok` method will print out the flag.

```c
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
    printf("%s\n", flag_piece);
    return strlen(flag_piece) == 8;
}
```

I'll compile.

```console
binder@83fbf6ad8938:/$ cmake --build /opt/bind_this/build/
Consolidate compiler generated dependencies of target flag1
[ 28%] Built target flag1
[ 71%] Built target bind_this
[ 85%] Building C object CMakeFiles/flag2.dir/flag2.c.o
[100%] Linking C shared library libflag2.so
[100%] Built target flag2
binder@83fbf6ad8938:/$
```

Then I'll add it to the preload and print out the flag.

```console
binder@83fbf6ad8938:/$ LD_PRELOAD=/opt/bind_this/build/libflag2.so /usr/local/bin/bind_this 

..<SNIP>..

tion_c4n
_b_w1ld}
```

Put all the pieces together: `devCTF{1nt3rp0zition_c4n_b_w1ld}`

</details>
