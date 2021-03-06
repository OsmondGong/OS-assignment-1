#include "opt-synchprobs.h"
#include "counter.h"
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>


/*
 * Declare the counter variable that all threads increment or decrement
 * via the interface provided here.
 *
 * Declaring it "volatile" instructs the compiler to always (re)read the
 * variable from memory and not to optimise by keeping the value in a process 
 * register and avoid memory references.
 *
 * NOTE: The volatile declaration is actually not needed for the provided code 
 * as the variable is only loaded once in each function.
 */

static volatile int the_counter;

/*
 * ********************************************************************
 * INSERT ANY GLOBAL VARIABLES YOU REQUIRE HERE
 * ********************************************************************
 */
struct lock *lock1;

void counter_increment(void)
{
        lock_acquire(lock1);
        the_counter = the_counter + 1;
        lock_release(lock1);
}
void counter_decrement(void)
{
        lock_acquire(lock1);
        the_counter = the_counter - 1;
        lock_release(lock1);
}
int counter_initialise(int val)
{
        the_counter = val;
        lock1 = lock_create("lock1");
        if (lock1 == NULL) {
                return ENOMEM;
        }
        return 0;
}

int counter_read_and_destroy(void)
{
        lock_destroy(lock1);
        return the_counter;
}
