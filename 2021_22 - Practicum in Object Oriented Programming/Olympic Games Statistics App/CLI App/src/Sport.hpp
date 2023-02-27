#ifndef SPORT_HPP
#define SPORT_HPP


#include "HashingStructures.hpp"
#include "HashableData.hpp"
#include "Event.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <map>


// Declare Operators to Overload:

namespace sale { class Sport; }
bool operator== (const sale::Sport &x, const sale::Sport &y);


namespace sale {


class Sport : public HashableData< std::shared_ptr<const std::string>,
								   sale::hashing::HashSharedPtrConstStr,
								   sale::hashing::EqualitySharedPtrConstStr >
{
	using events_map_t = std::unordered_multimap<
							sale::Event::TKey,  // key
							std::shared_ptr<sale::Event>,  // data
							sale::Event::KeyHashStruct,
							sale::Event::KeyEqStruct
						  >;

 public:

	// Constructors and Destructor:
	Sport(const std::string &name);
	Sport(std::shared_ptr<const std::string> name_ptr);
	~Sport() override = default;


	// HashableData overrides:
	TKey getKey() const override;


	// Getters:
	std::string getName() const;
	const std::string& getNameReference() const;
	const events_map_t& getEvents() const;

	// Managing Events:
	void addEvent(std::shared_ptr<sale::Event> event_ptr);
	bool doesEventExist(const sale::Event::TKey &key,
						const sale::Event::TEventType event_type) const;
	std::shared_ptr<sale::Event> getEvent(const sale::Event::TKey &key,
										  const sale::Event::TEventType event_type) const;

	// Operator Overloads:
	friend bool ::operator== (const Sport &x, const Sport &y);


  private:

	TKey name;

	// map of events
	std::unique_ptr<
		std::unordered_multimap<
			sale::Event::TKey,  // key
			std::shared_ptr<sale::Event>,  // data
			sale::Event::KeyHashStruct,
			sale::Event::KeyEqStruct
		>
	> events = nullptr;
};


}  // end of sale namespace

#endif
