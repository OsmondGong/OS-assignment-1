/* This file will contain your solution. Modify it as you wish. */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include "producerconsumer.h"

/* Declare any variables you need here to keep track of and
   synchronise your bounded buffer. A sample declaration of a buffer is shown
   below. It is an array of pointers to items.
   
   You can change this if you choose another implementation. 
   However, your implementation should accept at least BUFFER_SIZE 
   prior to blocking
*/
struct lock *lock1;
struct cv *full;
struct cv *empty;

#define BUFFLEN (BUFFER_SIZE + 1)

data_item_t * item_buffer[BUFFER_SIZE+1];



volatile int head, tail;


/* consumer_receive() is called by a consumer to request more data. It
   should block on a sync primitive if no data is available in your
   buffer. It should not busy wait! */

data_item_t * consumer_receive(void)
{
        data_item_t * item;

        lock_acquire(lock1);
        while(head == tail) {
                cv_wait(empty, lock1);
        }
        item = item_buffer[tail];
        tail = (tail + 1) % BUFFLEN;
        cv_signal(full, lock1);
        lock_release(lock1);

        return item;
}

/* procucer_send() is called by a producer to store data in your
   bounded buffer.  It should block on a sync primitive if no space is
   available in your buffer. It should not busy wait!*/

void producer_send(data_item_t *item)
{
        lock_acquire(lock1);
        while((head + 1) % BUFFLEN == tail) {
                cv_wait(full, lock1);
        }
        item_buffer[head] = item;
        head = (head + 1) % BUFFLEN;
        cv_signal(empty, lock1);
        lock_release(lock1);
}




/* Perform any initialisation (e.g. of global data) you need
   here. Note: You can panic if any allocation fails during setup */

void producerconsumer_startup(void)
{
        head = tail = 0;
        lock1 = lock_create("lock1");
        if (lock1 == NULL) {
                panic("I'm dead");
        }
        full = cv_create("full");
        if (full == NULL) {
                panic("I'm dead");
        }
        empty = cv_create("empty");
        if (empty == NULL) {
                panic("I'm dead");
        }

}

/* Perform any clean-up you need here */
void producerconsumer_shutdown(void)
{
        lock_destroy(lock1);
        cv_destroy(full);
        cv_destroy(empty);
}

