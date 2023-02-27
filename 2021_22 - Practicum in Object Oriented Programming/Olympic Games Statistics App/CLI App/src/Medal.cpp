#include "Medal.hpp"

#include <string>  // std::strcmp


// ================================
// Constructors and Destructor:


sale::Medal::Medal(const TMedal& medal, const int olympic_games_id,
				   std::shared_ptr<const std::string> country_name,
				   std::shared_ptr<sale::Event> event_)
	: medal(medal), olympic_games_id(olympic_games_id),
	  country_name(std::move(country_name)), event_(std::move(event_))
{
}

sale::Medal::Medal(const std::string& medal_str, const int olympic_games_id,
				   std::shared_ptr<const std::string> country_name,
				   std::shared_ptr<sale::Event> event_)
	: Medal(medal_str.c_str(), olympic_games_id,
			std::move(country_name), std::move(event_))
{
}

sale::Medal::Medal(const char* const medal_str, const int olympic_games_id,
				   std::shared_ptr<const std::string> country_name,
				   std::shared_ptr<sale::Event> event_)
	: olympic_games_id(olympic_games_id), country_name(std::move(country_name)),
	  event_(std::move(event_))
{
	if (std::strcmp(medal_str, "") == 0) {
		this->medal = sale::Medal::TMedal::NoMedal;
	}
	else if (std::strcmp(medal_str, "Gold") == 0) {
		this->medal = sale::Medal::TMedal::Gold;
	}
	else if (std::strcmp(medal_str, "Silver") == 0) {
		this->medal = sale::Medal::TMedal::Silver;
	}
	else if (std::strcmp(medal_str, "Bronze") == 0) {
		this->medal = sale::Medal::TMedal::Bronze;
	}
	else if (std::strcmp(medal_str, "No Medal") == 0) {
		this->medal = sale::Medal::TMedal::NoMedal;
	}
	else {
		throw InvalidMedalStrFormat();
	}
}


// ================================
// Operator Overloads:


bool sale::Medal::operator==(const Medal& other) const
{
	return this->medal == other.medal;
}

bool sale::Medal::operator!=(const Medal& other) const
{
	return !(*this == other);
}

bool sale::Medal::operator>(const Medal& other) const
{
	return this->medal < other.medal;
}

bool sale::Medal::operator<(const Medal& other) const
{
	return this->medal > other.medal;
}

bool sale::Medal::operator>=(const Medal& other) const
{
	return !(*this < other);
}

bool sale::Medal::operator<=(const Medal& other) const
{
	return !(*this > other);
}

int sale::Medal::getMedalScore() const
{
	return (int) sale::Medal::TMedal::NoMedal - (int) this->medal;
}

int sale::Medal::getOlympicGamesID() const
{
	return this->olympic_games_id;
}

sale::Medal::TMedal sale::Medal::getMedal() const
{
	return this->medal;
}

bool sale::Medal::isSharedByTeam() const
{
	return this->is_shared_by_team;
}

const std::string& sale::Medal::getCountryNameReference() const
{
	return *this->country_name;
}

std::shared_ptr<const std::string> sale::Medal::getCountryName() const
{
	return this->country_name;
}

std::shared_ptr<sale::Event> sale::Medal::getEvent() const
{
	return this->event_;
}

void sale::Medal::setAsSharedByTeam()
{
	this->is_shared_by_team = true;
}
