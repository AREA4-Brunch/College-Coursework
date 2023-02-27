#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.h"
#include "../test/printing.hpp"


Semaphore *STDOUT_WRITE_MUTEX;
Semaphore *SLEEP_MUTEX;
Semaphore *STDOUT_WRITE_MUTEX_PER_THREADS;


class MyThread : public Thread {

 public:

    MyThread(const char *name) : name(name) {}


 protected:


    virtual void run() override final {
        for (int i = 0; i < 10; ++i) {

            STDOUT_WRITE_MUTEX->wait();

            printString(this->name);
            printString(", id: ");
            printInt(getThreadId());
            printString("\n");

            STDOUT_WRITE_MUTEX->signal();


            SLEEP_MUTEX->wait();
            time_sleep(2);
            SLEEP_MUTEX->signal();
        }
    }


 private:

    const char *name;


};


void myTest() {
    STDOUT_WRITE_MUTEX = new Semaphore(1);
    SLEEP_MUTEX = new Semaphore(1);

    printString("id: ");
    printInt(getThreadId());
    printString("\n");

    MyThread A("A");
    MyThread B("B");
    MyThread C("C");

    A.start();
    B.start();
    C.start();

}


class MyPeriodicThread : public PeriodicThread {

 public:

    MyPeriodicThread(const char *name) : PeriodicThread(2), name(name) {

    }


 protected:


    virtual void periodicActivation() override {
        STDOUT_WRITE_MUTEX_PER_THREADS->wait();

        printString(this->name);
        printString("\n");

        STDOUT_WRITE_MUTEX_PER_THREADS->signal();
    }


 private:

    const char *name;

};


void myTest2() {
    STDOUT_WRITE_MUTEX_PER_THREADS = new Semaphore(1);


    MyPeriodicThread *threadA = new MyPeriodicThread("A");
    MyPeriodicThread *threadB = new MyPeriodicThread("B");
    MyPeriodicThread *threadC = new MyPeriodicThread("C");

    printString("A: ");
    printInt((uint64) threadA);
    printString("\nB: ");
    printInt((uint64) threadB);
    printString("\nC: ");
    printInt((uint64) threadC);
    printString("\n");

    threadA->start();
    threadB->start();
    threadC->start();

    time_sleep(40);
    threadB->stopThread();

}
