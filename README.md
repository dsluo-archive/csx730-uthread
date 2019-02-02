# csx730-uthreads

> The fibers of all things have their tension and are strained like the strings of an instrument.
> __--Henry David Thoreau__

**DUE MON 2019-02-04 (Feb 4) @ 11:55 PM**

This repository contains the skeleton code for the `csx730-uthreads` project
assigned to the students in the Spring 2019 CSCI 4730/6730 class
at the University of Georgia.

## Academic Honesty

You agree to the Academic Honesty policy as outlined in the course syllabus. 
In accordance with this notice, I must caution you **not** to fork this repository
on GitHub if you have an account. Doing so will more than likely make your copy of
the project publicly visible. Please follow the instructions contained in the 
[How to Get the Skeleton Code](#how-to-get-the-skeleton-code) section below in
order to do your development on Nike. Furthermore, you must adhere to the copyright
notice and licensing information at the bottom of this document.

## User Space Threads

MOTIVATE

In this project, you are tasked with implementing user space threads! The
structure for thread information is provided. Other project details are provided below.

## Suggested Reading

* [GCC: How to Use Inline Assembly Language in C Code](https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html)

## Useful References

* [X86 Opcode and Instruction Reference](http://ref.x86asm.net)

## How to Get the Skeleton Code

On Nike, execute the following terminal command in order to download the project
files into sub-directory within your present working directory:

```
$ git clone https://github.com/cs1730/csx730-uthreads.git
```

This should create a directory called `csx730-uthreads` in your present working directory that
contains the project files. For this project, the only files that are included with the project
download are listed near the top of the page [here](https://github.com/cs1730/csx730-uthreads).

Here is a table that briely outlines each file in the skeleton code:

| File                   | Description                                                      |
|------------------------|------------------------------------------------------------------|
| `Doxyfile`             | Configuration file for `doxygen`.                                |
| `Makefile`             | Configuration file for `make`.                                   |
| `README.md`            | This project description.                                        |
| `SUBMISSION.md`        | Student submission information.                                  |
| `csx730_uthreads.c`    | Where you will put most of your thread implementation.           |
| `csx730_uthreads.h`    | Thread structures, function prototypes, and macros.              |
| `csx730_list_driver.c` | Where the `main` method lives for the  __[DRIVER]__ requirement. |

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
highest possible grade is 110 (if you are enrolled in CSCI 4730).

### Functional Requirements

A functional requirement is *added* to your point total if satisfied.
There will be no partial credit for any of the requirements that simply 
require the presence of a function related a particular functionality. 
The actual functionality is tested using test cases.

1. __(?? points) Implement `csx730_uthreads.h` functions in `csx730_uthreads.c`.__
   Each of the functions whose prototype appears in the header  must
   be implemented correctly in the corresponding `.c` file.
   Here is a list of the functions:

   * __(?? points)__ `void foo(void)`
   * __(?? points)__ `void bar(void)`    

   The documentation for each function is provided directly in
   the header. You may generate an HTML version of the corresponding
   documentation using the `doc` target provided by the project's `Makefile`.
   Students should not modify the prototypes for these functions in any way--doing
   so will cause the tester used by the grader to fail. 

### 6730 Requirements

For the purposes of grading, a 6730 requirement is treated as a non-functional requirement
for students enrolled in CSCI 6730 and a functional requirement for students enrolled in
CSCI 4730. This effectively provides an extra credit opportunity to the undergraduate
students and a non-compliance penalty for the graduate students.

1. __(?? points) [REQNAME] Implement something...__
   Description here.

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
   functions. A reference is provided [here](https://en.cppreference.com/w/c).
   No other libraries are permitted, especially `pthreads`.

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
are on <code>nike.cs.uga.edu</code>. Change into the parent directory of your
project directory. If you've followed the instructions provided in earlier in this
document, then the name of your project directory is likely `csx730-list`.
While in your project parent directory, execute the following command: 

```
$ submit csx730-uthreads csx730
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

