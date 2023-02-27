#ifndef DATA_BASE_EXCEPTIONS_HPP
#define DATA_BASE_EXCEPTIONS_HPP


#include <exception>
#include <string>


namespace sale {
namespace exceptions {


class DataBaseException : public std::exception {
 private:
	std::string msg = "Data Base Error Ocurred";

 public:
	DataBaseException() = default;
	DataBaseException(const std::string &msg) : msg(msg) {}
	const char* what() const noexcept override {
		return this->msg.c_str();
	}
};


class FailedToOpenFile : public std::exception {
 private:
	std::string msg = "Failed to open file, is the file path correct ?";

 public:
	FailedToOpenFile() = default;
	FailedToOpenFile(const std::string &file_path) {
		this->msg = "Failed to open `" + file_path
				  + "`, is the file path correct ? ";
	}
	const char* what() const noexcept override {
		return this->msg.c_str();
	}
};


class LineInvalidFormat : public std::exception {
 public:
	const char* what() const noexcept override {
		return "Line does not match expected format.";
	}
};


}  // end of exceptions namespace
}  // end of sale namespace

#endif
