#include "BasicMetricsMenu.hpp"

#include "MenuExceptions.hpp"
#include <algorithm>  // std::for_each
#include "App.hpp"  // sale::App::quit
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <iomanip>  // std::setprecision
#include "DataBase.hpp"
#include <memory>
#include "FilteringUtils.hpp"
#include "Event.hpp"


typedef std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>
		set_t;

#define events_map_t std::unordered_multimap< \
						sale::Event::TKey, \
						std::shared_ptr<sale::Event>, \
						sale::Event::KeyHashStruct, \
						sale::Event::KeyEqStruct \
					 >


// ================================
// Static Fields Initializations:


const std::function<std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)>
sale::BasicMetricsMenu::getAthletePtrFunc = sale::DataBase::getAthletePtr{};


// ================================
// Constructors:


sale::BasicMetricsMenu::BasicMetricsMenu()
{
}


// ================================
// Menu and Submenus:


void sale::BasicMetricsMenu::init()
{
	this->addOption(
		"Change filters",
		[&] () { this->openFiltersMenu(); }
	);

	this->addOption(
		"Number of athletes",
		[&] () { this->displayNumberOfAthletes(); }
	);

	this->addOption(
		"Number of sport disciplines",
		[&] () { this->displayNumberOfSportDisciplines(); }
	);

	this->addOption(
		"Average athlete height",
		[&] () { this->displayAverageAthleteHeight(); }
	);

	this->addOption(
		"Average athlete weight",
		[&] () { this->displayAverageAthleteWeight(); }
	);
}

void sale::BasicMetricsMenu::updateMenuMsg()
{
	if (this->options.empty()) {
		this->menu_msg = "The main menu offers no options at this time.";
		return;
	}

	this->menu_msg = "Choose one of the following options:\n";
	this->menu_msg += "( Currently set filters: ";

	// add active filters to message if any:

	int old_strlen = this->menu_msg.length();
	bool is_first = true;  // do not end logging with comma
	for (auto const &kv : this->filters) {
		if (kv.second.length()) {
			if (!is_first) this->menu_msg += ", ";
			else is_first = false;
			this->menu_msg += kv.first + ": " + kv.second;
		}
	}

	// if no filters were set, notify user
	if (this->menu_msg.length() == old_strlen) {
		this->menu_msg += "No filters set ";
	}

	this->menu_msg += " )\n";
}

void sale::BasicMetricsMenu::openFiltersMenu()
{
	sale::OptionsMenu filters_menu;

	// app's forever loop will display basic metrics menu
	// again
	filters_menu.addOption("Go Back", [] () {});

	filters_menu.addOption(
		"Add filter to current combination",
		[&] () { this->openAddSingleFilterMenu(); }
	);

	filters_menu.addOption(
		"Remove all filters",
		[&] () {
			for (auto &kv : this->filters) kv.second = "";
			this->updateMenuMsg();
		}
	);

	std::cout << filters_menu;
	filters_menu.selectOption();
}

void sale::BasicMetricsMenu::openAddSingleFilterMenu()
{
	sale::OptionsMenu menu;

	auto readline = [] () -> std::string {
		std::string user_input = "";
		std::cin.sync();
		std::cin.ignore();
		std::getline(std::cin, user_input);
		return user_input;
	};

	menu.addOption(
		"Sport",
		[&] () {
			std::cout << "Enter the sport's name:" << std::endl;
			this->filters["Sport"] = readline();
		}
	);

	menu.addOption(
		"Country",
		[&] () {
			std::cout << "Enter the country's name:" << std::endl;
			this->filters["Country"] = readline();
		}
	);

	menu.addOption(
		"Year",
		[&] () {
			std::cout << "Enter the year:" << std::endl;
			this->filters["Year"] = readline();
		}
	);

	menu.addOption(
		"Event type",
		[&] () {
			std::cout << "Enter the event type (Team/Individual):" << std::endl;
			this->filters["Event Type"] = readline();
		}
	);

	menu.addOption(
		"Medal",
		[&] () {
			std::cout << "Enter the medal:" << std::endl;
			this->filters["Medal"] = readline();
		}
	);

	// display menu and wait for user's choice
	std::cout << menu;
	menu.selectOption();

	this->updateMenuMsg();
}


// ================================
// App Functionalities:


inline void sale::BasicMetricsMenu::displayNumberOfAthletes()
{
	int num_athletes = this->getNumAthletes();
	std::cout << "Number of athletes: " << num_athletes << std::endl;
}

