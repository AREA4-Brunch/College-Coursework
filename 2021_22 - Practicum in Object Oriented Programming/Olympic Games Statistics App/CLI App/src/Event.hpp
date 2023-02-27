#ifndef EVENT_HPP
#define EVENT_HPP


#include "HashingStructures.hpp"
#include "HashableData.hpp"
#include <string>
#include <memory>


// Declare Operators to Overload:

namespace sale { class Event; }
bool operator== (const sale::Event &x, const sale::Event &y);


namespace sale {


class Event : public HashableData< std::shared_ptr<const std::string>,
								   sale::hashing::HashSharedPtrConstStr,
							 	   sale::hashing::EqualitySharedPtrConstStr >
{

 public:

	enum class TEventType : bool { Team, Individual };


	// Constructors and Destructor:

	Event(const TEventType event_type, const std::string &name, const int oly_games_id=-1);
	Event(const TEventType event_type,
		  std::shared_ptr<const std::string> name_ptr, const int oly_games_id=-1);
	~Event() override = default;


	// HashableData overrides:

	TKey getKey() const override;

	
	// Getters:

	TEventType getType() const;
	std::string getName() const;
	const std::string& getNameReference() const;
	const int getOlympicGamesID() const;


	// Operator Overloads:
	friend bool ::operator== (const Event &x, const Event &y);


 private:
	
	TEventType event_type = TEventType::Team;
	TKey name;
	const int oly_games_id = -1;

};


}  // end of sale namespace

#endif
