#include "../h/syscall_cpp.hpp"
#include "../h/PCB.hpp"
#include "../h/RISCV.hpp"


void* operator new(size_t size) {
    return mem_alloc(size);
}


void operator delete (void *data) {
    mem_free(data);
}

//void *operator new[](size_t n) { return mem_alloc(n); }

//void operator delete[](void *p) noexcept { mem_free(p); }


// ==================================
// ==================================
// ==================================
// Thread Implementation:


// ==================================
// Helpers:


namespace {


class BodyWrapperSingleArg
{

 public:


    using main_t = kernel::PCB::main_t;
    using arg_t = kernel::PCB::arg_t;


    // ==================================
    // Constructors:


    /// Returns nullptr in case there was an error in
    /// constructing the object, cannot do it via
    /// regular constructor as in this project it cannot throw.
    static BodyWrapperSingleArg* createInstance(main_t main, arg_t arg) {
        void *self_memory = mem_alloc(sizeof(BodyWrapperSingleArg));
        if (!self_memory) return nullptr;  // malloc failed

        BodyWrapperSingleArg *self
            = new (self_memory) BodyWrapperSingleArg(main, arg);

        if (sem_open(&self->sem_deallocate, 0) != 0) {  // failed to create
            delete self;
            return nullptr;
        }

        if (sem_open(&self->sem_done, 0) != 0) {  // failed to create
            sem_close(self->sem_deallocate);
            delete self;
            return nullptr;
        }

        return self;
    }

    void* operator new (size_t size, void *memory) {
        return (BodyWrapperSingleArg *) memory;
    }


    // ==================================
    // Destructor:


    static void deleteInstance(BodyWrapperSingleArg *self) {
        if (!self) return;
        // implicitly evokes the destructor before mem_free
        delete self;
    }

    void operator delete(void *data) {
        mem_free(data);
    }


    // ==================================
    // Methods:


    static void runWrapper(void *body_wrapper) {
        BodyWrapperSingleArg *self = (BodyWrapperSingleArg *) body_wrapper;

        self->main(self->arg);

        sem_signal(self->sem_done);

        // request to dealloc PCB implicitly by kernel
        // by just letting this function end
        sem_wait(self->sem_deallocate);
    }


    // ==================================
    // Semaphore Communication:


    /// Waits until the main body is done executing.
    void waitUntilDone() volatile {
        if (!has_thread_started || !this->sem_done) return;

        sem_wait(this->sem_done);
    }


    /// Allow the body to end and start the deallocation
    /// process within the kernel.
    void acknowledgeDeallocationRequest() volatile {
        if (!has_thread_started || !this->sem_deallocate) return;

        sem_signal(this->sem_deallocate);
    }

    void setHasThreadStarted() {
        this->has_thread_started = true;
    }


 private:


    BodyWrapperSingleArg(main_t main, arg_t arg)
        : main(main), arg(arg)
    {

    }

    ~BodyWrapperSingleArg() {
        if (this->sem_deallocate) sem_close(this->sem_deallocate);
        if (this->sem_done) sem_close(this->sem_done);
    }


    // ==================================
    // Attributes:


    sem_t sem_done = nullptr;
    sem_t sem_deallocate = nullptr;

    main_t main = nullptr;
    arg_t arg = nullptr;

    bool has_thread_started = false;

};


}  // anonymous namespace


// ==================================
// Constructors:


Thread::Thread(void (*body)(void *), void *arg)
    : myHandle(nullptr)
{
    if (!body) return;

    // Allocate all space necessary for this thread:
    void *pcb_memory = mem_alloc(sizeof(kernel::PCB));
    if (!pcb_memory) return;  // malloc failed

    // Prepare all arguments:
    BodyWrapperSingleArg *body_wrapper
        = BodyWrapperSingleArg::createInstance(body, arg);

    if (!body_wrapper) {  // failed to allocate the body wrapper struct
        // deallocate the rest
        mem_free(pcb_memory);
        return;
    }

    // just create handle in order to store the body wrapper
    // so start and destructor could access it
    this->myHandle = (thread_t) new (pcb_memory) kernel::PCB(
        BodyWrapperSingleArg::runWrapper,
        body_wrapper,
        // dummy values
        nullptr,
        0
    );
}

Thread::Thread()
    : myHandle(nullptr)
{
    // Allocate all space necessary for this thread:
    void *pcb_memory = mem_alloc(sizeof(kernel::PCB));
    if (!pcb_memory) return;  // malloc failed

    // has friend privilege to call protected method run
    struct Wrapper {
        static void run(void *thread) {
            ((Thread *) thread)->run();
        }
    };

    // Prepare all arguments:
    BodyWrapperSingleArg *body_wrapper = BodyWrapperSingleArg::createInstance(Wrapper::run, (void *) this);

    if (!body_wrapper) {  // failed to allocate the body wrapper struct
        // deallocate the rest
        mem_free(pcb_memory);
        return;
    }

    // just create handle in order to store the body wrapper
    // so start and destructor could access it
    this->myHandle = (thread_t) new (pcb_memory) kernel::PCB(
        BodyWrapperSingleArg::runWrapper,
        body_wrapper,
        // dummy values
        nullptr,
        0
    );
}


