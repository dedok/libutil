libutil
=======

[! WARNING !]

Currently is not fully consistent version, some parts of libutil may not linking or/and compiling.
I'll push soon version with fixes

[! WARNING !]

Description

    libutil - a lot of C utils which could be useful is any C/C++ project

Dependecy

    1) CMake >= 2.8.0 - build system.
    2) liblua.a >= 5.0.0 - devel version of lua.

Compiling under GNU Lunix and Mac OS X

    $ export PREFIX="/path/to/install/directory"
    $ ./bootstrap.sh [--debug|--rdebug|--release]
    $ make -C tmp-build [install]

