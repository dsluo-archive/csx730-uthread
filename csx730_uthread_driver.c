#include "csx730_uthread.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

struct func_args {
	uthread * wait;
	char * name;
	int count;
};

void func(uthread_arg arg) {
	struct func_args * args = arg;

	for(int i = 0; i < args->count; i++) {
	// for(int i = 0;; i++) {
		printf("thread %s: %d\n", args->name, i);
	}

	uthread * wait = args->wait;
	if (wait)
		uthread_join(wait);
	printf("thread %s done\n", args->name);
}


int main() {
	// setbuf(stdout, NULL);
	uthread thread1;
	uthread thread2;

	struct func_args func1_args = {&thread2, "1", 20000};
	struct func_args func2_args = {NULL, "2", 40000};

	uthread_create(&thread1, &func, &func1_args, UTHREAD_STACK_SIZE);
	uthread_create(&thread2, &func, &func2_args, UTHREAD_STACK_SIZE);

	// uthread_join(&thread1);
	// uthread_join(&thread2);
	// sleep(1);
	// for(;;);
	// raise(SIGPROF);

	uthread_join(&thread1);
	uthread_join(&thread2);
	printf("done\n");
} // main