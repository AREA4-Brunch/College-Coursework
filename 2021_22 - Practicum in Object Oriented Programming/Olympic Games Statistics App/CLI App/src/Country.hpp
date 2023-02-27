#ifndef COUNTRY_HPP
#define COUNTRY_HPP


#include "HashingStructures.hpp"
#include "HashableData.hpp"
#include "Athlete.hpp"
#include "Team.hpp"
#include <string>
#include <memory>
#include <unordered_set>
#include <list>
#include <functional>  // std::remove_const


namespace sale {


// TLink has to have public:
// std::shared_ptr<sale::Athlete> operator()
//	(const sale::Athlete::TKey &) const;
template<typename TLink>
class Country : public HashableData< std::shared_ptr<const std::string>,
									 sale::hashing::HashSharedPtrConstStr,
									 sale::hashing::EqualitySharedPtrConstStr >
{

 public:

	// Constructors and Destructor:

	Country(const std::string &name);
	Country(std::shared_ptr<const std::string> name_ptr);
	~Country() override = default;


	// Getters:

	std::string getName() const;
	const std::string& getNameReference() const;

	const auto& getAthletesKeys() const;
	const auto& getTeams() const;


	// HashableData overrides:

	std::shared_ptr<const std::string> getKey() const override;


	// Managing Competitors:

	void addAthlete(const sale::Athlete::TKey key);
	void addTeam(std::shared_ptr<sale::Team<TLink>> team_ptr);


	// Operator Overloads:
	//friend bool ::operator==<TLink> (const Country &x, const Country &y);
	bool operator== (const Country &other) const;


	// function connecting Athlete ptrs with keys stored locally
	static const std::function<
		std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)
	> getAthletePtrFunc;


 private:

	// name of the country
	TKey name;

	// set of athletes that played for this country
	std::unique_ptr<
		std::unordered_set<
			std::remove_const<Athlete::TKey>::type
		>
	> athletes = nullptr;

	// list of teams that competed for this country
	std::list<std::shared_ptr<sale::Team<TLink>>> teams;

};


}  // end of sale namespace


// ================================
// ================================
// Implementation:


// ================================
// Static Fields Initializations:


template<typename TLink>
const std::function<std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)>
sale::Country<TLink>::getAthletePtrFunc = TLink{};


// ================================
// Constructors:


template<typename TLink>
sale::Country<TLink>::Country(const std::string &name)
	: name(new const std::string(name)),
	  athletes(new std::unordered_set<std::remove_const<Athlete::TKey>::type>())
{
}

template<typename TLink>
sale::Country<TLink>::Country(std::shared_ptr<const std::string> name_ptr)
	: name(std::move(name_ptr)),
	  athletes(new std::unordered_set<std::remove_const<Athlete::TKey>::type>())
{
}


// ================================
// HashableData overrides:


template<typename TLink>
std::shared_ptr<const std::string> sale::Country<TLink>::getKey() const
{
	return this->name;
}


// ================================
// Getters:


template<typename TLink>
std::string sale::Country<TLink>::getName() const
{
	return *this->name;
}

template<typename TLink>
const std::string& sale::Country<TLink>::getNameReference() const
{
	return *this->name;
}

template<typename TLink>
const auto& sale::Country<TLink>::getAthletesKeys() const
{
	if (!this->athletes) {  // return empty set
		return std::unordered_set<
				std::remove_const<Athlete::TKey>::type >();
	}
	return *this->athletes;
}

template<typename TLink>
const auto& sale::Country<TLink>::getTeams() const
{
	return this->teams;
}


// ================================
// Managing Competitors:


template<typename TLink>
void sale::Country<TLink>::addAthlete(const sale::Athlete::TKey key)
{
	this->athletes->insert(key);
}

template<typename TLink>
void sale::Country<TLink>::addTeam
	(std::shared_ptr<sale::Team<TLink>> team_ptr)
{
	this->teams.emplace_back(std::move(team_ptr));
}


// ================================
// Operator Overloads:


template<typename TLink>
bool sale::Country<TLink>::operator==(const sale::Country<TLink>& other) const
{
	static auto key_cmp_func = sale::Country<TLink>::KeyEqStruct();
	return key_cmp_func(this->name, other.name);
}


#endif
