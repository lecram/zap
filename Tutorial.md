# Introduction #

This tutorial is intended for programmers who want to learn the basics of zap.

You will need to [build zap](Build.md) from sources in order to run zap programs on your computer.

# Hello World #

A zap script is a source code file with instructions for execution by the zap interpreter. As a simple example, let's create a file called "hello.zp" containing the following text:

```
print("Hello, world!\n")
```

This script invokes the `print()` function, causing the interpreter to show a message on the console. In this example, the message is specified by a string literal. The characters `\n` at the end of the string indicate a line break.

## Running ##

To run this program we just need to pass the file "hello.zp" to the zap interpreter:

```
% zap hello.zp
Hello, world!
%
```