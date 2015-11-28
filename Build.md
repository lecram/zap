# Introduction #

The zap interpreter can easily be compiled from source. It takes a Subversion client and an ANSI C compiler. The GNU make tool can be used to facilitate the process, and is especially suitable if you intend to make changes and test the code.

# Getting the source #

The source files are kept in a Subversion (SVN) repository. To make anonymous check out of the repository, transferring files to your computer, you should use the following command:

```
% svn checkout http://zap.googlecode.com/svn/trunk/ zap
```

The files will be copied to the zap directory as specified.

# Compiling #

To compile the code, move to the subdirectory src:

```
% cd zap/src
```

The most convenient way to compile the files is invoking GNU make.

The Makefile in zap/src allows to compile the zap interpreter with only one command. In a Linux environment the command is usually as simple as possible:

```
% make
```

This will cause the standard compiler and shell (e.g. bash and GCC) to be used.

In Windows, some extra care is needed. You must specify the variables SHELL, CC and BINEXT.

SHELL determines the shell environment used and its typical value is "cmd".

CC determines the compiler used. If you are using MinGW GCC distribution, its typical value is "C:\MinGW\bin\gcc" (or just "gcc" if the directory "C:\MinGW\bin\" is listed in the %PATH% variable).

BINEXT determines the extension of the binary file generated and its typical value is ".exe".

To invoke GNU make on Windows and provide the appropriate arguments, you can use the typical command:

```
% make SHELL=cmd CC=gcc BINEXT=.exe
```

Or save the values of the arguments in a file named "env" (without extension), in the directory zap/src. The content of an typical "env" file is listed below.

```
SHELL = cmd
CC = gcc
BINEXT = .exe
RM = del
```

This file allows the zap interpreter to be built on Windows in a similar way that the procedure in the Linux environment, just by invoking GNU make without arguments. The variable RM will be used only for calls to "make clean".