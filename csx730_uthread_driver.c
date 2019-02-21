#include "csx730_uthread.h"
#include <unistd.h>
#include <stdio.h>


void func1(void * wait) {
	uthread * thread2 = wait;

	for(int i = 0; i < 100; i++) {
		sleep(1);
		printf("thread1: %d", i);
	}

	printf("waiting for thread2...");
	uthread_join(thread2);
}

void func2(void * arg) {
	for(int i = 0; i < 100; i++) {
		sleep(2);
		printf("thread2: %d", i);
	}
}


int main() {
	uthread thread1;
	uthread thread2;

	uthread_create(&thread1, &func1, NULL, UTHREAD_STACK_SIZE);
	uthread_create(&thread2, &func2, NULL, UTHREAD_STACK_SIZE);

	// uthread_join(&thread1);
	// uthread_join(&thread2);
	// sleep(1);
	for(;;);

	printf("done");
} // main
