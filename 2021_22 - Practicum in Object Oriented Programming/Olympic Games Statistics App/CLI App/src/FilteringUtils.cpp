#include "FilteringUtils.hpp"

#include "Sport.hpp"
#include "Event.hpp"



using db_oly_games_map = sale::utils::filters::db_oly_games_map;
using set_t = std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>;


void sale::utils::filters::filterGamesYear
	(std::unique_ptr<db_oly_games_map>& oly_games, const int year)
{
	const auto &db = sale::DataBase::getInstance();
	oly_games.reset(new db_oly_games_map{});

	if (db.olympic_games->count(year)) {
		oly_games->insert({ year, {} });
		// make deep copy of each olympic games ptr
		for (auto& games_ptr : db.olympic_games->at(year)) {
			oly_games->at(year).emplace_back
				(std::make_shared<sale::OlympicGames<sale::DataBase::getAthletePtr>> (*games_ptr));
		}
	}
}


void sale::utils::filters::filterGamesMedal
	(std::unique_ptr<db_oly_games_map>& oly_games,
	 const sale::Medal& medal_criteria,
	 bool only_individual_medals,
	 bool only_team_medals)
{
	const auto &db = sale::DataBase::getInstance();

	// reuse the database if oly_games_ptr is nullptr
	//if (!oly_games) {
	//	oly_games.reset(new db_oly_games_map(*db.olympic_games));
	//}

	// reuse the database if oly_games_ptr is nullptr
	if (!oly_games) {
		sale::utils::filters::copyOlympicGamesFromDatabase(oly_games);
	}


	const sale::Medal::TMedal medal_type = medal_criteria.getMedal();

	for (const auto& kv : *oly_games) {
		for (const auto games_ptr : kv.second) {
			const auto oly_games_id = games_ptr->getID();
			auto getAthletePtr = games_ptr->getAthletePtrFunc;

			// returns true ig medal of given type was won on these
			// games by given competitor
			auto does_have_medal = [&] (const sale::Competitor &competitor,
									    const sale::Medal::TMedal medal_t_crit) -> bool
			{
				for (const auto& medal : competitor.getMedals()) {
					if (medal.getOlympicGamesID() != oly_games_id) continue;
					if (only_individual_medals && medal.isSharedByTeam()) continue;
					if (only_team_medals && !medal.isSharedByTeam()) continue;
					if (medal.getMedal() == medal_t_crit) return true;
				}
				return false;
			};

			auto does_meet_criteria = [&] (const sale::Competitor &competitor) -> bool
			{
				if (medal_type != sale::Medal::TMedal::NoMedal) {
					return does_have_medal(competitor, medal_type);
				}

				// must not have a single medal on these games:
				return !does_have_medal(competitor, sale::Medal::TMedal::Gold)
					&& !does_have_medal(competitor, sale::Medal::TMedal::Silver)
					&& !does_have_medal(competitor, sale::Medal::TMedal::Bronze);
			};

			games_ptr->removeFromAthletesKeysCountries(
				[&] (const std::pair<std::remove_const<Athlete::TKey>::type,
									 std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>>
					 &athlete_country) -> bool
				{
					if (const auto athlete_ptr = getAthletePtr(athlete_country.first)) {
						return !does_meet_criteria(*athlete_ptr);
					}
					return true;  // remove if it does not exist in db
				}
			);

			// go through teams and remove those that do not
			// meet the criteria:
			games_ptr->removeFromTeamsCountries(
				[&] (const std::pair<std::shared_ptr<sale::Team<sale::DataBase::getAthletePtr>>,
								     std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>>
					&team_country)
				{
					return !does_meet_criteria(*team_country.first);
				}
			);
		}
	}

}


