#include "App.hpp"

#include "OptionsMenu.hpp"
#include "BasicMetricsMenu.hpp"
#include <unordered_set>
#include <set>
#include <tuple>
#include <algorithm>  // std::for_each
#include <map>
#include "FilteringUtils.hpp"
#include <list>
#include <unordered_map>
#include "Team.hpp"
#include "App.hpp"
#include <vector>


#define db_country_t sale::Country<sale::DataBase::getAthletePtr>
#define db_oly_games_t sale::OlympicGames<sale::DataBase::getAthletePtr>



// ================================
// Constructors:


sale::App::App()
{
    this->openStartupMenu();
}


// ================================
// Public Methods:


void sale::App::forEverLoop()
{
	while (true) {
        try {
            sale::Menu &active_menu = this->fetchActiveMenu();

            // display main window and let user
            // to select an option from it:
            std::cout << active_menu;
            active_menu.selectOption();
        }
        catch (const std::exception& err) {
            std::cerr << std::endl << "There was an error:"
                      << std::endl << err.what()
                      << std::endl << std::endl;
        }
	}
}


// ================================
// Static Helpers Functions:


std::string sale::App::readline(bool do_flush)
{
    std::string user_input = "";
    if (do_flush) {
        std::cin.sync();
        std::cin.ignore();
    }
    std::getline(std::cin, user_input);
    return user_input;
}

auto sale::App::readOlympicGamesFromUser(bool do_flush_first)
{
    if (do_flush_first) {
        std::cin.sync();
        std::cin.ignore();
    }

    const auto &db = sale::DataBase::getInstance();

    std::cout << "Enter olympic games:" << std::endl << "year:" << std::endl;
    int year = -1;
    std::cin >> year;

    std::cout << "type (Summer/Winter):" << std::endl;
    std::string games_type;
    std::cin >> games_type;
    std::cout << std::endl;

    const auto oly_games = db.getGamePtr(year, games_type.c_str());

    if (!oly_games) {
        std::cout << "These games do not exist in the database." << std::endl;
        std::cout << "Please try different one." << std::endl;
        return oly_games;
    }

    return oly_games;
}


// ================================
// Menus with Options:


sale::Menu& sale::App::fetchActiveMenu()
{
    return *(this->open_windows.top());
}

void sale::App::openStartupMenu()
{
    std::unique_ptr<sale::OptionsMenu> startup_menu(new sale::OptionsMenu);

    startup_menu->addOption("Quit", App::quit);

    startup_menu->addOption(
        "Load in all events and athletes data",
        [&] () {
            this->loadInOlympicGamesData(-1);
            this->openCompleteMenu();
        }
    );

    startup_menu->addOption(
        "Load in events and athletes data from specific year",
        [&] () {
            int year = -1;
            std::cout << "Enter the year: ";
            std::cin >> year;
            this->loadInOlympicGamesData(year);
            this->openCompleteMenu();
        }
    );
    
    this->open_windows.push(std::move(startup_menu));
}

void sale::App::openCompleteMenu()
{
    std::unique_ptr<sale::OptionsMenu> complete_menu(new sale::OptionsMenu);

    complete_menu->addOption("Quit", App::quit);

    complete_menu->addOption(
        "Load in all events and athletes data",
        [&] () { this->loadInOlympicGamesData(-1); }
    );

    complete_menu->addOption(
        "Load in events and athletes data from specific year",
        [&] () {
            int year = -1;
            std::cout << "Enter the year: ";
            std::cin >> year;
            this->loadInOlympicGamesData(year);
        }
    );

    complete_menu->addOption(
        "Clear all events and athletes data",
        [&] () {
            sale::DataBase &db = sale::DataBase::getInstance();
            db.clearInstance();
            this->closeWindow();  // go bak to startup menu
        }
    );

    complete_menu->addOption(
        "View basic metrics",
        [&] () { this->openBasicMetricsMenu(); }
    );

    complete_menu->addOption(
        "Find number of sports that given country won at least 1 medal in",
        [&] () { this->displayNumSportsCountryMedal(); }
    );

    complete_menu->addOption(
        "Find 3 winning countries on given olympic games",
        [&] () { this->displayOlympicGamesTop3(); }
    );

    complete_menu->addOption(
        "Find countries that won on at least one olympic games",
        [&] () { this->displayOlympicGamesWinners(); }
    );

    complete_menu->addOption(
        "Find 10 youngest athletes to win a medal on their first olympics",
        [&] () { this->displayYoungestWinnerAthletes(); }
    );
    
    complete_menu->addOption(
        "Find all country-player pairs where player won at least 1 medal on team and individually",
        [&] () { this->displayCountryPlayerPairs(); }
    );

    complete_menu->addOption(
        "Find all athletes that played on 2 given olympics",
        [&] () { this->displayAthletesCommonOlympics(); }
    );
    
    complete_menu->addOption(
        "Find all teams of a country on given olympics",
        [&] () { this->displayCountriesTeamsOnGames(); }
    );
    
    complete_menu->addOption(
        "Find all cities olympics games were held in",
        [&] () { this->displayAllCities(); }
    );

    this->open_windows.push(std::move(complete_menu));
}

