#ifndef HASHING_STRUCTURES_HPP
#define HASHING_STRUCTURES_HPP


#include <functional>  // std::hash
#include <memory>  // std::shared_ptr
#include <string>


namespace sale {
namespace hashing {


// =========================================
// Key Hashing Structures:


// key is passed by reference
struct HashSharedPtrConstStr
{
	std::size_t operator()
		(const std::shared_ptr<const std::string> &key_str_ptr) const;
};


// key is passed by value
struct HashInt
{
	std::size_t operator() (const int id) const;
};
//
//struct HashEventPtr
//{
//	std::size_t operator() (const std::shared_ptr<sale::Event> &event_) const;
//};
//

// =========================================
// Key Equality Structures:


// key is passed by reference
struct EqualitySharedPtrConstStr
{
	bool operator()
		(const std::shared_ptr<const std::string> &x,
		 const std::shared_ptr<const std::string> &y) const;
};


// key is passed by value
struct EqualityInt
{
	bool operator() (const int x, const int y) const;
};


}  // end of hashing namespace
}  // end of sale namespace

#endif