inline void sale::BasicMetricsMenu::displayNumberOfSportDisciplines()
{
	int num_disciplines = this->getNumSportDisciplines();
	std::cout << "Number of sport disciplines: "
			  << num_disciplines << std::endl;
}

inline void sale::BasicMetricsMenu::displayAverageAthleteHeight()
{
	float avg_height = this->getAvgAthleteHeight();
	std::cout << "Average athlete height: "
			  << std::fixed << std::setprecision(4)
			  << avg_height << std::endl;
}

inline void sale::BasicMetricsMenu::displayAverageAthleteWeight()
{
	float avg_weight = this->getAvgAthleteWeight();
	std::cout << "Average athlete weight: "
			  << std::fixed << std::setprecision(4)
			  << avg_weight << std::endl;
}


// ================================
// Filtering Athletes:


int sale::BasicMetricsMenu::getNumAthletes() const
{
	std::unique_ptr<set_t> athletes(nullptr);
	this->filterAthletes(athletes);

	// at least one filter was applied
	if (athletes) {
		const int num_athletes = athletes->size();
		athletes.reset(nullptr);
		return num_athletes;
	}

	// if no filters have been applied just use
	// the full databse
	const auto &db = sale::DataBase::getInstance();
	// when loading in data athletes were not loaded in
	// unless they participated in some olympic games
	// so just return number of athletes in db
	return db.athletes->size();
	
	//athletes.reset(new set_t());

	//for (const auto& kv : *db.olympic_games) {
	//	for (const auto& games_ptr : kv.second) {
	//		// add all individuals
	//		for (const auto& athlete_country : games_ptr->getAthletesKeysCountries()) {
	//			athletes->insert(athlete_country.first);
	//		}
	//		// add all team's individuals
	//		for (const auto& team_country : games_ptr->getTeamsCountries()) {
	//			for (const auto athlete_key : team_country.first->getAthletesKeys()) {
	//				athletes->insert(athlete_key);
	//			}
	//		}
	//	}
	//}

	//const int num_athletes = athletes->size();
	//athletes.reset(nullptr);
	//return num_athletes;
}

float sale::BasicMetricsMenu::getAvgAthleteHeight() const
{
	int height_sum = 0;
	int athletes_count = 0;

	auto process_height = [&] (const int height) {
		// skip athletes with unedfined height
		if (height >= 0) {
			height_sum += height;
			++athletes_count;
		}
	};

	std::unique_ptr<set_t> athletes(nullptr);
	this->filterAthletes(athletes);

	if (athletes) {
		// 1 or more filters have been applied
		std::for_each(
			athletes->begin(),
			athletes->end(),
			[&] (sale::Athlete::TKey athlete_key) {
				int height = this->getAthletePtrFunc(athlete_key)->getHeight();
				process_height(height);
			}
		);
		athletes.reset(nullptr);
	}
	else {  // no filters have been applied so reuse the database
		const auto &db = sale::DataBase::getInstance();
		std::for_each(
			db.athletes->begin(),
			db.athletes->end(),
			[&] (const std::pair< const sale::Athlete::TKey,
								  std::shared_ptr<sale::Athlete> > &kv)
			{
				process_height(kv.second->getHeight());
			}
		);
	}

	// return avg height
	// avoid division by zero
	if (athletes_count == 0) return 0.;
	return 1. * height_sum / athletes_count;
}

float sale::BasicMetricsMenu::getAvgAthleteWeight() const
{
	double weight_sum = 0.0;
	int athletes_count = 0;

	auto process_weight = [&] (const float weight) {
		// skip athletes with unedfined weight
		if (weight >= 0) {
			weight_sum += weight;
			++athletes_count;
		}
	};

	std::unique_ptr<set_t> athletes(nullptr);
	this->filterAthletes(athletes);

	if (athletes) {
		// 1 or more filters have been applied
		std::for_each(
			athletes->begin(),
			athletes->end(),
			[&] (sale::Athlete::TKey athlete_key) {
				float weight = this->getAthletePtrFunc(athlete_key)->getWeight();
				process_weight(weight);
			}
		);
		athletes.reset(nullptr);
	}
	else {  // no filters have been applied so reuse the database
		const auto &db = sale::DataBase::getInstance();
		std::for_each(
			db.athletes->begin(),
			db.athletes->end(),
			[&] (const std::pair< sale::Athlete::TKey,
								  std::shared_ptr<sale::Athlete> > &kv)
			{
				process_weight(kv.second->getWeight());
			}
		);
	}

	// return avg weight
	// avoid division by zero
	if (athletes_count == 0) return 0.;
	return 1. * weight_sum / athletes_count;
}

