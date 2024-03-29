# csx730-uthread

> The fibers of all things have their tension and are strained like the strings of an instrument.
> __--Henry David Thoreau__

**DUE FRI 2019-02-22 (Feb 22) @ 11:55 PM**

This repository contains the skeleton code for the `csx730-uthreads` project
assigned to the students in the Spring 2019 CSCI 4730/6730 class
at the University of Georgia.

## Updates

* __2019-02-06:__ Clarified the scheduler API requirements and updated the related prototypes and
  corresponding documentation.

## Academic Honesty

You agree to the Academic Honesty policy as outlined in the course syllabus. 
In accordance with this notice, I must caution you **not** to fork this repository
on GitHub if you have an account. Doing so will more than likely make your copy of
the project publicly visible. Please follow the instructions contained in the 
[How to Get the Skeleton Code](#how-to-get-the-skeleton-code) section below in
order to do your development on Nike. Furthermore, you must adhere to the copyright
notice and licensing information at the bottom of this document.

## User Space Threads

A thread of execution is the smallest sequence of programmed instructions that can be 
managed independently by a scheduler. A __user-mode thread__, sometimes referred to as a
_fiber_, is one that is scheduled in user mode instead of kernel mode. In user mode,
only a single thread can execute at a time. After some time, the current thread that
is executing will be temporarily interrupted by some signal, the disposition of which
should trigger a context switch to another thread without requiring either thread's 
cooperation. The basic idea is that these context switches should occur so quickly that
all threads appear to execute concurrently. Special care should be taken to block 
signals during a context switch. 

Each thread gets its own stack that is separate from the stack of the calling process
but somewhere within the process's virtual memory space. While this new stack space
can be allocated using `malloc(3)`, use of `mmap(2)` is recommended as it guarantees
the memory will be allocated at a nearby page boundary. You should 
[actively avoid](https://lwn.net/Articles/294001/) use of the `MAP_GROWSDOWN` 
flag when using `mmap(2)` for a stack. Instead simply treat the returned pointer to 
the mapped area as the end of the stack, then add the stack size to compute the 
initial stack pointer value.

In this project, you are tasked with implementing a preemptive multitasking, user-mode 
thread library in C and a little bit of x86 assembly! Some starter code is provided. 
Other project details are provided below.

## Useful References

* [X86 Opcode and Instruction Reference](http://ref.x86asm.net)
* [X86 64 Register and Instruction Quick Start](https://wiki.cdot.senecacollege.ca/wiki/X86_64_Register_and_Instruction_Quick_Start)
* [GCC: How to Use Inline Assembly Language in C Code](https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html)
* [C Typedef Declaration](https://en.cppreference.com/w/c/language/typedef)
* [`mmap(2)`](http://man7.org/linux/man-pages/man2/mmap.2.html)
* [`setitimer(2)`](http://man7.org/linux/man-pages/man2/getitimer.2.html)
* [`sigaction(2)`](http://man7.org/linux/man-pages/man2/sigaction.2.html)
* [`sigprocmask(2)`](http://man7.org/linux/man-pages/man2/sigprocmask.2.html)
* [`setjmp(3)`](http://man7.org/linux/man-pages/man3/setjmp.3.html)
* [`longjmp(3)`](http://man7.org/linux/man-pages/man3/longjmp.3.html)

## How to Change the Stack Pointer

You can easily move the stack pointer using 
[Extended ASM](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Extended-Asm) 
in GCC. Assuming that compiler optimizations are disabled, the following example 
should move the address stored in `rsptr` into the register for the stack pointer
before calling a function called `some_func`:

```c
void * rsptr =                // some address
__asm__("movq %0, %%rsp;"     // AssemblerTemplate
	:                     // OutputOperands
	: "r"(rsptr)          // InputOperands
	: "rsp");             // Clobbers
some_func();                  // function call on changed stack
```

Please note that in some cases it may be favorable to move the actual function
call into the `__asm__` block (or even an `__asm__ volatile` block), e.g., using
the `callq` instruction, in order to prevent the compiler from changing the
relative order of relevant instructions or from clobbering output operands 
in-between assembly blocks. If the function you intend to run on
the new stack requires arguments, then you may also need to manually move the 
values into the argument registers before changing the stack pointer. 

## How to Implement the Context Switch

Unless you want/need to fulfill the **[SETJMP]** requirement, you may directly
use `setjmp(3)` to save the excution context into a buffer and `longjmp(3)`
to restore the execution context. The documentation for these functions may be
hard to parse, but essentially `setjmp(3)` saves a copy of the neccesary CPU
registers into a buffer, then directly returns `0`. Later, when `longjmp(3)`
is executed with the buffer and some value, the registeres in the buffer get
restored, a (presumably) lon-local jump (e.g., the x86 `jumpq` instruction) is
performed and execution continues as if the corresponding call to `setjmp(3)`
has just returned wit the supplied value.

You may recall that the return value `fork(2)` can be used to determine if execution is occuring
in the parent process or the child process. Similarly, the return value of `setjmp(3)` can be used
to determine if the execution context has just been saved or restored. 

## Where can I store Extra Information about a Thread?

Each `uthread` has a member called `extra` that can point to any location the implementor
desires. A convenient way to keep extra information organized would be to use a `struct`.
A convenient place to store an object of this structure is it at the end of the thread's
allocated stack space. Since users of the library get to decide what each thread's stack
size is, it's recomended that you _increase_ the amount of memory allocated by the size of
your structure so that the user's desired stack size is honored.

## Planning it Out

Upon creation of the first user-mode thread, the scheduler should be start and the
thread added to its queue. This should also initiate an interval timer that sends a signal
to the process. An interval of about 5,000 microseconds should suffice. The rest of
the project boils down to careful _planning_. Drawing a state diagram before writing
any code will help.

You may find it useful to assign a thread for the calling process. This "main" thread does
not need its own stack since it's supposed to represent the main execution of the process
itself. This is just a suggestion--it's not a requirement, but it may simply some of your
scheduling logic simpler.

The interface for the functions in this project is intentionally similar to the one
provided by Pthreads. To test your project, you might first try some simple programs 
that use Pthreads, then try the same program using your project functions instead.
Don't try to overcomplicate it. We really just want to make sure that you can give the
illusion of parallelism via repeated user-mode context switches. 

## How to Get the Skeleton Code

On Nike, execute the following terminal command in order to download the project
files into a subdirectory within your present working directory:

```
$ git clone https://github.com/cs1730/csx730-uthread.git
```

This should create a directory called `csx730-uthread` in your present working directory that
contains the project files. For this project, the only files that are included with the project
download are listed near the top of the page [here](https://github.com/cs1730/csx730-uthread).

Here is a table that briely outlines each file in the skeleton code:

| File                      | Description                                                      |
|---------------------------|------------------------------------------------------------------|
| `Doxyfile`                | Configuration file for `doxygen`.                                |
| `Makefile`                | Configuration file for `make`.                                   |
| `README.md`               | This project description.                                        |
| `SUBMISSION.md`           | Student submission information.                                  |
| `csx730_uthread.c`        | Where you will put most of your thread implementation.           |
| `csx730_uthread.h`        | Thread structures, function prototypes, and macros.              |
| `setjmp/`                 | Subdirectory containing files for the **[SETJMP]** requirement.  |

If any updates to the project files are announced by your instructor, you can
merge those changes into your copy by changing into your project directory
on Nike and issuing the following terminal command:

```
$ git pull
```

If you have any problems with any of these procedures, then please contact
your instructor via Piazza. 

## Project Requirements

This assignment is worth 100 points. The lowest possible grade is 0, and the 
highest possible grade is 115 (if you are enrolled in CSCI 4730).

### Functional Requirements

A functional requirement is *added* to your point total if satisfied.
There will be no partial credit for any of the requirements that simply 
require the presence of a function related a particular functionality. 
The actual functionality is tested using test cases.

1. __(100 points) Implement `csx730_uthread.h` functions in `csx730_uthread.c`.__
   Each of the functions whose prototype appears in the header and does not require
   the `_CS6760_SOURCE` feature test macro must  be implemented correctly in the
   corresponding `.c` file. Here is a list of the functions forming the public API:

   * __(10 points)__ `void uthread_clear(uthread *);`
   * __(20 points)__ `int uthread_create(uthread *, uthread_func *, uthread_arg, size_t);`
   * __(20 points)__ `void uthread_exit(void);`
   * __(20 points)__ `void uthread_join(uthread *);`
   * __(10 points)__ `uthread * uthread_self(void);`

   Here is a list of functions forming the (private) scheduler API:
   
   * __(10 points)__ `void _uthread_sched_enqueue(uthread *);`
   * __(10 points)__ `uthread * _uthread_sched_dequeue(void);`

   The documentation for each function is provided directly in
   the header. You may generate an HTML version of the corresponding
   documentation using the `doc` target provided by the project's `Makefile`.
   Students should not modify the prototypes for these functions in any way--doing
   so will cause the tester used by the grader to fail.

   You are free,  _actively encouraged_, and will likely need to write other functions, as needed,
   to support the required set of functions. 

### 6730 Requirements

For the purposes of grading, a 6730 requirement is treated as a non-functional requirement
for students enrolled in CSCI 6730 and a functional requirement for students enrolled in
CSCI 4730. This effectively provides an extra credit opportunity to the undergraduate
students and a non-compliance penalty for the graduate students.

1. __(5 points) [PRIORITY] Implement `_CS6730_SOURCE` features__
   The `_CS6730_SOURCE` feature test macro should enable the following set of features:
   
   * Individual thread priority.
   * Priority queue scheduling.
   * The `uthread_create_priority` function.

   Additionally, `_CS6760_SOURCE` also changes the default behavior of the `uthread_create`
   function to default user-mode thread's priority to `UTHREAD_PRIORITY_NORMAL`.
   Students are expected to implement a priority queue using a max heap to satisfy the
   scheduling requirement.
   
1. __(10 points) [SETJMP] Implement the context switch without using `setjmp(3)` and `longjmp(3)`.__
   Without `setjmp(3)` and `longjmp(3)`, this task may seem daunting. Don't worry! You
   simply need to write the assembly to save and restore the CPU registers. While this could
   be done using an `__asm__` block, it's a lot cleaner to just write a dedicated `.s` file
   containing the necesary assembly. You will need to do the following for this requirement:

   * Move the files in the `setjmp` directory directly into the `csx730-uthread` directory.
   * Inspect and read `csx730_uthread_setjmp.h`.
   * Implement `_uthread_save` and `_uthread_restore` in `csx730_uthread_setjmp.s`.
   * In `csx730_uthread.c`, replace all instances of `setjmp(3)` with `_uthread_save` and
      all instances of `longjmp(3)` with `_uthread_restore`.
   * Use the alternative makefile:

     ```
     $ make -f Makefile-setjmp
     ```

   While you could combine C and assembly, as was seen in how to change the stack pointer, this
   requirement forces you to write the assembly directly in a `.s` file.   
   To simulate `setjmp(3)`, save the values of the relevant registers. 
   To simulate `longjmp(3)`, restore the register values, then return to the previously 
   saved environment by setting the stack pointer and manually returning. 

   At a minimum, the following 64 bit registers should be saved on an `x86` machine, as
   they are registers that called routines are expected to preserve:
   
   | Register | Description            |
   |----------|------------------------|
   | `rsp`    | register stack pointer |
   | `rbp`    | register base pointer  |
   | `rbx`    | register b extended    |
   | `r12`    | register 12            |
   | `r13`    | register 13            |
   | `r14`    | register 14            |
   | `r15`    | register 15            |

   This is not an exhaustive list! You may find that saving additional registers is needed.
   A `typedef struct` called `uthread_ctx` is provided with the starter code. You may add
   additional registers to the structure if you find it necesary. 

### Non-Functional Requirements

A non-functional requirement is *subtracted* from your point total if not satisfied. In order to
emphasize the importance of these requirements, non-compliance results in the full point amount
being subtracted from your point total. That is, they are all or nothing.

1. __(100 points) Build Compliance:__ Your project must compile on Nike
   using the provided `Makefile` and `gcc` (GCC) 8.2.0. Your code must compile,
   assemble, and link with no errors or warnings. The required compiler
   flags for this project are already included in the provided `Makefile`.

   The grader will compile and test your code using the `all` and `test` targets in
   the provided `Makefile`. The `test` target will not work until the test driver
   is provided during grading. If your code compiles, assembles, and links
   with no errors or warnings usign the `all` target, then it will very likely
   do the same with the `test` target.

1. __(100 points) Libraries:__ You are allowed to use any of the C standard library
   functions, unless they are explicitly forbidden below. A general reference for
   the C standard library is provided [here](https://en.cppreference.com/w/c).
   No other libraries are permitted, especially `pthreads`. You are also **NOT**
   allowed to use any of the following: 
   * `sigsetjmp(3)`, 
   * `siglongjmp(3)`,
   * `getcontext(2)`, 
   * `setcontext(2)`, 
   * `makecontext(3)`, and 
   * `swapcontext(3)`.

1. __(100 points) `SUBMISSION.md`:__ Your project must include a properly formatted 
   `SUBMISSION.md` file that includes, at a minimum, the following information:

   * __Author Information:__ You need to include your full name, UGA email, and
     which course you are enrolled in (e.g., CSCI 4730 or CSCI 6730).

   * __Implementation Overview:__ You need to include a few sentences that provide an overview
     of your implementation.
     
   * __6730 Requirements:__ You need to include a list of the 6730 requirements that you
     would like the grader to check. These requirements will only be checked if you
     include them in this list. 
     
   * __Reflecton:__ You need to provide answers to the following questions:

     1. What do you think was the motivation behind assigning this project in this class?
     1. What is the most important thing you learned in this project?
     1. What do you wish you had spent more time on or done differently?
     1. What part of the project did you do your best work on?
     1. What was the most enjoyable part of this project?
     1. What was the least enjoyable part of this project?
     1. How could your instructor change this project to make it better next time?

   A properly formatted sample `SUBMISSION.md` file is provided that may be modified to
   help fulfill this non-functional requirement.

1. __(25 points) Memory Leaks:__ Your submission should not result in any memory leaks.
   The grader will check this using `valgrind(1)`.

1. __(25 points) Code Documentation:__ Any new functions or macros must be properly documented
   using Javadoc-style comments. An example of such comments can be seen in the souce code
   provided with the project. Please also use inline documentation, as needed, to explain
   ambiguous or tricky parts of your code.

## Submission Instructions

You will still be submitting your project via Nike. Make sure your project files
are on `nike.cs.uga.edu`. Change into the parent directory of your
project directory. If you've followed the instructions provided in earlier in this
document, then the name of your project directory is likely `csx730-list`.
While in your project parent directory, execute the following command: 

```
$ submit csx730-uthread csx730
```

If you have any problems submitting your project then please make a private Piazza
post to "Instructors" as soon as possible. 

<hr/>

[![License: CC BY-NC-ND 4.0](https://img.shields.io/badge/License-CC%20BY--NC--ND%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-nc-nd/4.0/)

<small>
Copyright &copy; Michael E. Cotterell and the University of Georgia.
This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/">Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License</a> to students and the public.
The content and opinions expressed on this page do not necessarily reflect the views of nor are they endorsed by the University of Georgia or the University System of Georgia.
</small>

