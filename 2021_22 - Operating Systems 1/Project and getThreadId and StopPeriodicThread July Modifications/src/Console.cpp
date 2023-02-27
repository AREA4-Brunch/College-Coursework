#include "../h/Console.hpp"
#include "../h/syscall_c.h"  // semaphores


// ==================================
// Constructor and Helpers:


kernel::Console::Console()
    : input_stream(new kernel::utils::ThreadSafeBuffer<char> (256)),
      output_stream(new kernel::utils::ThreadSafeBuffer<char> (256)),
      mutex_hardware_write(new kernel::Semaphore(1)),
      mutex_hardware_read(new kernel::Semaphore(1))
{

}

kernel::Console& kernel::Console::getInstance() {
    static kernel::Console instance;
    return instance;
}


// ==================================
// Hardware Communication:


int kernel::Console::popOutputStreamCharToHardware(kernel::ControlFlow &flow) {
    int err_code = 0;  // success

    // do not retry wait in case of error in order not
    // to stay too long in the interrupt routine
    if (mutex_hardware_write->wait(flow) < 0) return err_code = -100;

    // pop char only when write is available otherwise
    // it would have to be pushed back into output stream
    if (this->isWriteAvailable()) {
        const char byte = this->popFromOutputStream(err_code);

        if (err_code < 0) {  // failed to pop
            if (mutex_hardware_write->signal(flow) < 0) {  // signal failed
                // add 1000 on top of err code from popFromOutputStream
                err_code -= 1000;  // write was not available
            }

            return err_code;  // failed to retrieve char
        }
        // else err_code matches one from popFromOutputStream

        __asm__ volatile (
              "sb %[data], (%[dest])"
            :
            : [data] "r" (byte),
              [dest] "r" (::CONSOLE_TX_DATA)
        );
    } else {
        err_code = -101;  // write is unavailable
    }

    // do not retry wait in case of error in order not
    // to stay too long in the interrupt routine
    if (mutex_hardware_write->signal(flow) < 0) {  // signal failed
        // add 1000 on top of possible err code from popFromOutputStream
        if (err_code >= 0) err_code += 1000;
        else err_code -= 1000;  // write was not available before this
    }

    return err_code;
}

int kernel::Console::onInterrupt(kernel::ControlFlow &flow) {
    int err_code = 0;  // success

    // do not retry wait in case of error in order not
    // to stay too long in the interrupt routine
    if (this->mutex_hardware_read->wait(flow) < 0) return err_code = -100;

    for (int i = 0, max_chars_per_interrupt_call = 20;
         i < max_chars_per_interrupt_call && this->isReadAvailable();
         ++i)
    {
        // read byte from console without any semaphores
        const char read_chr = static_cast<char> (this->readByteFromConsole());

        // kill oldest created periodic thread if k
        // was typed
        if (read_chr == 'k') {
            flow.signalKillOldestPeriodicThread();
        }

        // push using semaphore
        err_code = this->pushIntoInputStream(read_chr);
    }

    // do not retry signal in case of error in order not
    // to stay too long in the interrupt routine
    if (this->mutex_hardware_read->signal(flow) < 0) {  // signal failed
        // add 1000 on top of possible err code from pushIntoInputStream
        err_code += 1000;
    }

    return err_code;
}


// ==================================
// Misc.


bool kernel::Console::isOutputStreamEmpty() const {
    // thread-safe size checking and check if anything
    // is waiting to send to hardware
    bool is_os_empty =  this->mutex_hardware_write->getValue() >= 0
                     && this->output_stream->size() == 0;

    return is_os_empty;
}

void kernel::Console::waitOutputStream() const {
    while (!this->isOutputStreamEmpty()) {
        thread_dispatch();
    }
}

void kernel::Console::printString(const char *string) {
    // forbid any interruptions
//    auto &processor = kernel::RISCV::getInstance();
//    // store current sstatus to later restore old SIE value
//    auto sstatus = processor.sstatus().read();
//    processor.sstatus().mask_clear(kernel::RISCV::sstatus_t::BitMask::SIE);


    for ( ; *string != '\0'; ++string) {
        this->pushIntoOutputStream(*string);
    }


    // restore SIE
//    processor.sstatus().mask_set(
//    (sstatus & kernel::RISCV::sstatus_t::BitMask::SIE) ?
//           kernel::RISCV::sstatus_t::BitMask::SIE  // was already enabled
//         : 0UL
//    );
}


// ==================================
// Adding Chars into Streams:


int kernel::Console::pushIntoInputStream(char chr) {
    return this->input_stream->push(chr);
}

int kernel::Console::pushIntoOutputStream(char chr) {
    if (chr == '\r') chr = '\n';
    return this->output_stream->push(chr);
}


// ==================================
// Removing Chars from Streams:


char kernel::Console::popFromInputStream(int &err_code) {
    return this->input_stream->pop(err_code);
}

char kernel::Console::popFromOutputStream(int &err_code) {
    return this->output_stream->pop(err_code);
}


// ==================================
// Private Helpers:


bool kernel::Console::isReadAvailable() const {
    return this->getConsoleStatus() & ::CONSOLE_RX_STATUS_BIT;
}

bool kernel::Console::isWriteAvailable() const {
    return this->getConsoleStatus() & ::CONSOLE_TX_STATUS_BIT;
}

uint8 kernel::Console::getConsoleStatus() const {
    uint8 volatile console_status;

    __asm__ volatile (
          "lb %[dest], (%[src])"
        : [dest] "=r" (console_status)
        : [src] "r" (::CONSOLE_STATUS)
    );

    return console_status;
}

uint8 kernel::Console::readByteFromConsole() const {
    uint8 volatile byte;

    __asm__ volatile (
          "lb %[dest], (%[src])"
        : [dest] "=r" (byte)
        : [src] "r" (::CONSOLE_RX_DATA)
    );

    return byte;
}

bool kernel::Console::writeByteToConsole(const uint8 byte) {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());
    bool did_write = false;

    // keep retrying to wait in case error occurred
    while (mutex_hardware_write->wait(flow) < 0);

    if (this->isWriteAvailable()) {
        __asm__ volatile (
              "sb %[data], (%[dest])"
            :
            : [data] "r" (byte),
              [dest] "r" (::CONSOLE_TX_DATA)
        );

        did_write = true;
    }

    // keep retrying to signal in case error occurred
    while (mutex_hardware_write->signal(flow) < 0);

    return did_write;
}