void sale::App::openBasicMetricsMenu()
{
    std::unique_ptr<sale::BasicMetricsMenu>
        basic_metrics_menu(new sale::BasicMetricsMenu);

    basic_metrics_menu->addOption("Quit", App::quit);

    basic_metrics_menu->addOption(
        "Go Back", [&] () { this->closeWindow(); }
    );

    // add other menu's functionality
    basic_metrics_menu->init();

    this->open_windows.push(std::move(basic_metrics_menu));
}


// ================================
// App Functionality:


void sale::App::quit()
{
    std::cout << std::endl
              << "Are you sure you want to quit? (y/[n]): ";

    char answer = 'n';
    std::cin >> answer;
    if (answer == 'y') exit(0);
}

void sale::App::closeWindow()
{
    if (this->open_windows.empty()) return;
    this->open_windows.pop();
}

void sale::App::loadInOlympicGamesData(const int year)
{
    sale::DataBase &db = sale::DataBase::getInstance();

    std::cout << "Enter relative path to file with events data:" << std::endl;
    //std::string events_data_file_path = "";
    //std::cin >> events_data_file_path;
    std::string events_data_file_path = this->readline(true);

    std::cout << "Enter relative path to file with athletes data:" << std::endl;
    //std::string athletes_data_file_path = "";
    //std::cin >> athletes_data_file_path;
    std::string athletes_data_file_path = this->readline(false);

    db.loadInData(events_data_file_path, athletes_data_file_path,
                  year, std::cout, true);
}

void sale::App::displayNumSportsCountryMedal() const
{
    const auto &db = sale::DataBase::getInstance();
    
    std::unique_ptr<std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>>
        athletes(nullptr);

    std::cout << "Enter the country's name:" << std::endl;
    std::shared_ptr<const std::string> country_key
        (new const std::string(this->readline()));

    if (!db.countries->count(country_key)) {
        std::cout << "This country does not exist in the database." << std::endl;
        return;
    }

    const auto country_criteria = db.countries->at(country_key);

    // set of keys of sports left to find among competitors
    std::unordered_set<sale::Sport::TKey,
                       sale::Sport::KeyHashStruct,
                       sale::Sport::KeyEqStruct>
        sports_meeting_criteria;

    auto process_competitor = [&] (const sale::Competitor& competitor) {
        for (const auto& medal : competitor.getMedals()) {
            if (medal.getCountryNameReference() != country_criteria->getNameReference())
                continue;

            const auto event_ = medal.getEvent();

            for (const auto& kv : *db.sports) {
                const auto &sport = *kv.second;
                if (sport.doesEventExist(event_->getKey(), event_->getType()))
                    sports_meeting_criteria.insert(sport.getKey());
            }
        }
    };

    auto are_country_keys_equal = db_country_t::KeyEqStruct{};

    for (const auto& kv : *db.olympic_games) {
        for (const auto& games_ptr : kv.second) {
            // process individuals
            for (const auto& athlete_country : games_ptr->getAthletesKeysCountries()) {
                const auto athlete_key = athlete_country.first;
                const auto country_ptr = athlete_country.second;

                const auto athlete_ptr = games_ptr->getAthletePtrFunc(athlete_key);

                if (!athlete_ptr) continue;
                if (!are_country_keys_equal(country_ptr->getKey(), country_key)) continue;

                process_competitor(*athlete_ptr);
            }

            // process teams
            for (const auto& team_country : games_ptr->getTeamsCountries()) {
                const auto &team = *team_country.first;
                const auto country_ptr = team_country.second;

                if (!are_country_keys_equal(country_ptr->getKey(), country_key)) continue;

                process_competitor(team);
            }
        }
    }

    const int num_sports = sports_meeting_criteria.size();
    std::cout << "Number of sports: " << num_sports << std::endl;
}