void sale::BasicMetricsMenu::filterAthletes(std::unique_ptr<set_t> &athletes) const
{
	using namespace sale::utils;
	

	std::unique_ptr<
		std::map<
			int,  // key - year
			// data - group of games:
			std::vector<std::shared_ptr<OlympicGames<sale::DataBase::getAthletePtr>>>
		>
	> filtered_olympic_games;

	// apply first the filters that will likely narrow
	// the data down the most:

	if (this->filters.at("Year").length()) {
		filters::filterGamesYear
			(filtered_olympic_games, std::atoi(this->filters.at("Year").c_str()));
	}

	if (this->filters.at("Event Type").length()) {
		filters::filterGamesEventType(filtered_olympic_games, this->filters.at("Event Type"));
	}

	if (this->filters.at("Country").length()) {
		filters::filterGamesCountry(filtered_olympic_games, this->filters.at("Country"));
	}

	if (this->filters.at("Sport").length()) {
		filters::filterGamesSport(filtered_olympic_games, this->filters.at("Sport"));
	}

	if (this->filters.at("Medal").length()) {
		sale::Medal medal_criterion(this->filters.at("Medal"), -1, nullptr, nullptr);

		if (this->filters.at("Event Type") == "")
			filters::filterGamesMedal(filtered_olympic_games, medal_criterion);
		else if (this->filters.at("Event Type") == "Team")
			filters::filterGamesMedal(filtered_olympic_games, medal_criterion, false, true);
		else  // only individual event type
			filters::filterGamesMedal(filtered_olympic_games, medal_criterion, true, false);
	}

	if (!filtered_olympic_games) {  // no filters were applied
		return;
	}

	// extract the athletes from filtered olympics and unite them all
	set_t athletes_keys;

	for (const auto& kv : *filtered_olympic_games) {
		for (const auto& games_ptr : kv.second) {
			auto athletes_keys_game = games_ptr->getAllAthletesKeys();
			athletes_keys.insert(athletes_keys_game.begin(), athletes_keys_game.end());
		}
	}

	athletes.reset(new set_t(athletes_keys));
	filtered_olympic_games.reset(nullptr);
}


// ================================
// Filtering Events:


int sale::BasicMetricsMenu::getNumSportDisciplines() const
{
	// filter all the athletes
	const auto &db = sale::DataBase::getInstance();
	
	std::unique_ptr<events_map_t> events(nullptr);
	this->filterEvents(events);

	if (events) {  // at least one filter was applied
		const int num_events = events->size();
		events.reset(nullptr);
		return num_events;
	}

	// if no filters have been applied just use
	// the full database

	int num_disciplines = 0;

	// sum up all different events for all different sports in db
	for (const auto& kv : *db.sports) {
		const auto &sport_ptr = kv.second;
		num_disciplines += sport_ptr->getEvents().size();
	}

	return num_disciplines;
}

