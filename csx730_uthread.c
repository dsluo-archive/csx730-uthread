#define _GNU_SOURCE

#include "csx730_uthread.h"
#include <stddef.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

// Scheduler ------------------------------------------------

static struct {
    uthread * current;
    struct uthread_node * head;
    struct uthread_node * tail;
    size_t count;
} uthread_scheduler;

struct uthread_node {
    uthread * thread;
    struct uthread_node * next;
};

void _uthread_sched_enqueue(uthread * thread) {
    struct uthread_node * node = malloc(sizeof(struct uthread_node));;
    node->thread = thread;
    node->next = NULL;

    if (uthread_scheduler.head == NULL) {
        uthread_scheduler.head = uthread_scheduler.tail = node;
    } else {
        uthread_scheduler.tail->next = node;
        uthread_scheduler.tail = node;
    }

    uthread_scheduler.count++;
}

uthread * _uthread_sched_dequeue(void) {
    if (uthread_scheduler.head == NULL) {
        return NULL;
    } else {
        uthread * thread = uthread_scheduler.head->thread;
        struct uthread_node * old_head = uthread_scheduler.head;
        uthread_scheduler.head = uthread_scheduler.head->next;
        free(old_head);
        uthread_scheduler.count--;
        return thread;
    }
}

// uthread

struct uthread_extra {
    uthread_func * func;
    uthread_arg arg;
    jmp_buf * ctx;
    uthread * wait_target;
};

void uthread_clear(uthread * thread) {
    memset(thread, 0, sizeof(uthread));
}

void interrupt(int signum) {
    if (signum != SIGPROF)
        return;
    
    uthread * thread = _uthread_sched_dequeue();
    if (thread == NULL)
        return;
    
    switch (thread->state)
    {
        case NEW:
        case WAITING:
        case RUNNING: {

            // Save current context.
            _uthread_sched_enqueue(uthread_scheduler.current);

            jmp_buf ctx;
            setjmp(ctx);

            struct uthread_extra * extra = uthread_scheduler.current->extra;
            extra->ctx = &ctx;

            uthread_scheduler.current = thread;
            break;
        }
        default: // DONE
            return;
    }

    switch (thread->state)
    {
        case NEW: {

            uthread_scheduler.current = thread;
            thread->state = RUNNING;
            struct uthread_extra * extra = thread->extra;
            uthread_func * func = extra->func;
            uthread_arg arg = extra->arg;
            void * rsptr = thread->stack.rsp;
            __asm__("movq %0, %%rsp;"
                    :
                    : "r" (rsptr)
                    : "rsp");
            (*func)(arg);
            thread->state = DONE;
            break;
        }
        case WAITING: {
            struct uthread_extra * extra = thread->extra;
            if (extra->wait_target->state == DONE)
                thread->state = RUNNING;
            break;
        }
        default:
            break;
    }

    // if (thread->state == NEW) {
    // } else if (thread->state == WAITING) {
    //     uthread_scheduler.current = thread;
    //     uthread_scheduler.current->state = RUNNING;
    //     struct uthread_extra * extra = uthread_scheduler.current->extra;
    //     longjmp(*extra->ctx, 0);
    // }
}

int uthread_create(uthread * thread, uthread_func * func, uthread_arg arg, size_t stack_size) {
    if (thread == NULL || func == NULL)
        return EINVAL;
    if (stack_size < UTHREAD_MIN_STACK_SIZE)
        return ENOMEM;

    // main thread, first thread
    if (uthread_scheduler.current == NULL) {
        // set signal handler for interrupts
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &interrupt;
        sigaction(SIGPROF, &sa, NULL);

        // setup uthread for main thread
        uthread main_thread;
        uthread_clear(&main_thread);
        main_thread.state = RUNNING;
        struct uthread_extra * main_extra = malloc(sizeof(struct uthread_extra));
        memset(main_extra, 0, sizeof(struct uthread_extra));
        main_thread.extra = main_extra;
        uthread_scheduler.current = &main_thread;

        // interrupt every 5000 us
        struct itimerval timer;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 5000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 5000;
        setitimer(ITIMER_PROF, &timer, NULL);
    }

    void * stack = mmap(NULL, stack_size, PROT_NONE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (stack == MAP_FAILED)
        return EAGAIN;
    
    uthread_clear(thread);
    thread->stack.start = (char *) stack + stack_size;
    thread->stack.rsp = stack;
    thread->stack.size = stack_size;

    thread->state = NEW;

    struct uthread_extra * extra = malloc(sizeof(struct uthread_extra));
    extra->func = func;
    extra->arg = arg;
    extra->ctx = NULL;
    extra->wait_target = NULL;

    thread->extra = extra;
    _uthread_sched_enqueue(thread);

    return 0;
}

void uthread_exit(void) {
    uthread * current = uthread_self();
    current->state = DONE;
}

void uthread_join(uthread * thread) {
    uthread * this_thread = uthread_self();
    this_thread->state = WAITING;
    struct uthread_extra * extra = this_thread->extra;
    extra->wait_target = thread;
}

uthread * uthread_self(void) {
    return uthread_scheduler.current;
}
