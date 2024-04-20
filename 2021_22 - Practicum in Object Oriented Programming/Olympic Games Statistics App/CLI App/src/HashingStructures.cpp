#include "HashingStructures.hpp"


// =========================================
// Key Hashing Structures:


std::size_t sale::hashing::HashSharedPtrConstStr::operator()
    (const std::shared_ptr<const std::string> &key_str_ptr) const
{
    return std::hash<std::string>{} (*key_str_ptr);
}

std::size_t sale::hashing::HashInt::operator() (const int id) const
{
    return std::hash<int>{} (id);
}


// =========================================
// Key Equality Structures:


bool sale::hashing::EqualitySharedPtrConstStr::operator()
    (const std::shared_ptr<const std::string> &x,
     const std::shared_ptr<const std::string> &y) const
{
    return std::strcmp(x->c_str(), y->c_str()) == 0;
}

bool sale::hashing::EqualityInt::operator() (const int x, const int y) const {
    return x == y;
}

//std::size_t sale::hashing::HashEventPtr::operator()(const std::shared_ptr<sale::Event>& event_) const
//{
//    int sign = event_->getType() == sale::Event::TEventType::Individual ? 1 : -1;
//    return sign * (std::hash<std::string>{} (event_->getNameReference()));
//}
