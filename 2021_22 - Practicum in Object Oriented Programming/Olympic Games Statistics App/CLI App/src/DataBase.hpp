#ifndef DATA_BASE_HPP
#define DATA_BASE_HPP


#include "Athlete.hpp"
#include "Team.hpp"
#include "Sport.hpp"
#include "Country.hpp"
#include "OlympicGames.hpp"
//#include "FilteringUtils.hpp"


#include <memory>
#include <unordered_map>
#include <map>
#include <list>
#include <iostream>



namespace sale {


class BasicMetricsMenu;  // friend of DataBase
class App;  // friend of DataBase
namespace utils { struct filters; };


class DataBase
{

    using set_t = std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>;

    friend class sale::BasicMetricsMenu;
    friend class sale::App;
    friend struct sale::utils::filters;


 public:

    static DataBase& getInstance();
    static void clearInstance();
    static void destroyInstance();

    DataBase(const DataBase &) = delete;
    DataBase& operator= (const DataBase &) = delete;


    void loadInData(const std::string &competitors_file_path,
                    const std::string &athletes_file_path,
                    const int year=-1,
                    std::ostream &os_for_report=std::cout,
                    bool do_log_report=true);

 private:


    DataBase() {}
    ~DataBase() {};
    

    // Struct for linking athlete ids in objects
    // with the athletes ptrs data table
    struct getAthletePtr
    {
        // returns pointer to an athlete with given key if
        // such exists in the database, nullptr otherwise
        std::shared_ptr<Athlete> operator() (Athlete::TKey key) const
        {
            DataBase &db = DataBase::getInstance();
            if (db.athletes->count(key)) return (*db.athletes)[key];
            return nullptr;
        }
    };


    // ================================
    // Attributes:

    std::list<
        std::shared_ptr<Team<getAthletePtr>>
    > teams;

    std::unique_ptr<
        std::unordered_map<
            Athlete::TKey,  // key
            std::shared_ptr<Athlete>,  // data
            Athlete::KeyHashStruct,
            Athlete::KeyEqStruct
        >
    > athletes = nullptr;

    std::unique_ptr<
        std::unordered_map<
            Country<getAthletePtr>::TKey,  // key
            std::shared_ptr<Country<getAthletePtr>>,  // data
            Country<getAthletePtr>::KeyHashStruct,
            Country<getAthletePtr>::KeyEqStruct
        >
    > countries = nullptr;

    std::unique_ptr<
        std::unordered_map<
            Sport::TKey,  // key
            std::shared_ptr<Sport>,  // data
            Sport::KeyHashStruct,
            Sport::KeyEqStruct
        >
    > sports = nullptr;
    
    // group olympic games per year
    std::unique_ptr<
        std::map<
            int,  // key - year
            // data - group of games:
            std::vector<std::shared_ptr<OlympicGames<getAthletePtr>>>
        >
    > olympic_games = nullptr;


    // ================================
    // Helpers for loading in data:


    // Loads in competitors from specified year,
    // if given year is negative loads in all of them.
    // Returns number of unsuccessfuly read lines.
    // Does not close the ifstream.
    int parseCompetitors(std::ifstream &in_file, const int year=-1,
                         bool do_log_errors=true);

    // Loads in all data about the athletes whose keys
    // are present in the athletes table of this database
    // Returns number of unsuccessfuly read lines.
    // Does not close the ifstream.
    int parseAthletes(std::ifstream &in_file, bool do_log_errors=true);

    // Opens file at given path, calls given parsing function
    // on it and closes it when parsing function is done.
    // Logs status messages in given ostream if do_log_report=true
    void openAndParseFile(const std::string &file_path,
                          std::ostream &os_for_report,
                          bool do_log_report,
                          const std::function<int (std::ifstream&)> &parsing_func);

    // return ptr to olympics game object that has given params
    // if such exists, nullptr otherwise
    std::shared_ptr<sale::OlympicGames<getAthletePtr>> getGamePtr
        (const int year, const char * const game_type) const;

};


}  // end of namespace sale


#endif
