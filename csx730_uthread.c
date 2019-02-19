#include "csx730_uthread.h"
#include <stddef.h>
#include <sys/mman.h>
#include <stdlib.h>


struct uthread_queue queue = {NULL, NULL};

// i think this works, but idk
void uthread_clear(uthread * thread) {
    free(thread);
    
}

int uthread_create(uthread * thread, uthread_func * func, uthread_arg arg, size_t stack_size) {
    void * stack = mmap(NULL, stack_size, PROT_NONE, MAP_ANONYMOUS, -1, 0);

    thread->stack.start = (char *) stack - stack_size;
    thread->stack.rsp = stack;
    thread->stack.size = stack_size;

    thread->state = NEW;
    enqueue(thread);

}

void uthread_exit(void);

void uthread_join(uthread * thread);
uthread * uthread_self(void);

void _uthread_sched_enqueue(uthread * thread) {
    struct uthread_node * node = (struct uthread_node *) malloc(sizeof(struct uthread_node));
    node->thread = thread;
    node->next = NULL;

    if (queue.head == NULL) {
        queue.head = queue.tail = node;
    } else {
        queue.tail->next = node;
        queue.tail = node;
    }
}

uthread * _uthread_sched_dequeue(void) {
    if (queue.head == NULL) {
        return NULL;
    } else {
        uthread * thread = queue.head->thread;
        struct uthread_node * old_head = queue.head;
        queue.head = queue.head->next;
        free(old_head);
        return thread;
    }
}