# buyer-seller

## Description
buyer\_seller is a project consisting of two programs, buyer and
seller. The buyer program is a client that reads a data file of 50000 entries,
consisting of an account number and order or payment, and sends it to it 
seller through the use of Unix Domain Sockets. Acting as a server, seller
receives these transactions, and through the use of multi-threading,
consolidates data from all 10 clients into a single account structure, avoiding
collisions.

## Installation
The entire project can be cloned from [Git](https://git.cybbh.space/170D/wobc/student-folders/23_001/williams/buyer-seller.git)

Developed with C18 and compiled with gcc-9.

---

## Build
`buyer_seller` can be built using the make tool.
```sh
<project_root>$ make
```
will produce to binaries, `client` and `server`.

---

## Run
### Server
`server` may be invoked with an optional argument `-p`:
```sh
<project_root>$ ./server
```
or
```sh
<project_root>$ ./server -p
```
The `-p` option flag will print the transactions per second of each client.

### Client
`client` must be invoked with a valid file argument and the `-p` option:
```sh
<project_root>$ ./client <FILE>
```
or
```sh
<project_root>$ ./client <FILE> -p
```
The `-p` option flag will print the transactions per second of the client.
**NOTE: `server` must be running before `client` is executed**

## Debug/Error Checking
To build `buyer_seller` with debug flags:
```sh
<project_root> $ make debug
```
to be able to debug with gdb.
```sh
<project_root> $ gdb ./server
<project_root> $ gdb ./client <FILE>
```
To check either binary against valgrind for memory leaks, build with debug flags
and run with `valgrind` prepended.</br>
**Example can be found in doc/testplan.pdf**

## Performance/Optimization
To be able to view performance information, build and execute with the following
profiling flags:
```sh 
<project_root> $ make profile
<project_root> $ ./client <FILE>
<project_root> $ ./server
```
After the program has been run, use `gprof` to view graph profile.
```sh
<project_root> $ gprof client gmon.out > doc/client_analysis.txt
<project_root> $ gprof server gmon.out > doc/server_analysis.txt
```
**NOTE: gprof will create gmon.out in the top level directory by default. However,
this command redirects the report to text format in `doc/`**

---

### Testing
Automated unit tests have been written for `buyer_seller` and can be run using
make.
```sh
<project_root> $ make check
```
will build and run automated unit testing.
```sh
Running suite(s): test_client_helper
100%: Checks: 3, Failures: 0, Errors: 0
test/test_client_helper.c:20:P:client_helper:*curr++:0: Passed
test/test_client_helper.c:27:P:client_helper:*curr++:0: Passed
test/test_client_helper.c:32:P:client_helper:*curr++:0: Passed
```
Additional information about `buyer_seller` testing can be found
[here](./doc/testplan.pdf)

---

### Cleanup
After running the program, execute:
```sh
<project_root> $ make clean
```
to delete object files as well as data created from `make profile`.

---

## Support
Additional support can be found in the man pages for the executables:
```sh
<project_root> $ man ./doc/server.1
<project_root> $ man ./doc/client.1
```
This README and project is a work in progress. For questions regarding the use
of the programs or to submit bugs/fixes, email: clayton.e.williams15.mil@army.mil
