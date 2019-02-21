#include "csx730_uthread.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

void func1(uthread_arg arg) {

	for(int i = 0; i < 10; i++) {
		sleep(1);
		printf("thread1: %d\n", i);
	}

}

void func2(uthread_arg arg) {
	for(int i = 0; i < 10; i++) {
		sleep(2);
		printf("thread2: %d\n", i);
	}
}


int main() {
	setbuf(stdout, NULL);
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