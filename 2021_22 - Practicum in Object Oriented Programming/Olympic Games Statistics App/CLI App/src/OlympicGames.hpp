#ifndef OLYMPIC_GAMES_HPP
#define OLYMPIC_GAMES_HPP


#include "Athlete.hpp"
#include "Team.hpp"
#include <string>
#include <memory>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <functional>  // std::remove_const
#include "CustomException.hpp"


namespace sale {


template<typename TLink>
class Country;

//template<typename TLink>
//class Team;


template<typename TLink>
class OlympicGames
{

 public:

    // Constructors and Destructor:

    // type is e.g: Summer, Winter
    OlympicGames(const int year, const std::string &type, const std::string &city);
    OlympicGames(const OlympicGames &);


    // Getters:

    int getID() const;

    int getYear() const;

    std::string getCity() const;
    const std::string& getCityReference() const;

    std::string getType() const;
    const std::string& getTypeReference() const;

    const auto& getAthletesKeysCountries() const;
    const auto& getTeamsCountries() const;

    // returns ptr to country that athlete represented on these games
    std::shared_ptr<sale::Country<TLink>> getAthletesCountry(sale::Athlete::TKey key) const;

    std::unordered_set<std::remove_const<sale::Athlete::TKey>::type> getAllAthletesKeys() const;


    // Adding Competitors:

    // adds given athlete representing given country individually
    // if no such athlete represents any other country individually
    // on these games, returns bool if athlete was added
    bool addAthlete(sale::Athlete::TKey key,
                    std::shared_ptr<sale::Country<TLink>> country);

    void addTeam(std::shared_ptr<sale::Team<TLink>> team_ptr,
                 std::shared_ptr<sale::Country<TLink>> country);


    // Removing competitors:

    int removeFromAthletesKeysCountries
        (const std::function< bool (const std::pair<std::remove_const<Athlete::TKey>::type,
                                                   std::shared_ptr<sale::Country<TLink>>> &) >
         &criterion);

    int removeFromTeamsCountries
        (const std::function<bool (const std::pair<std::shared_ptr<sale::Team<TLink>>,
                                                   std::shared_ptr<sale::Country<TLink>>> &) >
         &criterion);

    // removes all teams records from current games
    void dropTeamsCountries();
    
    // removes all individuals records from current games
    void dropAthletesKeysCountries();


    // Operator Overloads:

    bool operator== (const OlympicGames &other) const;

    // function connecting Athlete ptrs with keys stored locally
    static const std::function<
        std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)
    > getAthletePtrFunc;


 private:
    
    static int ID;
    const int id = ++ID;

    // Attributes:
     
    const int year;
    const std::string type;
    const std::string city;

    // map of athletes keys and countries they represented
    std::unique_ptr<
        std::unordered_map<
            std::remove_const<Athlete::TKey>::type,
            std::shared_ptr<sale::Country<TLink>>
        >
    > athletes = nullptr;

    // list of teams and countries they competed for on these games
    std::list<std::pair<std::shared_ptr<sale::Team<TLink>>,
                        std::shared_ptr<sale::Country<TLink>>>> teams;

};


}  // end of sale namespace



// ================================
// ================================
// Implementation:


// ================================
// Static Fields Initializations:


template<typename TLink>
int sale::OlympicGames<TLink>::ID = 0;


template<typename TLink>
const std::function<std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)>
sale::OlympicGames<TLink>::getAthletePtrFunc = TLink{};


// ================================
// Constructors:


template<typename TLink>
sale::OlympicGames<TLink>::OlympicGames(const int year,
                                        const std::string &type,
                                        const std::string &city)
    : year(year), type(type), city(city),
      athletes(new std::unordered_map< std::remove_const<Athlete::TKey>::type,
                                       std::shared_ptr<sale::Country<TLink>> >())
{
}

template<typename TLink>
sale::OlympicGames<TLink>::OlympicGames(const OlympicGames &other)
    : id(other.id), year(other.year), type(other.type), city(other.city),
      teams(other.teams)
{
    if (other.athletes) {
        athletes.reset(
            new std::unordered_map<
                    std::remove_const<Athlete::TKey>::type,
                    std::shared_ptr<sale::Country<TLink>>
            > (*other.athletes)
        );
    }
}


// ================================
// Getters:


