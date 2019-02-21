#include "csx730_uthread.h"
#include <string.h>

static struct {
    uthread * current;
    struct uthread_node * head;
    struct uthread_node * tail;
} uthread_scheduler;

struct uthread_node {
    uthread * thread;
    struct uthread_node * next;
};

void uthread_clear(uthread * thread) {
    memset(thread, 0, sizeof(thread));
}

int uthread_create(uthread * thread, uthread_func * func, uthread_arg arg, size_t stack_size);

void uthread_exit(void);

void uthread_join(uthread * thread);

uthread * uthread_self(void);

void _uthread_sched_enqueue(uthread * thread) {
    struct uthread_node * node = malloc(sizeof(struct uthread_node));
    node->thread = thread;
    node->next = NULL;

    if (uthread_scheduler.tail == NULL) {
        uthread_scheduler.head = uthread_scheduler.tail = node;
    } else {
        uthread_scheduler.tail->next = node;
        uthread_scheduler.tail = node;
    }
}

uthread * _uthread_sched_dequeue(void) {
    if (uthread_scheduler.head == NULL) {
        return NULL;
    } else {
        struct uthread_node * old_head = uthread_scheduler.head;
        struct uthread * thread = old_head->thread;
        uthread_scheduler.head = uthread_scheduler.head->next;
        free(old_head);
        return thread;
    }
}