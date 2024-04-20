#include "Event.hpp"


// ================================
// Constructors:


sale::Event::Event(const sale::Event::TEventType event_type,
                   const std::string &name, const int oly_games_id)
    : event_type(event_type), name(new const std::string(name)),
      oly_games_id(oly_games_id)
{
}

sale::Event::Event(const TEventType event_type,
                   std::shared_ptr<const std::string> name_ptr,
                   const int oly_games_id)
    : event_type(event_type), name(std::move(name_ptr)),
      oly_games_id(oly_games_id)
{
}


// ================================
// HashableData overrides:


sale::Event::TKey sale::Event::getKey() const
{
    return this->name;
}


// ================================
// Getters:


sale::Event::TEventType sale::Event::getType() const
{
    return this->event_type;
}

std::string sale::Event::getName() const
{
    return *this->name;
}

const std::string& sale::Event::getNameReference() const
{
    return *this->name;
}

const int sale::Event::getOlympicGamesID() const
{
    return this->oly_games_id;
}


// ================================
// Operator Overloads:


bool operator==(const sale::Event& x, const sale::Event& y)
{
    if (x.event_type != y.event_type) return false;
    static auto key_cmp_func = sale::Event::KeyEqStruct();
    return key_cmp_func(x.name, y.name);
}