void sale::BasicMetricsMenu::filterEvents(std::unique_ptr<events_map_t>& events) const
{
	using namespace sale::utils;
	
	const auto &db = sale::DataBase::getInstance();

	std::unique_ptr<
		std::map<
			int,  // key - year
			// data - group of games:
			std::vector<std::shared_ptr<OlympicGames<sale::DataBase::getAthletePtr>>>
		>
	> filtered_olympic_games;

	// multimap of events that remain after filtering
	std::unique_ptr<events_map_t> filtered_events(new events_map_t());

	// set sport filter if any
	std::shared_ptr<sale::Sport> sport(nullptr);

	if (this->filters.at("Sport").length()) {  // should set sport filter
		std::shared_ptr<const std::string> sport_key
			(new const std::string(this->filters.at("Sport")));

		if (!db.sports->count(sport_key)) {
			// sport does not exist, filtered events are empty then
			events = std::move(filtered_events);
			return;
		}

		sport = db.sports->at(sport_key);
	}
	
	bool do_filter_by_event_type = this->filters.at("Event Type").length() != 0;

	const auto event_type = std::strcmp(this->filters.at("Event Type").c_str(), "Team") == 0 ?
							sale::Event::TEventType::Team
						  : sale::Event::TEventType::Individual;

	bool do_filter_by_medal = this->filters.at("Medal").length() != 0;

	const sale::Medal::TMedal medal_criterion
		= sale::Medal(this->filters.at("Medal"), -1, nullptr, nullptr).getMedal();
	
	bool do_filter_by_country = this->filters.at("Country").length() != 0;

	// Filter by year and country:

	if (this->filters.at("Year").length()) {
		filters::filterGamesYear
			(filtered_olympic_games, std::atoi(this->filters.at("Year").c_str()));
	}

	if (this->filters.at("Country").length()) {
		filters::filterGamesCountry(filtered_olympic_games, this->filters.at("Country"));
	}

	// no filters above have been applied
	if (!filtered_olympic_games) {
		if (!sport && !do_filter_by_event_type && this->filters.at("Medal").length() == 0)
			return;  // no more filters are to be applied:

		// copy the database in order to apply the rest of filters
		filters::copyOlympicGamesFromDatabase(filtered_olympic_games);
	}

	// Filter by sport, event type and medal:

	const auto do_event_keys_match = sale::Event::KeyEqStruct{};

	auto does_event_exist
		= [] (const events_map_t &haystack,
			  const sale::Event::TKey &key,
			  const sale::Event::TEventType event_type) -> bool
	{
		auto keys_range = haystack.equal_range(key);
		for (auto iter = keys_range.first, end = keys_range.second;
			iter != end;
			++iter)
		{
			if (iter->second->getType() == event_type) return true;
		}
		return false;
	};

	for (const auto& kv : *filtered_olympic_games) {
		for (const auto games_ptr : kv.second) {
			const auto oly_games_id = games_ptr->getID();
			const auto getAthletePtr = games_ptr->getAthletePtrFunc;

			auto does_event_meet_criteria = [&] (const sale::Event &event_) -> bool
			{
				// filter the event by type
				if (do_filter_by_event_type && event_.getType() != event_type)
					return false;

				// if this event has already been stored skip it
				if (does_event_exist(*filtered_events,
									 event_.getKey(), event_.getType()))
					return false;

				// if there is a sport filter on and event does not match it continue
				if (sport && !sport->doesEventExist(event_.getKey(), event_.getType()))
					return false;

				return true;
			};

			// returns true if medal meeting all criteria was won for given event
			// by given competitor
			auto event_has_medal =
				[&] (const sale::Competitor &competitor, const sale::Event &event_) -> bool
			{
				for (const auto& medal : competitor.getMedals()) {
					if (medal.getOlympicGamesID() != oly_games_id) continue;
					
					if (do_filter_by_event_type) {
						if ( event_type == sale::Event::TEventType::Individual
						  && medal.isSharedByTeam())
							continue;
						if ( event_type == sale::Event::TEventType::Team
						  && !medal.isSharedByTeam())
							continue;
					}

					if ( do_filter_by_country
					  && medal.getCountryNameReference() != this->filters.at("Country"))
						continue;

					const auto medals_event = medal.getEvent();

					// check if it mees sport criteria
					if ( sport
					  && !sport->doesEventExist(medals_event->getKey(), medals_event->getType()))
						continue;

					// if medal meets all other criteria compare against event
					if (!(*medals_event == event_)) return false;

					// !important apply filter for the medal now, after all other filters:
					if (medal_criterion == sale::Medal::TMedal::NoMedal) return false;

					if (medal_criterion == medal.getMedal()) {
						return true;
					}
				}

				return false;
			};

			auto process_competitor = [&] (const sale::Competitor &competitor) {
				for (const auto& event_games_id : competitor.getEvents()) {
					auto event_ptr = event_games_id.first;

					if (!does_event_meet_criteria(*event_ptr)) continue;

					// check if competitor competed on this event on cur games
					for (const auto id : event_games_id.second) {
						if (id == oly_games_id) {  // cur event was played out on cur games:
							if ( !events
							 ||  does_event_exist(*events, event_ptr->getKey(),
												  event_ptr->getType()) )
							{
								if ( do_filter_by_medal
								  && !event_has_medal(competitor, *event_ptr) )
								{
									break;
								}

								filtered_events->insert({ event_ptr->getKey(), std::move(event_ptr) });
							}
							break;
						}
					}
				}
			};

			// process individuals:
			if (!do_filter_by_event_type || event_type == sale::Event::TEventType::Individual) {
				for (const auto& athlete_country : games_ptr->getAthletesKeysCountries()) {
					if (const auto athlete_ptr = getAthletePtr(athlete_country.first)) {
						process_competitor(*athlete_ptr);
					}
				}
			}
			
			// process teams
			if (!do_filter_by_event_type || event_type == sale::Event::TEventType::Team) {
				for (const auto& team_country : games_ptr->getTeamsCountries()) {
					process_competitor(*team_country.first);
				}
			}
		}
	}

	events = std::move(filtered_events);
	filtered_olympic_games.reset(nullptr);
}
