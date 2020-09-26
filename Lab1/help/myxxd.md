# Objectives
  
  * Compile a C program with debug symbols and all warnings enabled
  * Write simple C programs with if-statements, while-statements, and multiple functions
  * Write formatted data to `stdout` with `printf`
  * Use `fread` to read bytes from a file
  * Convert ascii decimal values for characters to a string of ones and zeros representing it's binary encoding
  * Use `<`, `cat`, `echo`, and `|` in the terminal to provide data to `stdin` for a C program to read
  
# Project Summary

The goal of this lab is to write a program that mimics some of the functionality of the `xxd` command line tool that creates a hex or bits dump or its input (see `man xxd`). The part of `xxd` to mimic is its default behavior when reading from standard input that generates a hex dump of the input and the `-b` (`-bits`) flag that switches it to a bits (binary digits) dump of the input. 

The [myxxd.c](myxxd.c) file contains code to parse the command line options (`parseCommandLine`) and then call the appropriate output function for the hex dump (`readAndPrintInputAsHex`) or the bits dump (`readAndPrintInputAsBits`). The file also contains an implementation of `readAndPrintInputAsHex` to handle reading from `stdin` using the `fread` C library function. The implementation of `readAndPrintInputAsHex` calls two unimplemented functions `printDataAsHex` and `printDataAsChars` that produce the actual hex dump. Implementing these two functions is the starting point for the lab. Once these are implemented and tested, follow the same structure to implement `readAndPrintInputAsBits`: the function should be implemented using the same decomposition of the problem as in `readAndPrintInputAsHex` but differ in the number of bytes read at a time by `fread` and how those bytes are displayed in the output.  

# Where to start?

  1. Read all of the writeup.
  2. Play with `xxd` and I/O redirection as you read about them in the writeup.
  3. Study the starter code in [myxxd.c](myxxd.c) and correlate it with the writeup.
  4. Write a few simple test inputs to drive development (start small).
  5. Implement `void printDataAsHex(unsigned char* data, size_t size)` and test it.
  6. Implement `void printDataAsChars(unsigned char* data, size_t size)` and test it.
  7. Test a few bigger inputs and use diff to compare the output.
  8. Follow the pattern in `readAndPrintInputAsHex` to implement `void readAndPrintInputAsBits(FILE* input)`.

# Submission

