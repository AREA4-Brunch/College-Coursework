#ifndef FILTERING_UTILS_HPP
#define FILTERING_UTILS_HPP


#include <string>
#include <unordered_set>
#include <map>
#include <memory>
#include <vector>
#include "Athlete.hpp"
#include "OlympicGames.hpp"
#include "Country.hpp"
#include "DataBase.hpp"


namespace sale {
namespace utils {
struct filters {

    using db_oly_games_map
        = std::map<
            int,  // key - year
            // data - group of games:
            std::vector<std::shared_ptr<sale::OlympicGames<sale::DataBase::getAthletePtr>>>
          >;


    // filters given games to contain only 
    static void filterGamesYear(std::unique_ptr<db_oly_games_map> &oly_games,
                                const int year);


    // removes individuals and teams from those games in which they
    // did not satisfy the given medal criteria
    static void filterGamesMedal(std::unique_ptr<db_oly_games_map> &oly_games,
                                 const sale::Medal &medal_criteria,
                                 bool only_individual_medals=false,
                                 bool only_team_medals=false
                                );


    static void filterGamesSport(std::unique_ptr<db_oly_games_map> &oly_games,
                                    const std::string &sport);


    static void filterGamesCountry(std::unique_ptr<db_oly_games_map> &oly_games,
                                   const std::string &country);


    static void filterGamesEventType(std::unique_ptr<db_oly_games_map> &oly_games,
                                     const std::string &event_type);


    // removes from given set of ahtlete keys the ones that are
    // not present in given olympic games, if `athletes` is nullptr, adds all athlete keys
    template<typename TLink>
    static void filterAthletesOlympicGame
    (std::unique_ptr<std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>> &athletes,
     const sale::OlympicGames<TLink> &olympic_game)
    {
        using set_t = std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>;

        std::unique_ptr<set_t> filtered_athletes(new set_t());

        // go through all athletes on cur olympic games
        for (const auto athlete_country : olympic_game.getAthletesKeysCountries()) {
            const auto athlete_key = athlete_country.first;
            if (!athletes || athletes->count(athlete_key)) {
                filtered_athletes->insert(athlete_key);
            }
            //const auto athlete_ptr = olympic_game->getAthletePtrFunc(athlete_key);
        }

        // go through each team on cur olympic games
        for (const auto team_countries : olympic_game.getTeamsCountries()) {
            const auto &team = *team_countries.first;
            // go through each player in the team
            for (const auto athlete_key : team.getAthletesKeys()) {
                if (!athletes || athletes->count(athlete_key)) {
                    filtered_athletes->insert(athlete_key);
                }
                //const auto athlete_ptr = olympic_game->getAthletePtrFunc(athlete_key);
            }
        }

        athletes = std::move(filtered_athletes);
    }


    // Helper Functions:

    static void copyOlympicGamesFromDatabase(std::unique_ptr<db_oly_games_map>& oly_games);


};  // end of filters struct
}  // end of utils namespace
}  // end of sale namespace


#endif // !FILTERING_UTILS_HPP