auto sale::App::getCountriesOlympicGamesScores
   (const db_oly_games_t &oly_games) const
{
    // medals[country] = <# of gold medals, # silver, # bronze>
    std::unordered_map<sale::Country<sale::DataBase::getAthletePtr>::TKey,
                       std::tuple<int, int, int>>
        countries_scores;
    
    auto count_medals = [] (const sale::Medal &medal, int &gold_count,
                            int &silver_count, int &bronze_count)
    {
        switch (medal.getMedal()) {
            case sale::Medal::TMedal::Gold:
                ++gold_count;
                break;
            case sale::Medal::TMedal::Silver:
                ++silver_count;
                break;
            case sale::Medal::TMedal::Bronze:
                ++bronze_count;
                break;
            default:  // given medal type is not supported ?
                break;
        }
    };

    // go through each country and its athlete on given games
    for (const auto& kv : oly_games.getAthletesKeysCountries()) {
        const auto athlete_key = kv.first;
        const auto &country = *kv.second;
        const auto country_key = kv.second->getKey();
        const auto &getAthletePtr = country.getAthletePtrFunc;

        int gold_count = 0;
        int silver_count = 0;
        int bronze_count = 0;

        // add score for each player's medal that was won individually
        if (auto athlete_ptr = getAthletePtr(athlete_key)) {
            for (const auto& medal : athlete_ptr->getMedals()) {
                if (!medal.isSharedByTeam()
                    && medal.getOlympicGamesID() == oly_games.getID())
                {
                    count_medals(medal, gold_count, silver_count, bronze_count);
                }
            }
        } else continue;

        if (countries_scores.count(country_key)) {  // increase what already exists
            auto &country_score = countries_scores.at(country_key);
            country_score = { gold_count + std::get<0>(country_score),
                              silver_count + std::get<1>(country_score),
                              bronze_count + std::get<2>(country_score)
            };
        }
        // create new country's record
        else countries_scores[country_key] = { gold_count, silver_count, bronze_count };
    }

    // go through each country and its team on given games
    for (const auto& kv : oly_games.getTeamsCountries()) {
        const auto &team = *kv.first;
        const auto &country = *kv.second;
        const auto country_key = kv.second->getKey();

        int gold_count = 0;
        int silver_count = 0;
        int bronze_count = 0;

        // add score for each team's medal
        for (const auto& medal : team.getMedals()) {
            if (medal.getOlympicGamesID() == oly_games.getID()) {
                count_medals(medal, gold_count, silver_count, bronze_count);
            }
        }

        if (countries_scores.count(country_key)) {  // increase what already exists
            auto &country_score = countries_scores.at(country_key);
            country_score = { gold_count + std::get<0>(country_score),
                              silver_count + std::get<1>(country_score),
                              bronze_count + std::get<2>(country_score)
            };
        }
        // create new country's record
        else countries_scores[country_key] = { gold_count, silver_count, bronze_count };
    }

    return countries_scores;
}

auto sale::App::getSortedCountriesOlympicGamesScores
   (const db_oly_games_t &oly_games) const
{
    const auto countries_scores = this->getCountriesOlympicGamesScores(oly_games);

    // sorts by gold, then silver, then bronze, then by country names ascending
    auto cmp_countries_scores = [] (const std::pair<db_country_t::TKey,
                                    std::tuple<int, int, int>> &x_kv,
                                    const std::pair<db_country_t::TKey,
                                    std::tuple<int, int, int>> &y_kv)
    {
        if (x_kv.second != y_kv.second) return x_kv.second > y_kv.second;
        return x_kv.first < y_kv.first;
    };

    const std::set<std::pair<db_country_t::TKey, std::tuple<int, int, int>>,
                   decltype(cmp_countries_scores)>
        sorted_countries 
            (countries_scores.begin(), countries_scores.end(), cmp_countries_scores);
    
    return sorted_countries;
}