void sale::utils::filters::filterGamesSport
	(std::unique_ptr<db_oly_games_map>& oly_games, const std::string& sport_name)
{
	const auto &db = sale::DataBase::getInstance();

	const sale::Sport::TKey sport_key(new const std::string(sport_name));

	// sport does not exist, oly_games are empty then
	if (!db.sports->count(sport_key)) {
		oly_games.reset(new db_oly_games_map());
		return;
	}

	// reuse the database if oly_games_ptr is nullptr
	if (!oly_games) {
		sale::utils::filters::copyOlympicGamesFromDatabase(oly_games);
	}

	const auto &sport = *db.sports->at(sport_key);

	for (const auto& kv : *oly_games) {
		for (const auto games_ptr : kv.second) {
			const auto oly_games_id = games_ptr->getID();
			auto getAthletePtr = games_ptr->getAthletePtrFunc;

			auto does_meet_criteria = [&] (const sale::Competitor &competitor) -> bool
			{
				for (const auto& event_games_id : competitor.getEvents()) {
					const auto &event_ptr = event_games_id.first;

					// if this event does not exist in given sports skip it
					if (!sport.doesEventExist(event_ptr->getKey(), event_ptr->getType()))
						continue;

					for (const int id : event_games_id.second) {
						if (id == oly_games_id) return true;
					}
				}
				return false;
			};

			games_ptr->removeFromAthletesKeysCountries(
				[&] (const std::pair<std::remove_const<Athlete::TKey>::type,
									 std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>>
					&athlete_country) -> bool
				{
					if (const auto athlete_ptr = getAthletePtr(athlete_country.first)) {
						return !does_meet_criteria(*athlete_ptr);
					}
					return true;  // remove if it does not exist in db
				}
			);

			// go through teams and remove those that do not
			// meet the criteria:
			games_ptr->removeFromTeamsCountries(
				[&] (const std::pair<std::shared_ptr<sale::Team<sale::DataBase::getAthletePtr>>,
					std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>>
					&team_country)
				{
					return !does_meet_criteria(*team_country.first);
				}
			);
		}
	}

}


void sale::utils::filters::filterGamesCountry
	(std::unique_ptr<db_oly_games_map>& oly_games, const std::string& country)
{
	const auto &db = sale::DataBase::getInstance();

	const sale::Country<sale::DataBase::getAthletePtr>::TKey
		country_key(new const std::string(country));

	// sport does not exist, oly_games are empty then
	if (!db.countries->count(country_key)) {
		oly_games.reset(new db_oly_games_map());
		return;
	}

	// reuse the database if oly_games_ptr is nullptr
	if (!oly_games) {
		sale::utils::filters::copyOlympicGamesFromDatabase(oly_games);
	}

	auto are_keys_equal = sale::Country<sale::DataBase::getAthletePtr>::KeyEqStruct{};

	//const auto &country = *db.countries->at(country_key);

	for (const auto& kv : *oly_games) {
		for (const auto games_ptr : kv.second) {
			const auto oly_games_id = games_ptr->getID();

			games_ptr->removeFromAthletesKeysCountries(
				[&] (const std::pair<std::remove_const<Athlete::TKey>::type,
									 std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>>
					&athlete_country) -> bool
				{
					return !are_keys_equal(athlete_country.second->getKey(), country_key);
				}
			);

			// go through teams and remove those that do not
			// meet the criteria:
			games_ptr->removeFromTeamsCountries(
				[&] (const std::pair<std::shared_ptr<sale::Team<sale::DataBase::getAthletePtr>>,
									 std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>>
					 &team_country)
				{
					return !are_keys_equal(team_country.second->getKey(), country_key);
				}
			);
		}
	}

}


void sale::utils::filters::filterGamesEventType
	(std::unique_ptr<db_oly_games_map>& oly_games, const std::string& event_type_str)
{
	const auto &db = sale::DataBase::getInstance();

	const auto event_type = std::strcmp(event_type_str.c_str(), "Team") == 0 ?
							sale::Event::TEventType::Team
						  : sale::Event::TEventType::Individual;

	// reuse the database if oly_games_ptr is nullptr
	if (!oly_games) {
		sale::utils::filters::copyOlympicGamesFromDatabase(oly_games);
	}

	for (const auto& kv : *oly_games) {
		for (const auto games_ptr : kv.second) {
			if (event_type == sale::Event::TEventType::Individual) {
				// remove all records of teams
				games_ptr->dropTeamsCountries();
			}
			else {  // remove all records of individuals
				games_ptr->dropAthletesKeysCountries();
			}
		}
	}
}


void sale::utils::filters::copyOlympicGamesFromDatabase
	(std::unique_ptr<db_oly_games_map>& oly_games)
{
	const auto &db = sale::DataBase::getInstance();

	oly_games.reset(new db_oly_games_map(*db.olympic_games));

	// make deep copy of olympic games
	for (auto& kv : *oly_games) {
		for (auto it = kv.second.begin(); it != kv.second.end(); ++it) {
			it->reset(new sale::OlympicGames<sale::DataBase::getAthletePtr>(**it));
		}
	}
}
