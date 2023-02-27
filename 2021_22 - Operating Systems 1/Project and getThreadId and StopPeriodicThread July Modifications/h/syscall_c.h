#ifndef SYSCALL_C_H
#define SYSCALL_C_H

#include "../lib/hw.h"
#include "../h/PCB.hpp"  // for struct _thread


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Allocates given amount of bytes on heap and
 * returns pointer to beginning of that memory.
 *
 * @param size - minimum number of bytes to
 *               allocate
 * @return Address of continuous block of bytes
 *         of given size.
 */
void* mem_alloc (size_t size);


/**
 * Labels given data as deallocated and returns
 * 0 in case of success, negative value otherwise.
 *
 * @param data - ptr returned by previously called mem_alloc
 * @return - 0 in case of success, negative value otherwise.
 */
int mem_free (void* data);


struct _thread;
typedef struct _thread* thread_t;

/// Returns 0 in case of success, else negative value.
int thread_create (
    thread_t* handle,
    void(*start_routine)(void*),
    void* arg
);


/**
 * Shuts down currently running thread.
 * In case of error returns negative value,
 * otherwise does not return and new running executes.
 *
 * @return In case of error returns error code.
 */
int thread_exit();


/// Potentially gives up the processor to
/// some other thread.
void thread_dispatch ();


struct _sem;
typedef struct _sem* sem_t;

int sem_open (
    sem_t* handle,
    unsigned init
);


/**
 * Deallocates given semaphore and unblocks all threads
 * in the waiting queue under it, where their wait() returns
 * a negative error code.
 *
 * @param handle
 * @return In case of success returns 0, else negative value.
 */
int sem_close (sem_t handle);


/**
 * Waits on the given semaphore by yielding the processor.
 *
 * @param id - handle of the semaphore
 * @return In case of success returns 0, else negative value
 *         (including the destruction of passed id).
 */
int sem_wait (sem_t id);


int sem_signal (sem_t id);


typedef unsigned long time_t;

/**
 * Puts currently running thread to sleep for
 * given number of clock ticks.
 *
 * @param sleep_duration - number of clock ticks
 * @return 0 in case of success, else negative value
 */
int time_sleep (time_t sleep_duration);


const int EOF = -1;

/**
 * Loads in a character from console buffer. In
 * case the buffer is empty suspends currently
 * running thread until a character appears.
 *
 * @return loaded in character in case of success,
 *         EOF in case of error
 */
char getc();


/// Logs given character onto a console.
void putc(char chr);


/// Returns ID of the currently running thread
/// dispatches before it returns.
int getThreadId();


void thread_kill(thread_t handle);


#ifdef __cplusplus
}
#endif


#endif
