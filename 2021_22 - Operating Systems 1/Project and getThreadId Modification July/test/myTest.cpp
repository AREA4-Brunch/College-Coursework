#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.h"
#include "../test/printing.hpp"


Semaphore *STDOUT_WRITE_MUTEX;
Semaphore *SLEEP_MUTEX;


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
