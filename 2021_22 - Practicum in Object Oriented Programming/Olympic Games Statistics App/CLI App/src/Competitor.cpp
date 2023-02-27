#include "Competitor.hpp"

#include <utility>  // std::move
#include <algorithm>  // std::for_each


// ================================
// Constructors:


sale::Competitor::Competitor(const int max_num_events)
{
	this->events.reserve(max_num_events);
}


// ================================
// Public Methods:


void sale::Competitor::addEvent(std::shared_ptr<sale::Event> to_add, const int oly_games_id)
{
	// find given event in the vector
	const sale::Event &event_ = *to_add;
	auto found = std::find_if(this->events.begin(), this->events.end(),
		[&] (const std::pair<std::shared_ptr<sale::Event>, std::vector<int>> &el) {
			return event_ == *el.first;
		}
	);
	
	// if event does not exist then add it
	if (found == this->events.end())
		this->events.push_back({std::move(to_add), {oly_games_id}});
	else
		found->second.push_back(oly_games_id);
}

void sale::Competitor::shrinkToFit()
{
	this->events.shrink_to_fit();
	this->medals.shrink_to_fit();
}

const std::vector<sale::Medal>& sale::Competitor::getMedals() const
{
	return this->medals;
}

const std::vector<std::pair<std::shared_ptr<sale::Event>, std::vector<int>>>&
	sale::Competitor::getEvents() const
{
	return this->events;
}

void sale::Competitor::addMedal(const Medal& medal)
{
	if (medal.getMedal() == sale::Medal::TMedal::NoMedal) return;
	this->medals.emplace_back(medal);
}
