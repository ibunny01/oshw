/* Author: Garrett Scholtes
 * Date:   2015-09-12
 *
 * Lab3 - CS4029
 * 
 * This module uses multiple synchronous threads
 * to increment some values in an array.
 * 
 * ---------------------------------------------------------------------------
 * 
 * >   All elements should have value 1 when the module is unloaded.
 * >   But the number of times idx is incremented may be 1000001.
 * >   Why is that?
 * 
 * To answer this question, consider a series of events scheduled to
 * occuring in this particular order:
 *          (Assume idx = 999999 before these events)
 *    1. ts1 sees that idx < 1000000, and enters while loop
 *    2. ts2 sees that idx < 1000000, and enters while loop
 *    3. ts1 interrogates semaphore state, acquires lock, and increments idx
 *          (notice that idx = 1000000 now)
 *    4. ts1 releases lock
 *    5. ts2 interrogates semaphore state, acquires lock, and increments idx
 * Oh my!  Step 5 causes idx = 1000001... and now we have modified memory
 * that we should not have access to.
 * 
 * The old code currently looks like this:
 *     while(idx < ARR_SIZE) {
 *         if(semaphore unocked) {
 *             ....
 *         }
 *     }
 * 
 * Moving the check would mitigate the extraneous increment:
 *     while(true) {
 *         if(semaphore unocked) {
 *             if(idx >= ARR_SIZE) {
 *                 break;
 *             }
 *             ....
 *         }
 *     }
 * 
 * idx is now protected by the semaphore during the check.
 * ---------------------------------------------------------------------------
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>

#define ARR_SIZE 1000000

static struct task_struct *t1;
static struct task_struct *t2;
static int arr[ARR_SIZE];
static int idx = 0;
static long id1 = 1;
static long id2 = 2;

static int stat[ARR_SIZE] = { 0 };
static int cs1 = 0;
static int cs2 = 0;

static struct semaphore lock;

MODULE_LICENSE("GPL");

int incrementer( void *ptr_void ) {
    long ptr;
    ptr = (long)ptr_void;

    printk(KERN_INFO "Consumer TID %ld\n", ptr);

    while(true) {
        // Acquire lock and perform work if semaphore is unlocked
        if(!down_interruptible(&lock)) {
            // idx also needs to be protected while performing
            // the check, so we place the check in here instead
            // of in the while loop conditional.
            if(idx >= ARR_SIZE) {
                break;
            }
            //Perform work
            arr[idx++] += 1;
            schedule();

            // Unlock semaphore
            up(&lock);

            // Do a stats test (described in part 1)
            if(ptr == 1) {
                cs1++;
            } else {
                cs2++;
            }
        }
    }
    return 0;
}

int init_module( void ) {
    printk("lab3 module installing\n");

    t1 = kthread_create(incrementer, (void*)id1, "inc1");
    t2 = kthread_create(incrementer, (void*)id2, "inc2");
    if(t1 && t2) {
        printk(KERN_INFO "Starting...\n");
        wake_up_process(t1);
        wake_up_process(t2);

        // Initialize semaphore as unlocked (1)
        sema_init(&lock, 1);
    } else {
        printk(KERN_EMERG "Error\n");
    }

    return 0;
}

void cleanup_module( void ) {
    int i;

    printk("lab3 module uninstalling\n");

    for(i = 0; i < ARR_SIZE; i++) {
        stat[arr[i]] += 1;
    }

    for(i = 0; i < ARR_SIZE; i++) {
        if(stat[i] > 0) {
            printk("[HIT] stat[%d] = %d\n", i, stat[i]);
        }
    }

    printk("cs1 =     %d\n", cs1);
    printk("cs2 =     %d\n", cs2);
    printk("cs1+cs2 = %d\n", cs1+cs2);

    printk("lab3 module uninstalled\n");
    return;
}