void sale::App::displayOlympicGamesTop3() const
{
    const auto &db = sale::DataBase::getInstance();
    const auto oly_games = this->readOlympicGamesFromUser();
    
    const auto sorted_countries
        = this->getSortedCountriesOlympicGamesScores(*oly_games);

    // log just top 3 scores
    int ordinal = 1;
    for (const auto& country_score : sorted_countries) {
        const auto &medals_count = country_score.second;
        // if no medals have been won do not show the country, as it
        // may have not participated on these games at all
        if (medals_count == std::make_tuple(0, 0, 0)) continue;

        const auto &country_ptr = db.countries->at(country_score.first);
        std::cout << ordinal++ << ". "
                  << country_ptr->getNameReference()
                  << " (Gold: " << std::get<0>(medals_count) << ", "
                  << " Silver: " << std::get<1>(medals_count) << ", "
                  << " Bronze: " << std::get<2>(medals_count) << ")"
                  << std::endl;

        if (ordinal >= 4) break;
    }

    if (ordinal == 1) {  // no country had score > 0
        std::cout << "No country won a medal on these games" << std::endl;
        return;
    }
}

void sale::App::displayOlympicGamesWinners() const
{
    using country_t = sale::Country<sale::DataBase::getAthletePtr>;
    const auto &db = sale::DataBase::getInstance();
    std::unordered_set<country_t::TKey, country_t::KeyHashStruct, country_t::KeyEqStruct>
        countries_winners;

    for (const auto& kv : *db.olympic_games) {
        for (const auto &games_ptr : kv.second) {
            const auto sorted_countries_scores 
               = this->getSortedCountriesOlympicGamesScores(*games_ptr);

            // if no winners on cur games continue
            if (sorted_countries_scores.empty()) continue;

            // if no country won a single medal ignore them all
            const auto &prev_score = sorted_countries_scores.begin()->second;
            if (prev_score == std::make_tuple(0, 0, 0)) continue;

            // process all winners in case of tie and add them set
            bool is_first = true;
            for (const auto& country_score : sorted_countries_scores) {
                if (!is_first && country_score.second != prev_score) break;
                if (is_first) is_first = false;
                // add winner of these games
                countries_winners.insert(country_score.first);
            }
        }
    }

    // log all the winners:
    std::for_each(
        countries_winners.begin(),
        countries_winners.end(),
        [] (const country_t::TKey &country_name_ptr) {
            std::cout << *country_name_ptr << std::endl;
        }
    );
}

void sale::App::displayYoungestWinnerAthletes() const
{
    const int top_youngest_athletes = 10;

    using athlete_key_t = std::remove_const<sale::Athlete::TKey>::type;
    using athletes_keys_set_t = std::unordered_set<athlete_key_t>;
    
    athletes_keys_set_t participated;
    
    const auto &db = sale::DataBase::getInstance();

    // sorts them in descending order, returns if x.age > y.age
    auto cmp_prodigies = [&] (const athlete_key_t x, const athlete_key_t y) -> bool {
        return db.athletes->at(x)->getAge() > db.athletes->at(y)->getAge();
    };

    // athletes who won medal on their 1st olympics
    // key is their age, value is the athlete key
    std::map<int, std::vector<athlete_key_t>> prodigies;

    // caps size of prodigies to top_youngest_athletes
    auto push_to_prodigies = [&] (const athlete_key_t athlete_key) {
        const int age = db.athletes->at(athlete_key)->getAge();

        if (prodigies.count(age)) {  // age already exists just add prodigy
            prodigies[age].push_back(athlete_key);
            return;
        }
        
        if (prodigies.size() < top_youngest_athletes) {  // create new group
            prodigies[age] = { athlete_key };
            return;
        }

        // older than the oldest should not be added if capacity was reached
        if (age > prodigies.rbegin()->first) return;

        // remove the oldest
        prodigies.erase(std::prev(prodigies.end()));
        prodigies[age] = { athlete_key };
    };

    auto is_prodigy =
        [] (const sale::Athlete &athlete, const int oly_games_id) -> bool
    {
        for (const auto& medal : athlete.getMedals()) {
            if (medal.getOlympicGamesID() == oly_games_id) return true;
        }
        return false;
    };

    auto process_athlete =
        [&] (const athlete_key_t athlete_key, const int oly_games_id)
    {
        if (!db.athletes->count(athlete_key)) return;

        const auto &athlete = *db.athletes->at(athlete_key);
        
        if (athlete.getAge() < 0) return;  // age is NaN
        if (participated.count(athlete_key)) return;  // not athlete's 1st olympics
        
        participated.insert(athlete_key);

        if (is_prodigy(athlete, oly_games_id))
            push_to_prodigies(athlete_key);
    };

    auto sort_by_event = [] (const std::shared_ptr<db_oly_games_t> &x,
                             const std::shared_ptr<db_oly_games_t> &y)
    {
        if (x->getTypeReference() == "Summer") return true;
        return false;  // because x is winter which comes after summer
    };

    // go through all olympic games chronologically
    // and find athletes that won a medal on those games,
    // if they have not participated yet add to youngest winners

    for (const auto& kv : *db.olympic_games) {  // loop through years
        auto sorted_games = kv.second;
        std::sort(sorted_games.begin(), sorted_games.end(), sort_by_event);

        for (const auto& games_ptr : sorted_games) {  // loop through games by season
            const int games_id = games_ptr->getID();

            // go through all individuals
            for (const auto& athlete_country : games_ptr->getAthletesKeysCountries()) {
                const auto athlete_key = athlete_country.first;
                process_athlete(athlete_key, games_id);
            }

            // go through all teams
            for (const auto& team_country : games_ptr->getTeamsCountries()) {
                const auto &team_ptr = team_country.first;
                for (const auto athlete_key : team_ptr->getAthletesKeys()) {
                    process_athlete(athlete_key, games_id);
                }
            }
        }
    }

    // log all top 10 prodigies
    // (if multiple share age they are all included)
    int ordinal = 1;
    for (const auto &kv : prodigies) {
        for (const auto athlete_key : kv.second) {
            if (!db.athletes->count(athlete_key)) continue;

            std::cout << ordinal++ << ": " << db.athletes->at(athlete_key)->getNameReference()
                      << " (age: " << kv.first << ")" << std::endl;
        }
    }
}

