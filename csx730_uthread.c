#define _GNU_SOURCE

#include "csx730_uthread.h"
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>

// http://www.evanjones.ca/software/threading.html
static struct {
    uthread * current;
    struct uthread_node * head;
    struct uthread_node * tail;
} uthread_scheduler;

struct uthread_node {
    uthread * thread;
    struct uthread_node * next;
};

struct uthread_extra {
    uthread_func * func;
    uthread_arg arg;
    jmp_buf * ctx;
};

void block_int(void) {
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigmask, NULL);
}

void unblock_int(void) {
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGPROF);
    sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
}

void interrupt(int signum) {
    if (signum != SIGPROF)
        return;
    
    block_int();
    uthread * thread = _uthread_sched_dequeue();
    if (thread == NULL) {
        unblock_int();
        return;
    }

    if (thread->state == NEW || thread->state == RUNNING || thread->state == WAITING) {
        jmp_buf ctx;
        if (!setjmp(ctx)) {
            // save current context
            ((struct uthread_extra *) uthread_scheduler.current->extra)->ctx = &ctx;
            _uthread_sched_enqueue(uthread_scheduler.current);

            uthread_scheduler.current = thread;
            if (thread->state == NEW) {
                // start thread
                thread->state = RUNNING;
                struct uthread_extra * extra = thread->extra;
                uthread_func * func = extra->func;
                uthread_arg arg = extra->arg;
                void * rsptr = thread->stack.rsp;
                unblock_int();
                __asm__("movq %0, %%rsp;"
                        :
                        : "r" (rsptr)
                        : "rsp");
                (*func)(arg);
                block_int();
                thread->state = DONE;
            } else {
                jmp_buf * new_ctx = ((struct uthread_extra *) thread->extra)->ctx;
                longjmp(*new_ctx, 1);
            }
        }
    } 
    unblock_int();
}

void uthread_clear(uthread * thread) {
    memset(thread, 0, sizeof(uthread));
}

int uthread_create(uthread * thread, uthread_func * func, uthread_arg arg, size_t stack_size) {
    if (thread == NULL || func == NULL)
        return EINVAL;
    if (stack_size < UTHREAD_MIN_STACK_SIZE)
        return ENOMEM;

    block_int();

    if (uthread_scheduler.current == NULL) {
        struct sigaction sa = {0};
        sa.sa_handler = &interrupt;
        sigaction(SIGPROF, &sa, NULL);

        uthread main_thread;
        uthread_clear(&main_thread);
        struct uthread_extra main_extra = {0};
        main_thread.extra = &main_extra;
        main_thread.state = RUNNING;
        uthread_scheduler.current = &main_thread;

        struct itimerval timer;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 5000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 5000;
        setitimer(ITIMER_PROF, &timer, NULL);
    }

    void * stack = mmap(NULL, stack_size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (stack == MAP_FAILED) {
        unblock_int();
        return EAGAIN;
    }

    struct uthread_extra * extra = malloc(sizeof(struct uthread_extra));
    extra->arg = arg;
    extra->func = func;

    uthread_clear(thread);
    thread->extra = extra;
    thread->state = NEW;
    thread->stack.start = stack;
    thread->stack.rsp = (char *) stack + stack_size;
    thread->stack.size = stack_size;

    _uthread_sched_enqueue(thread);

    unblock_int();
    return 0;
}

void uthread_exit(void) {
    uthread * current = uthread_self();
    current->state = DONE;
}

void uthread_join(uthread * thread) {
    uthread * current = uthread_self();
    current->state = WAITING;
    while (thread->state != DONE);
    current->state = RUNNING;
}

uthread * uthread_self(void) {
    return uthread_scheduler.current;
}

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
        uthread * thread = old_head->thread;
        uthread_scheduler.head = uthread_scheduler.head->next;
        if (uthread_scheduler.head == NULL) // old_head was the only thing in the queue
            uthread_scheduler.tail = NULL;
        free(old_head);
        return thread;
    }
}