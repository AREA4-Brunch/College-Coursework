#ifndef _syscall_cpp
#define _syscall_cpp

#include "syscall_c.h"


void* operator new (size_t);
void operator delete (void*);


class Thread {

 public:


    /// Initializes thread with given params, but
    /// does not start it.
    Thread (void (*body)(void*), void* arg);

    /// Waits for the thread to be done executing.
    virtual ~Thread ();

    /// Starts the thread and returns 0 in case of success,
    /// returns negative value otherwise.
    int start ();

    static void dispatch ();

    static int sleep (time_t);


 protected:


    /// Constructs thread whose body is method run,
    /// does not start it.
    Thread ();


    virtual void run () {}


 private:


    thread_t myHandle;

};


class Semaphore {

public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();

private:
    sem_t myHandle;

};


class PeriodicThread : public Thread {

protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}

private:

    static void periodicThreadWrapper(void *desc);

};


class Console {

public:
    static char getc ();
    static void putc (char);

};

#endif