auto sale::App::getCountryPlayerPairs() const
{
    const auto &db = sale::DataBase::getInstance();

    std::list<std::pair<std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>,
                        std::shared_ptr<sale::Athlete>
        >> country_player_pairs;

    // athlete_country_keys[athlete_id][country_key] = (did_win_team_medal, did_win_indiv_medal)
    std::unordered_map<
        std::remove_const<sale::Athlete::TKey>::type,
        std::unordered_map<
            sale::Country<sale::DataBase::getAthletePtr>::TKey,
            std::pair<bool, bool>,
            sale::Country<sale::DataBase::getAthletePtr>::KeyHashStruct,
            sale::Country<sale::DataBase::getAthletePtr>::KeyEqStruct
        >
    > athlete_country_criteria;

    for (const auto& kv : *db.olympic_games) {  // go through each year
        for (const auto& oly_games : kv.second) {  // go through all games that year
            // go through only individuals in cur country
            // because they have a medal individually as well
            for (const auto &athlete_country : oly_games->getAthletesKeysCountries()) {
                const auto athlete_key = athlete_country.first;

                if (athlete_country_criteria.count(athlete_key)) continue;
                athlete_country_criteria[athlete_key] = {};

                if (!db.athletes->count(athlete_key)) continue;
                auto athlete_ptr = db.athletes->at(athlete_key);

                // look for 1 individual and 1 team medal
                const auto &medals = athlete_ptr->getMedals();
                if (medals.size() < 2) continue;  // did not win enough medals

                for (const auto& medal : medals) {
                    const auto country_key = medal.getCountryName();
                    auto country_ptr = db.countries->at(country_key);

                    if (medal.isSharedByTeam()) {
                        if (!athlete_country_criteria.at(athlete_key).count(country_key)) {
                            athlete_country_criteria.at(athlete_key)[country_key]
                                = { true, false };
                        }
                        else {  // country_key exists
                            if (!athlete_country_criteria.at(athlete_key).at(country_key).first) {
                                // update is happening
                                athlete_country_criteria.at(athlete_key).at(country_key).first = true;

                                if (athlete_country_criteria.at(athlete_key).at(country_key)
                                    == std::make_pair(true, true))
                                {
                                    // after the update it finally meets criteria
                                    country_player_pairs.push_back({ std::move(country_ptr),
                                                                     athlete_ptr });
                                }
                            }
                        }
                    }
                    else {  // country_key exists
                        if (!athlete_country_criteria.at(athlete_key).count(country_key)) {
                            athlete_country_criteria.at(athlete_key)[country_key]
                                = { false, true };
                        }
                        else {
                            if (!athlete_country_criteria.at(athlete_key).at(country_key).second) {
                                // update is happening
                                athlete_country_criteria.at(athlete_key).at(country_key).second = true;

                                if (athlete_country_criteria.at(athlete_key).at(country_key)
                                    == std::make_pair(true, true))
                                {
                                    // after the update it finally meets criteria
                                    country_player_pairs.push_back({ std::move(country_ptr),
                                                                     athlete_ptr });
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return country_player_pairs;
}

auto sale::App::getCountryGamesTeams(const db_oly_games_t &oly_games,
                                     const db_country_t::TKey &country_key) const
{
    using team_t = sale::Team<sale::DataBase::getAthletePtr>;
    
    auto cmp_keys = db_country_t::KeyEqStruct{};
    
    auto cmp_teams = [] (const std::shared_ptr<team_t> &x,
                         const std::shared_ptr<team_t> &y) -> bool
    {
        const int num_athletes_x = x->getAthletesKeys().size();
        const int num_athletes_y = y->getAthletesKeys().size();

        if (num_athletes_x > num_athletes_y) return true;
        if (num_athletes_x < num_athletes_y) return false;

        return std::strcmp(x->getEvents()[0].first->getNameReference().c_str(),
                           y->getEvents()[0].first->getNameReference().c_str()) < 0;
    };

    std::unique_ptr<std::set<std::shared_ptr<team_t>, decltype(cmp_teams)>>
        teams (new std::set<std::shared_ptr<team_t>, decltype(cmp_teams)>(cmp_teams));

    for (const auto& team_country : oly_games.getTeamsCountries()) {
        if (cmp_keys(country_key, team_country.second->getKey())) {
            teams->insert(team_country.first);
        }
    }

    return teams;
}

void sale::App::displayCountryPlayerPairs() const
{
    auto country_player_pairs = this->getCountryPlayerPairs();
    
    int ordinal = 1;

    std::for_each(
        country_player_pairs.begin(),
        country_player_pairs.end(),
        [&] (const std::pair<std::shared_ptr<sale::Country<sale::DataBase::getAthletePtr>>,
                             std::shared_ptr<sale::Athlete>> &country_athlete)
        {
            const auto &country_ptr = country_athlete.first;
            const auto &athlete_ptr = country_athlete.second;

            std::cout << ordinal++ << ". " << country_ptr->getNameReference() << ": "
                      << athlete_ptr->getNameReference() << std::endl;
        }
    );
}

void sale::App::displayAthletesCommonOlympics() const
{
    const auto &db = sale::DataBase::getInstance();

    const auto oly_games1 = this->readOlympicGamesFromUser();
    const auto oly_games2 = this->readOlympicGamesFromUser();

    std::unique_ptr<std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>> athletes;
    sale::utils::filters::filterAthletesOlympicGame(athletes, *oly_games1);
    sale::utils::filters::filterAthletesOlympicGame(athletes, *oly_games2);

    for (const auto athlete_key : *athletes) {
        if (!db.athletes->count(athlete_key)) continue;
        const auto &athlete = *db.athletes->at(athlete_key);
        std::cout << athlete_key << ": " << athlete.getNameReference() << std::endl;
    }
}

void sale::App::displayCountriesTeamsOnGames() const
{
    const auto &db = sale::DataBase::getInstance();
    
    const auto oly_games = this->readOlympicGamesFromUser();

    std::string country_name = "";
    std::cout << "Enter country's name:" << std::endl;
    std::cin >> country_name;

    std::shared_ptr<const std::string> country_key(new const std::string(country_name));

    const auto teams = this->getCountryGamesTeams(*oly_games, country_key);
    int ordinal = 1;

    std::for_each(
        teams->begin(),
        teams->end(),
        [&] (const std::shared_ptr<sale::Team<sale::DataBase::getAthletePtr>> &self) {
            std::cout << ordinal++ << ". ";
            
            auto &os = std::cout;
            os << "Team {";

            for (const auto athlete_key : self->getAthletesKeys()) {
                if (!db.athletes->count(athlete_key)) continue;
                os << std::endl << "\t " << (*db.athletes->at(athlete_key));
            }

            os << std::endl << "\t" << "}" << std::endl << std::endl << std::endl;
        }
    );
}

void sale::App::displayAllCities() const
{
    const auto &db = sale::DataBase::getInstance();
    
    std::unordered_set<std::string> cities;

    for (const auto& kv : *db.olympic_games) {  // loop through years
        for (const auto& games : kv.second) {
            cities.insert(games->getCity());
        }
    }

    for (const auto& city : cities) {
        std::cout << city << std::endl;
    }
}
