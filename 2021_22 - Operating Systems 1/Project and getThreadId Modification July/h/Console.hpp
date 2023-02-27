#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "../lib/hw.h"
#include "../h/ThreadSafeBuffer.hpp"


namespace kernel {


class Console final
{


 public:


    static Console& getInstance();

    // Forbid copies because of Singleton
    Console(const Console&) = delete;
    Console& operator= (const Console&) = delete;


    // ==================================
    // Hardware Communication:


    /**
     * If write is available pops a char into the
     * console's hardware and returns true, else
     * returns false.
     *
     * @param flow - flow on which semaphores will execute.
     * @return - 0 if everything was successful;
     *         - positive value if data was successfully stored,
     *           but semaphore signalling error occurred;
     *         - negative value if data was not stored;
     */
    int popOutputStreamCharToHardware(kernel::ControlFlow &flow);

    /**
     * Loads in new characters, if any, from console
     * hardware into input stream.
     * Must be called from interrupt routine.
     *
     * @param flow - flow on which semaphores will execute.
     * @return - 0 if everything was successful;
     *         - positive value if data was successfully stored,
     *           but semaphore signalling error occurred;
     *         - negative value if data was not stored;
     */
    int onInterrupt(kernel::ControlFlow &flow);


    // ==================================
    // Misc.


    /// Returns whether there is nothing left to send to
    /// to the console and there is currently no
    /// character transfer pending either.
    /// Is thread-safe, must be called outside interrupt
    /// routine, since it uses C API.
    bool isOutputStreamEmpty() const;

    /// Waits for all characters from output stream to
    /// be sent to the console's hardware.
    /// Is thread-safe, must be called outside interrupt
    /// routine, since it uses C API.
    void waitOutputStream() const;

    /// Pushes chars of given string into the buffer.
    /// Is thread-safe.
    void printString(const char *string);

    /// Converts given int into string and pushes chars into
    /// the buffer.
    /// Is thread-safe.
    template<typename TInt>
    void printInt(TInt x);

    /// Converts given int into string and pushes chars into
    /// the buffer along with newline char at the end.
    /// Is thread-safe.
    template<typename TInt>
    void printlnInt(TInt x);


    // ==================================
    // Adding Chars into Streams:


    /**
     * Adds given char to the input stream.
     * Is thread-safe.
     *
     * @param chr
     * @return - 0 if everything was successful;
     *         - positive value if data was successfully stored,
     *           but semaphore signalling error occurred;
     *         - negative value if data was not stored;
     */
    int pushIntoInputStream(char chr);

    /**
     * Adds given char to the output stream.
     * Is thread-safe.
     *
     * @param chr
     * @return - 0 if everything was successful;
     *         - positive value if data was successfully stored,
     *           but semaphore signalling error occurred;
     *         - negative value if data was not stored;
     */
    int pushIntoOutputStream(char chr);


    // ==================================
    // Removing Chars from Streams:


    /**
     * Pops first char from the input stream.
     * Is thread-safe.
     *
     * @param err_code - stores into it:
     *                   - 0 if everything was successful;
     *                   - positive value if data was successfully retrieved,
     *                     but semaphore signalling error occurred;
     *                   - negative value if operation was unsuccessful;
     * @return First element from the buffer
     */
    char popFromInputStream(int &err_code);

    /**
     * Pops first char from the output stream.
     * Is thread-safe.
     *
     * @param err_code - stores into it:
     *                   - 0 if everything was successful;
     *                   - positive value if data was successfully retrieved,
     *                     but semaphore signalling error occurred;
     *                   - negative value if operation was unsuccessful;
     * @return First element from the buffer
     */
    char popFromOutputStream(int &err_code);


 private:


    // ==================================
    // Constructor, Destructor and Singleton:


    Console();

    // must not destroy local static variable since
    // project is missing __dso_handle
    //~Console();


    // ==================================
    // Attributes:


    // must be pointer so there is no non-primitive's destructor as
    // __dso_handle missing error occurs
    kernel::utils::ThreadSafeBuffer<char> *input_stream = nullptr;

    // must be pointer so there is no non-primitive's destructor as
    // __dso_handle missing error occurs
    kernel::utils::ThreadSafeBuffer<char> *output_stream = nullptr;

    // locks writeByteToConsole() and popOutputStreamCharToHardware()
    kernel::Semaphore *mutex_hardware_write = nullptr;

    // locks readByteFromConsole() and onInterrupt()
    kernel::Semaphore *mutex_hardware_read = nullptr;


    // ==================================
    // Helpers:


    /// Returns whether console has set the ready bit
    /// for reading single byte from it.
    /// Not thread-safe, should be called from interrupt routine.
    bool isReadAvailable() const;

    /// Returns whether console has set the ready bit
    /// for writing single byte to it.
    /// Not thread-safe, should be called from interrupt routine.
    bool isWriteAvailable() const;

    /// Not thread-safe, should be called from interrupt routine.
    uint8 getConsoleStatus() const;

    /// Does not check if read is available.
    /// Not thread-safe, should be called from interrupt routine.
    uint8 readByteFromConsole() const;

    /**
     * Writes given byte to the console's hardware if write
     * is available.
     * Is thread-safe.
     *
     * @param byte
     * @return true if write was available, false otherwise.
     */
    bool writeByteToConsole(uint8 byte);

};


} // kernel namespace



// ==================================
// ==================================
// ==================================
// Implementation of Template Methods:


template<typename TInt>
void kernel::Console::printInt(TInt x) {
//    // forbid any interruptions
//    auto &processor = kernel::RISCV::getInstance();
//    // store current sstatus to later restore old SIE value
//    auto sstatus = processor.sstatus().read();
//    processor.sstatus().mask_clear(kernel::RISCV::sstatus_t::BitMask::SIE);


    const int sign = x < 0 ? -1 : 1;
    const char last_digit = '0' + sign * (x % 10);
    x /= sign * 10;  // remove last digit and make x positive

    TInt divisor = (TInt) 1;
    while (divisor <= x) divisor *= 10;

    // log negative sign
    if (sign < 0) this->pushIntoOutputStream('-');

    // log all digits of current x (missing last)
    for (divisor /= 10; divisor > 0; x %= divisor, divisor /= 10) {
        this->pushIntoOutputStream('0' + x / divisor);
    }

    this->pushIntoOutputStream(last_digit);


    // restore SIE
//    processor.sstatus().mask_set(
//    (sstatus & kernel::RISCV::sstatus_t::BitMask::SIE) ?
//           kernel::RISCV::sstatus_t::BitMask::SIE  // was already enabled
//         : 0UL
//    );
}

template<typename TInt>
void kernel::Console::printlnInt(TInt x) {
    // forbid any interruptions
//    auto &processor = kernel::RISCV::getInstance();
//    // store current sstatus to later restore old SIE value
//    auto sstatus = processor.sstatus().read();
//    processor.sstatus().mask_clear(kernel::RISCV::sstatus_t::BitMask::SIE);


    this->printInt(x);
    this->pushIntoOutputStream('\n');


    // restore SIE
//    processor.sstatus().mask_set(
//    (sstatus & kernel::RISCV::sstatus_t::BitMask::SIE) ?
//           kernel::RISCV::sstatus_t::BitMask::SIE  // was already enabled
//         : 0UL
//    );
}


#endif //CONSOLE_HPP
