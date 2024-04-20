#ifndef APP_HPP
#define APP_HPP


#include "Menu.hpp"
#include <memory>
#include <stack>
#include <unordered_map>
#include "Country.hpp"
#include "DataBase.hpp"
#include "OlympicGames.hpp"


namespace sale {


class App 
{

 public:

    App();
    ~App() = default;

    // forbid copies of the app
    App(const App&) = delete;
    App& operator=(const App &) = delete;


    // Main loop that once entered cannot be exited
    void forEverLoop();


 private:

    typedef sale::Country<sale::DataBase::getAthletePtr>
            db_country_t;

    typedef sale::OlympicGames<sale::DataBase::getAthletePtr>
            db_oly_games_t;


    // ================================
    // Main Menus:

    // keep track of windows/menus stacked over each other
    std::stack<std::unique_ptr<sale::Menu>> open_windows;

    // returns reference to active menu object, the
    // one on the top of open_windows stack
    sale::Menu& fetchActiveMenu();

    // options menu version with options available
    // before the database is loaded in
    void openStartupMenu();

    // options menu with options available once
    // the data has been loaded in
    void openCompleteMenu();

    // options menu with options to present basic metrics
    void openBasicMetricsMenu();

    
    // ================================
    // App Functionality:
    
    // calls exit(0), asks for confirmation over stdin/stdout
    static void quit();

    // closes window on top of open_windows stack
    void closeWindow();

    void loadInOlympicGamesData(const int year=-1);

    // Special databse search options:

    // logs number of sports in which country won at least 1 medal
    // prompts user for country's name via stdin/stdout
    void displayNumSportsCountryMedal() const;

    // logs 3 best ranked countries on olympic games set by user
    // prompts user for olympic games via stdin/stdout
    void displayOlympicGamesTop3() const;

    // logs all countries that won at least 1 olympic games
    void displayOlympicGamesWinners() const;

    // logs 10 youngest athletes to win medal on their first
    // olympic gamse
    void displayYoungestWinnerAthletes() const;
    
    // logs all country-player pairs where player won
    // at least 1 medal on team and individual events
    void displayCountryPlayerPairs() const;

    // logs all athletes that participated on both olympic games
    // given by user via stdin/stdout
    void displayAthletesCommonOlympics() const;

    // logs all teams a country had on games set by user
    // in descending order by num players and ascending
    // order lexicographically by event's name
    void displayCountriesTeamsOnGames() const;

    // logs all cities olympic games were held in
    void displayAllCities() const;

    // returns map of key(country key), value(score on given games)
    auto getCountriesOlympicGamesScores
        (const db_oly_games_t &oly_games) const;
    
    auto getSortedCountriesOlympicGamesScores
        (const db_oly_games_t &oly_games) const;
    
    auto getCountryPlayerPairs() const;
    
    auto getCountryGamesTeams(const db_oly_games_t &oly_games,
                              const db_country_t::TKey &country_key) const;

    // Helper Functions:
    static std::string readline(bool do_flush=true);
    static auto readOlympicGamesFromUser(bool do_flush_first=false);
};


}  // end of namespace sale

#endif
