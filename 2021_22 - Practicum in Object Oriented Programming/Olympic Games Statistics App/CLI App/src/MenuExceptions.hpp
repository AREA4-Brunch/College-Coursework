#ifndef MENU_EXCEPTIONS_HPP
#define MENU_EXCEPTIONS_HPP

#include <exception>
#include <string> 


namespace sale {
namespace exceptions {

class MenuException : public std::exception {
 private:
    std::string msg = "Main Menu Exception Ocurred";

 public:
    MenuException() = default;
    MenuException(const std::string &msg) : msg(msg) {}
    const char* what() const noexcept override { return this->msg.c_str(); }
};


class InvalidOptionOrdinalException : public std::exception {
 public:
    const char* what() const noexcept override {
        return "Invalid option ordinal entered.";
    }
};


}  // end of exceptions namespace
}  // end of sale namespace

#endif

