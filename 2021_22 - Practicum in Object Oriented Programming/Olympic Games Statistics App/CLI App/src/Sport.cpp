#include "Sport.hpp"

#include "CustomException.hpp"


typedef std::unordered_multimap<
			sale::Event::TKey,  // key
			std::shared_ptr<sale::Event>,  // data
			sale::Event::KeyHashStruct,
			sale::Event::KeyEqStruct
		> events_map_t;


// ================================
// Constructors:


sale::Sport::Sport(const std::string &name)
	: name(new const std::string(name)), events(new events_map_t())
{
}

sale::Sport::Sport(std::shared_ptr<const std::string> name_ptr)
	: name(std::move(name_ptr)), events(new events_map_t())
{
}


// ================================
// HashableData overrides:


sale::Sport::TKey sale::Sport::getKey() const
{
	return this->name;
}


// ================================
// Getters:


std::string sale::Sport::getName() const
{
	return *this->name;
}

const std::string& sale::Sport::getNameReference() const
{
	return *this->name;
}

const events_map_t& sale::Sport::getEvents() const
{
	// if events have not been created return empty map
	if (!this->events) {
		throw sale::exceptions::CustomException("Sport events ptr is nullptr");
	}
	return *this->events;
}


// ================================
// Managing Events:


void sale::Sport::addEvent(std::shared_ptr<sale::Event> event_ptr)
{
	//(*this->events)[event_ptr->getKey()] = std::move(event_ptr);
	const auto &key = event_ptr->getKey();
	this->events->insert({key, std::move(event_ptr)});
}

bool sale::Sport::doesEventExist(const sale::Event::TKey& key,
								 const sale::Event::TEventType event_type) const
{
	//return this->events->count(key) > 0;
	auto keys_range = this->events->equal_range(key);
	for (auto iter = keys_range.first, end = keys_range.second;
		 iter != end;
		 ++iter)
	{
		if (iter->second->getType() == event_type) return true;
	}
	return false;
}

std::shared_ptr<sale::Event> sale::Sport::getEvent(const sale::Event::TKey& key, const sale::Event::TEventType event_type) const
{
	auto keys_range = this->events->equal_range(key);
	for (auto iter = keys_range.first, end = keys_range.second;
		iter != end;
		++iter)
	{
		if (iter->second->getType() == event_type) return iter->second;
	}
	return nullptr;
}


// ================================
// Operator Overloads:


bool operator==(const sale::Sport& x, const sale::Sport& y)
{
	static auto key_cmp_func = sale::Sport::KeyEqStruct();
	return key_cmp_func(x.name, y.name);
}
