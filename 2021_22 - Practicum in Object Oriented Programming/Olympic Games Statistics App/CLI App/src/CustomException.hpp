#ifndef CUSTOM_EXCEPTIONS_HPP
#define CUSTOM_EXCEPTIONS_HPP

#include <exception>
#include <string> 


namespace sale {
namespace exceptions {

class CustomException : public std::exception {
 private:
	std::string msg = "A custom exceptions msg";

 public:
	CustomException() = default;
	CustomException(const std::string &msg) : msg(msg) {}
	const char* what() const noexcept override { return this->msg.c_str(); }
};


}  // end of exceptions namespace
}  // end of sale namespace

#endif

