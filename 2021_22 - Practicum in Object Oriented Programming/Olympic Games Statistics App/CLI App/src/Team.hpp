#ifndef TEAM_HPP
#define TEAM_HPP


#include "Competitor.hpp"
#include "Athlete.hpp"
#include <vector>
#include <functional>
#include <iostream>


namespace sale {


// Friend Declarations:

template<typename TLink> class Team;

//template<typename TLink>
//std::ostream& operator<< (std::ostream &os, const sale::Team<TLink> &self);
//

// TLink has to have public:
// std::shared_ptr<sale::Athlete> operator() (const sale::Athlete::TKey &) const;
template<typename TLink>
class Team final : public Competitor
{

 public:

	Team(std::shared_ptr<sale::Event> event_ptr, const int oly_games_id);

	Team(const int max_num_athletes,
		 std::shared_ptr<sale::Event> event_ptr,
		 const int oly_games_id);

	~Team() override = default;


	void addAthlete(sale::Athlete::TKey key);


	// Getters:
	
	const auto& getAthletesKeys() const;

	// reduces all vectors down to take up only as much
	// as the elemnts inside them
	virtual void shrinkToFit() override;


	// Operator Overloads:

	//template<typename TLink>
	//friend std::ostream& operator<< (std::ostream& os, const sale::Team<TLink>& self) {

	//}


 private:

	// function connecting Athlete ptrs with keys stored locally
	static const std::function<
		std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)
	> getAthletePtrFunc;
	
	// ids of athletes in the team
	std::vector<std::remove_const<sale::Athlete::TKey>::type> athlete_keys;

};


}  // end of namespace sale



// ================================
// ================================
// Implementation:


// ================================
// Static Fields Initializations:


template<typename TLink>
const std::function<std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)>
sale::Team<TLink>::getAthletePtrFunc = TLink{};


// ================================
// Constructors:


template<typename TLink>
sale::Team<TLink>::Team(std::shared_ptr<sale::Event> event_ptr,
						const int oly_games_id)
{
	this->addEvent(std::move(event_ptr), oly_games_id);
}

template<typename TLink>
sale::Team<TLink>::Team
	(const int max_num_athletes, std::shared_ptr<sale::Event> event_ptr,
	 const int oly_games_id)
	: Team(std::move(event_ptr), oly_games_id)
{
	this->athlete_keys.reserve(max_num_athletes);
}


// ================================
// Public Methods:


template<typename TLink>
void sale::Team<TLink>::addAthlete(sale::Athlete::TKey key)
{
	this->athlete_keys.push_back(key);
}

template<typename TLink>
const auto& sale::Team<TLink>::getAthletesKeys() const
{
	return this->athlete_keys;
}

template<typename TLink>
void sale::Team<TLink>::shrinkToFit()
{
	sale::Competitor::shrinkToFit();
	this->athlete_keys.shrink_to_fit();
}


// ================================
// Operator Overloads:

//
//namespace sale {
//
//
//template<typename TLink>
//std::ostream& operator<< (std::ostream& os, const sale::Team<TLink>& self) {
//
//}
//
//
//}  // end of sale namespace
//

#endif

