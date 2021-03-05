/* This file will contain your solution. Modify it as you wish. */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <kern/errno.h>
#include "client_server.h"

/*
 * Declare any variables you need here to implement and
 *  synchronise your queues and/or requests.
 */
#define QUEUELEN 51
request_t * queue[QUEUELEN];

volatile int head, tail;

struct lock *lock1;
struct cv *full;
struct cv *empty;

/* work_queue_enqueue():
 *
 * req: A pointer to a request to be processed. You can assume it is
 * a valid pointer or NULL. You can't assume anything about what it
 * points to, i.e. the internals of the request type.
 *
 * This function is expected to add requests to a single queue for
 * processing. The queue is a queue (FIFO). The function then returns
 * to the caller. It can be called concurrently by multiple threads.
 *
 * Note: The above is a high-level description of behaviour, not
 * detailed psuedo code. Depending on your implementation, more or
 * less code may be required. 
 */

   

void work_queue_enqueue(request_t *req)
{
        lock_acquire(lock1);
        while((head + 1) % QUEUELEN == tail) {
                cv_wait(full, lock1);
        }
        queue[head] = req;
        head = (head + 1) % QUEUELEN;
        cv_signal(empty, lock1);
        lock_release(lock1);
}

/* 
 * work_queue_get_next():
 *
 * This function is expected to block on a synchronisation primitive
 * until there are one or more requests in the queue for processing.
 *
 * A pointer to the request is removed from the queue and returned to
 * the server.
 * 
 * Note: The above is a high-level description of behaviour, not
 * detailed psuedo code. Depending on your implementation, more or
 * less code may be required.
 */


request_t *work_queue_get_next(void)
{
        request_t *item;
        lock_acquire(lock1);
        while(head == tail) {
                cv_wait(empty, lock1);
        }
        item = queue[tail];
        tail = (tail + 1) % QUEUELEN;
        cv_signal(full, lock1);
        lock_release(lock1);

        return item;
}




/*
 * work_queue_setup():
 * 
 * This function is called before the client and server threads are started. It is
 * intended for you to initialise any globals or synchronisation
 * primitives that are needed by your solution.
 *
 * In returns zero on success, or non-zero on failure.
 *
 * You can assume it is not called concurrently.
 */

int work_queue_setup(void)
{
        head = tail = 0;
        lock1 = lock_create("lock1");
        if (lock1 == NULL) {
                return ENOSYS;
        }
        full = cv_create("full");
        if (full == NULL) {
                panic("I'm dead");
        }
        empty = cv_create("empty");
        if (empty == NULL) {
                return ENOSYS;
        }
        return 0;

}


/* 
 * work_queue_shutdown():
 * 
 * This function is called after the participating threads have
 * exited. Use it to de-allocate or "destroy" anything allocated or created
 * on setup.
 *
 * You can assume it is not called concurrently.
 */

void work_queue_shutdown(void)
{
        lock_destroy(lock1);
        cv_destroy(full);
        cv_destroy(empty);
}
