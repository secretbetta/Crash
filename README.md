# Project 2: Command Line Shell

Author: Andrew Nguyen 

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html

## About This Project
A custom shell named *Crash* that has 4 built in commands: cd, history, jobs, and exit. Works exactly how a shell should work such as executing commands and more.

### What is a shell?
The shell is the command interpretor in an operating system such as Unix or GNU/Linux, it is a program that executes other programs. It provides a computer user an interface to the Unix/GNU Linux system so that the user can run different commands or utilities/tools with some input data.

### Running the program
```bash
>$ make 
>$ ./crash
```

### Included Files
There are several files included. These are:
   - <b>Makefile</b>: Including to compile and run the program.
   - <b>shell.c</b>: The program driver
   - <b>builtin.c</b>: Built in functions
   - <b>ui.c</b>: The user interface of the shell
   - <b>history.c</b>: Includes functions that will record, print, and show command history.
   - <b>util.c</b>: Includes helper functions to read in a file using system call read and also tokenize a string.


### Program Output
```bash
[🙂|1|anguyen27@anguyen27VM ~/Projects/P2-secretbetta]$ echo Hello World!
Hello World!
[🙂|2|anguyen27@anguyen27VM ~/Projects/P2-secretbetta]$ cd tests
[🙂|3|anguyen27@anguyen27VM ~/Projects/P2-secretbetta/tests]$ cd ..
[🙂|4|anguyen27@anguyen27VM ~/Projects/P2-secretbetta]$ history
 1 echo Hello World!
 2 cd tests
 3 cd ..
 4 history
[🙂|5|anguyen27@anguyen27VM ~/Projects/P2-secretbetta]$ #this is a comment :D
[🙂|6|anguyen27@anguyen27VM ~/Projects/P2-secretbetta]$ ./not a cmd
Execvp: No such file or directory
[😭|7|anguyen27@anguyen27VM ~/Projects/P2-secretbetta]$
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```