template<typename TLink>
int sale::OlympicGames<TLink>::getID() const
{
    return this->id;
}

template<typename TLink>
int sale::OlympicGames<TLink>::getYear() const
{
    return this->year;
}

template<typename TLink>
std::string sale::OlympicGames<TLink>::getCity() const
{
    return this->city;
}

template<typename TLink>
const std::string& sale::OlympicGames<TLink>::getCityReference() const
{
    return this->city;
}

template<typename TLink>
std::string sale::OlympicGames<TLink>::getType() const
{
    return this->type;
}

template<typename TLink>
const std::string& sale::OlympicGames<TLink>::getTypeReference() const
{
    return this->type;
}

template<typename TLink>
const auto& sale::OlympicGames<TLink>::getAthletesKeysCountries() const
{
    if (!this->athletes) {  // return empty set
        throw sale::exceptions::CustomException("Athletes Countries pairs map is nullptr");
    }
    return *this->athletes;
}

template<typename TLink>
const auto& sale::OlympicGames<TLink>::getTeamsCountries() const
{
    return this->teams;
}

template<typename TLink>
std::shared_ptr<sale::Country<TLink>>
    sale::OlympicGames<TLink>::getAthletesCountry(sale::Athlete::TKey key) const
{
    if (this->athletes->count(key)) return this->athletes->at(key);
    return nullptr;
}

template<typename TLink>
std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>
    sale::OlympicGames<TLink>::getAllAthletesKeys() const
{
    std::unordered_set<std::remove_const<sale::Athlete::TKey>::type> athlete_keys;

    for (const auto &athlete_country : *this->athletes) {
        athlete_keys.insert(athlete_country.first);
    }

    for (const auto &team_country : this->teams) {
        for (const auto athlete_key : team_country.first->getAthletesKeys()) {
            athlete_keys.insert(athlete_key);
        }
    }

    return athlete_keys;
}


// ================================
// Managing Competitors:


template<typename TLink>
bool sale::OlympicGames<TLink>::addAthlete
    (sale::Athlete::TKey key, 
     std::shared_ptr<sale::Country<TLink>> country)
{
    if (this->athletes->count(key)) return false;
    this->athletes->insert({ key, std::move(country) });
    return true;
}

template<typename TLink>
void sale::OlympicGames<TLink>::addTeam
    (std::shared_ptr<sale::Team<TLink>> team_ptr,
     std::shared_ptr<sale::Country<TLink>> country)

{
    this->teams.push_back({ std::move(team_ptr), std::move(country) });
}

template<typename TLink>
int sale::OlympicGames<TLink>::removeFromAthletesKeysCountries
(const std::function<bool (const std::pair<std::remove_const<Athlete::TKey>::type,
                                          std::shared_ptr<sale::Country<TLink>>> &)> &criterion)
{
    int old_size = this->athletes->size();

    for (auto it = this->athletes->cbegin(); it != this->athletes->cend(); ) {
        if (criterion(*it)) this->athletes->erase(it++);
        else ++it;
    }
    
    int num_deleted = this->athletes->size() - old_size;
    return num_deleted;
}

template<typename TLink>
int sale::OlympicGames<TLink>::removeFromTeamsCountries
    (const std::function<bool(const std::pair<std::shared_ptr<sale::Team<TLink>>,
                                              std::shared_ptr<sale::Country<TLink>>>&)>
     & criterion)
{
    int old_size = this->teams.size();

    this->teams.remove_if(
        [&] (const std::pair<std::shared_ptr<sale::Team<TLink>>,
                             std::shared_ptr<sale::Country<TLink>>>
             &team_country) -> bool
        {
            return criterion(team_country);
        }
    );

    int num_deleted = this->teams.size() - old_size;
    return num_deleted;
}

template<typename TLink>
void sale::OlympicGames<TLink>::dropTeamsCountries()
{
    this->teams = {};
}

template<typename TLink>
void sale::OlympicGames<TLink>::dropAthletesKeysCountries()
{
    *this->athletes = {};
}


// ================================
// Operator Overloads:


template<typename TLink>
bool sale::OlympicGames<TLink>::operator==(const sale::OlympicGames<TLink>& other) const
{
    if (this->year != other.year) return false;
    return this->type == other.type;
}

#endif