// ==================================
// Destructor:


Thread::~Thread() {
    // if malloc failed upon construction of the thread
    // then there is nothing to clean up
    if (!this->myHandle) return;

    kernel::PCB *my_handle = (kernel::PCB *) this->myHandle;

    // once the thread is done executing it will
    // be waiting for acknowledge signal so the
    // handle will currently still point to valid address
    BodyWrapperSingleArg *body_wrapper = (BodyWrapperSingleArg *) my_handle->getArg();
    body_wrapper->waitUntilDone();

    // allow the deallocation and true end of execution of the thread
    body_wrapper->acknowledgeDeallocationRequest();

    // my_handle is now a dangling pointer so set it to 0
    this->myHandle = nullptr;

    // deallocate the body wrapper
    BodyWrapperSingleArg::deleteInstance(body_wrapper);
}


// ==================================
// Public Methods:


int Thread::start() {
    // create the real thread and let it execute
    kernel::PCB *my_handle = (kernel::PCB *) this->myHandle;

    auto wrappers_run = my_handle->getMain();
    auto wrapper_ptr = my_handle->getArg();

    // deallocate the temporary thread
    mem_free(my_handle);

    int err_code = thread_create(&this->myHandle, wrappers_run, wrapper_ptr);
    if (err_code == 0) ((BodyWrapperSingleArg*) wrapper_ptr)->setHasThreadStarted();
    thread_dispatch();
    return err_code;
}


// ==================================
// Public Static Methods:


void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t duration) {
    return time_sleep(duration);
}


// ==================================
// Protected Static Methods:


bool Thread::destroyWithoutWaiting(Thread *self) {
    kernel::PCB *my_handle = (kernel::PCB *) self->myHandle;

    // disallow calling this twice
    if (!my_handle) return false;

    // deallocate the body wrapper
    BodyWrapperSingleArg *body_wrapper = (BodyWrapperSingleArg *) my_handle->getArg();

    // kill the thread first
    thread_kill(self->myHandle);

    BodyWrapperSingleArg::deleteInstance(body_wrapper);

    // flag for destructor doing nothing
    self->myHandle = nullptr;

    return true;
}

bool Thread::hasBeenDestroyed(Thread *self) {
    return self->myHandle == nullptr;
}


// ==================================
// ==================================
// ==================================
// Periodic Thread Implementation:


namespace {


struct PeriodicThreadDescriptor {
    PeriodicThread *thread = nullptr;
    time_t period = 0;

    PeriodicThreadDescriptor(PeriodicThread *thread, time_t period)
        : thread(thread), period(period)
    {
    }
};


}  // anonymous namespace


void PeriodicThread::periodicThreadWrapper(void *arg) {
    if (!arg) return;  // failed to malloc space for the arg

    // extract everything from given arg and
    // dealloc the argument now, as the forever loop
    // will have to be killed agressively
    auto per_thread_desc = (PeriodicThreadDescriptor *) arg;
    time_t period = per_thread_desc->period;
    PeriodicThread *thread = per_thread_desc->thread;
    mem_free(arg);

    for ( ; ; ) {
        thread->periodicActivation();
        time_sleep(period);
    }
}


PeriodicThread::PeriodicThread(time_t period)
    : Thread(
        PeriodicThread::periodicThreadWrapper,
        new PeriodicThreadDescriptor(this, period)
    )
{
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());

    flow.lockPeriodicThread();
    flow.increasePeriodicThreadCount(this);
    flow.unlockPeriodicThread();
}

PeriodicThread::~PeriodicThread() {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());
    flow.lockPeriodicThread();

    if (!Thread::hasBeenDestroyed(this)) {
        // this thread has not already been destroyed
        flow.decreasePeriodicThreadCount(this);

        // kill this thread and
        // deallocate and clean up to avoid waiting
        // in the parent class destructor
        Thread::destroyWithoutWaiting(this);
    }

    flow.unlockPeriodicThread();
}

void PeriodicThread::stopThread() {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());
    flow.lockPeriodicThread();

    if (!Thread::hasBeenDestroyed(this)) {
        // this thread has not already been destroyed
        flow.decreasePeriodicThreadCount(this);

        // kill this thread and
        // deallocate and clean up to avoid waiting
        // in the parent class destructor
        Thread::destroyWithoutWaiting(this);
    }

    flow.unlockPeriodicThread();
}


// ==================================
// ==================================
// ==================================
// Semaphore Implementation:


// ==================================
// Constructors:


Semaphore::Semaphore(unsigned int init) {
    sem_open(&this->myHandle, init);
}


// ==================================
// Destructor:


Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}


// ==================================
// Public Methods:


int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal(this->myHandle);
}



// ==================================
// ==================================
// ==================================
// Console Implementation:


char Console::getc() {
    return ::getc();
}

void Console::putc(char chr) {
    ::putc(chr);
}