Upload the final [myxxd.c](myxxd.c) file to [Canvas](http://canvas.byu.edu) for submission. A correct solution exactly matches the output from `xxd` with and without the `-b` flag an any input to `stdin`.

# Rubric

The TA's build the solution and test it against a set of input files by comparing the output from the program to that of `xxd` using `diff`.

  * `void printDataAsHex(unsigned char* data, size_t size)` (40 points)
  * `void printDataAsChars(unsigned char* data, size_t size)` (40 points)
  * Exact match with `xxd` default behavior on all input files (10 points)
  * `void readAndPrintInputAsBits(FILE* input)` (80 points)
  * Exact match with `xxd -b` on all input files (10 points)
  * Style (20 points)

# Getting the Starter Code

Copying [myxxd.c](myxxd.c) in the browser or downloading the code in the browser is one way to get the code, but if the work is being done remotely with `ssh`, then the file is on the wrong machine. The `scp` tool can copy the file to the remote machine, but there is an easier solutions using `git`.

The labs are in a `git` repository named [byu-cs-224-labs](https://bitbucket.org/byucs224/byu-cs-224-labs/src/master/). There is a button on the page named **Clone**, click that button, copy the command, and past it into the terminal. It will download the files directly to the remote machine. Here is the command to save time with following the link:

```
git clone https://bitbucket.org/byucs224/byu-cs-224-labs/src/master/
```

`git` is not part of this course. It is just a convenient way to distribute files. Once the files exist on the machine, nothing else is done with git until the next lab, in which case the command `git pull` in the directory will get the new files or the repository can be cloned again in a different location.

# Project Details

Carefully reading the project details is likely to save hours in completing the lab. Take time to understand **what** needs to be done, then plan out **how** to get it done, and finally do it. Each of the sections below is intended to answer the **what** and **how** for this project.

## Building and Running the Program

The program is build with the following command.

```
$ gcc -Wall -g myxxd.c -o myxxd
```

The `-Wall` tells `gcc` to issue warnings an anything that seems odd. The `-o` names the executable as `myxxd`. The `-g` includes debug information in the executable and is useful when using `gdb` (see *Debugging with gdb*).

Running the program is equally as easy.

```
$ ./myxxd
```

## Providing Input

*Standard input* is equivalent to the input stream `cin` in the C++ language. The C language equivalent is `stdin` and is defined in `stdio.h` (e.g., `#include <stdio.h>`). As in C++, reading from `stdin` reads typed characters at the keyboard. Keyboard input is easy; run `xxd` and then then type `Lab 0: myxxd` followed by the return. There should be stuff printed out on the return. Below is what all this looks like on the command line.

```
$ xxd
Lab 0: myxxd
00000000: 4c61 6220 303a 206d 7978 7864 0a         Lab 0: myxxd.
```

The program reads input until it is ended. To end the input hit return followed by `CTRL-d` (e.g., hold down the `control` key and while holding it type `d`). In some presentations `CTRL-d` is also indicated by `^d`. Both mean the same thing. A newline sometimes triggers output in many characters have been typed. The program is still reading input in that case and continues to read until the input is ended with `CTRL-d`.

Typing input at the keyboard for developing and debugging a program is both tedious and time consuming. The command line in the shell provides a few basic tools to automate (and simplify) input to `stdin` in the form of *I/O redirection* and *Pipes*. I/O redirection effectively changes where input to `stdin` comes from and output from a program goes to (see **Checking Output** below). 

A *pipe* is used to connect the output of one program to the input of another program. A pipe is created with the `|` symbol and requires a program that generates output. This lab uses two such programs: `echo` and `cat`. The `echo` program writes any specified operands to *standard output* (see `man echo`). Standard output is equivalent to `cout` in the C++ language. The C language equivalent is `stdout` and is defined in `stdio.h`. 

```
$ echo "Lab 0: myxxd"
Lab 0: myxxd
```

A pipe is used to redirect `stdout` from echo to `stdin` for `xxd`.

```
$ echo "Lab 0: myxxd" | xxd
00000000: 4c61 6220 303a 206d 7978 7864 0a         Lab 0: myxxd.
```

Now it is possible to type the input once as an operand to the `echo` command and pipe its output to the input of `xxd`. Using the up-arrow on the command line makes it easy to repeat the command over and over without having to type it again. The `echo` command with a pipe is great for simple one-line input for early stages of testing basic functionality. 

Use the `cat` command with the pipe or *input stream redirect* to generate bigger input. The `cat` command reads a file sequentially and writes it to `stdout` (see `man cat`). 

```
$ cat input.txt 
Lab 0: myxxd
```

The pipe connects the `stdout` from `cat` to the `stdin` for `xxd`.

```
$ cat input.txt | xxd
00000000: 4c61 6220 303a 206d 7978 7864 0a         Lab 0: myxxd.
```

Input stream redirect simplifies the process even further. It simply connects `stdin` to a file making input a snap. The `<` does the redirection.

```
$ xxd < input.txt 
00000000: 4c61 6220 303a 206d 7978 7864 0a         Lab 0: myxxd.
```

The program, `xxd`, is on the left of `<` and the file, `input.txt` is on the right.

`cat`, `echo`, `|`, and `<` give many options to providing input from the command line to `stdin` for the program. These combined with the up-arrow key should make testing the program efficient and convenient.

## Making Sense of the xxd Hex Dump

It is important to understand the `xxd` output in order to mimic it. It will be explained by example and connected to the provided starter file.

``` 
$ echo "What does this output mean?" | xxd
00000000: 5768 6174 2064 6f65 7320 7468 6973 206f  What does this o
00000010: 7574 7075 7420 6d65 616e 3f0a            utput mean?.
```

The output is best understood by breaking it into three sections: (1) file offset, (2) hex dump, and (3) character representation.

```
|  (1)  |                  (2)                   |      (3)       |
00000000: 5768 6174 2064 6f65 7320 7468 6973 206f  What does this o
```

The file offset (1) is the number of bytes from the beginning of the file where the data is located. In other words, it is the byte address in the file for the data that follows. The address itself is 32-bits in length or 4-bytes in total. The offset should be output as a 4-byte hexadecimal number. The `readAndPrintAsHex` function in the [myxxd.c](myxxd.c) starter code shows how to use `printf` to output the file offset: `printf("%08x:", offset);` where `08x` prints in as 8 hexadecimal characters. 

The hex dump (2) is the actual byte values in the file starting at the indicated offset. There are at most 16 of these on each line and they are grouped by pairs. In the example `57` is the byte displayed in hexadecimal at address `00000000`, `68` is the byte in hexadecimal at address `0000000001`, `61` is the byte in hexadecimal at address `00000002`, etc. The width of the hex dump as always 16-bytes with the extra spaces between the pairs plus an extra space on the end to separate it from section (3) the character representation. **This fixed width means that partial lines must be padded with extra space** as in the example.

``` 
00000010: 7574 7075 7420 6d65 616e 3f0a            utput mean?.
```

The hex dump section in the starter code is output by the function `void printDataAsHex(unsigned char* data, size_t size)`. The array `data` contains `size` number of bytes and `size` is never more than 16 but can be less than 16.

The character representation (3) is exactly that. The ASCII character for each byte. Characters are encoded by ASCII that assigns each character an integer value in the range of `0` and `127` inclusive (see `man ascii`). Printable characters start at the space ( value `32`) and end at the tilde (decimal value `126`). Any character that is not in the range of `32` and `126` inclusive is represented with a period (`.`). 

```
00000010: 7574 7075 7420 6d65 616e 3f0a            utput mean?.
```

The last character in the hex dump, `0a`, in the `nl` character for *newline*. It is outside the range with decimal value `10` and so it appears as a `.` in the character representation. Unlike the hex dump, no padding in required to fill partial lines in the character representation. 

The character representation section in the starter code is output by the function `void printDataAsChars(unsigned char* data, size_t size)`. The array `data` contains `size` number of bytes and `size` is never more than 16 but can be less than 16.

## Making Sense of the xxd Bits Dump

The bits dump (`-b` or `-bits`) is similar to the hex dump only the bytes are output in the binary representation of their ASCII value, section (2) only outputs six bytes rather than 16, and bytes are not paired.

```
$ echo "What does this output mean?" | xxd -bits
00000000: 01010111 01101000 01100001 01110100 00100000 01100100  What d
00000006: 01101111 01100101 01110011 00100000 01110100 01101000  oes th
0000000c: 01101001 01110011 00100000 01101111 01110101 01110100  is out
00000012: 01110000 01110101 01110100 00100000 01101101 01100101  put me
00000018: 01100001 01101110 00111111 00001010                    an?.
```

As before, padding is added to fill partially complete lines. Functionality for this section is to be implemented in the `void readAndPrintInputAsBits(FILE* input)` function following the pattern in its hex dump counterpart.

## Printing Bits

Implement the algorithm covered in class to convert an `unsigned char` to a string in bits. The pseudo-code is below and takes as input an `unsigned char` with name `x`.

```c
/* repeat 8 times */
if (x % 2 == 1) {
  /* store a 1 for the bit */
} else {
  /* store a 0 for the bit */
}
x = x / 2;
```

The algorithm generates the least significant bit first and the most significant bit last. Suppose that `x = 13`.

```
Iteration 0: x = 13 --> 1
Iteration 1: x = 6  --> 0
Iteration 2: x = 3  --> 1
Iteration 3: x = 1  --> 1
Iteration 4: x = 0  --> 0
Iteration 5: x = 0  --> 0
Iteration 6: x = 0  --> 0
Iteration 7: x = 0  --> 0
```

The output string should be `00001101`. Iteration `0` generated the least significant bit `1`, and iteration `7` generated the most significant bit `0`.

## Checking Output

There are two ways to check the output: visually and programatically. Visually comparing output is fine for small inputs but breakdowns for big (more complex) inputs. The `diff` is a programmatic solution in that it compares two files line by line and reports any differences (see `man diff`). Using `diff` though requires two files files to compare. 

*Output stream redirection* captures `stdout` and redirects it to a file. The redirect is with the '>' symbol. Here is an example of how it can be used with `diff` to compare two files.

```
$ cat myxxd.c | xxd > xxd.txt
$ cat myxxd.c | ./myxxd > myxxd.txt
$ diff myxxd.txt xxd.txt 
```

This same example can be accomplished with input stream redirect as below.

```
$ xxd < myxxd.c > xxd.txt
$ ./myxxd < myxxd.c > myxxd.txt
$ diff myxxd.txt xxd.txt 
```

As there is no output from the `diff` command, the two file exactly match. Here in an example where the there is a mismatch.

```
$ diff myxxd.txt xxd.txt
22c22
< 00000150: 6865 2069 6e74 6567 6572 2076 616c 7565  he integer value.a
---
> 00000150: 6865 2069 6e74 6567 6572 2076 616c 7565  he integer value
32c32
< 000001f0: 7273 6543 6f6d 6d61 6e64 4c69 6e65 2869  rseCommandLine(ix
---
> 000001f0: 7273 6543 6f6d 6d61 6e64 4c69 6e65 2869  rseCommandLine(i
```

## What to do with an infinite loop?

My favorite infinite loop is the one below.

```c
i = 0;
while (i < 10){
    printf("%i\n", i);
}
```

Here I forgot to increment `i` in the loop body, so for sure this program will not terminate.

The command line provides a very easy solution to a non-terminating program: `CTRL-c` (`^c`). Like `CTRL-d`, hold down the control key and this time type `c` while holding the key. After a brief pause the program will exit and give back the command line prompt. **Never kill the shell to fix an infinite loop.** Killing the shell leaves the program running and will seriously bog down the machine.

## I need a debugger!

The debugger is `gdb` and it is available from the command line if needed. This [primer](../gdb-primer.md) is a good starting point. There are many good online resources as well.