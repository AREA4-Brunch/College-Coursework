#ifndef BASIC_METRICS_MENU_HPP
#define BASIC_METRICS_MENU_HPP


#include "OptionsMenu.hpp"
#include "Athlete.hpp"
#include <map>
#include <unordered_set>
#include <unordered_map>


namespace sale {


class BasicMetricsMenu final : public sale::OptionsMenu
{

 public:


    BasicMetricsMenu();
    ~BasicMetricsMenu() override = default;

    // adds all its functionality to selectable options
    void init();


 protected:


    void updateMenuMsg() override;


 private:

    using set_t = std::unordered_set<std::remove_const<sale::Athlete::TKey>::type>;
    using events_map_t = std::unordered_multimap<
                            sale::Event::TKey,  // key
                            std::shared_ptr<sale::Event>,  // data
                            sale::Event::KeyHashStruct,
                            sale::Event::KeyEqStruct
                         >;

     // do not use unordered_map, need to preserve order
     // filters[filter name] = empty string for not applied,
     //                           else user input of filter's value
     std::map<std::string, std::string> filters = {
         {"Sport", ""},
         {"Country", ""},
         {"Year", ""},
         {"Event Type", ""},
         {"Medal", ""},
     };

     // function connecting Athlete ptrs with keys stored locally
     static const std::function<
         std::shared_ptr<sale::Athlete> (sale::Athlete::TKey)
     > getAthletePtrFunc;

     // options menu with options to apply filters on basic metrics
     void openFiltersMenu();

     // creates options menu enabling user to select one filter
     // among those turned off in filters map
     void openAddSingleFilterMenu();


     // App Functionalities:

     void displayNumberOfAthletes();
     void displayNumberOfSportDisciplines();
     void displayAverageAthleteHeight();
     void displayAverageAthleteWeight();

     int getNumAthletes() const;
     int getNumSportDisciplines() const;

     // returns average height of athletes,
     // ones with undefined height are skipped,
     // if no such athletes exist returns -1.
     float getAvgAthleteHeight() const;

     // returns average weight of athletes,
     // ones with undefined height are skipped,
     // if no such athletes exist returns -1.
     float getAvgAthleteWeight() const;

     // modifies given ptr to point to unordered set of athlete keys
     // that represent result of filter query,
     // if no filters are turned on sets it to nullptr
     void filterAthletes(std::unique_ptr<set_t> &athletes) const;

     // modifies given ptr to point to unordered set of events keys
     // that represent result of filter query,
     // if no filters are turned on sets it to nullptr
     void filterEvents(std::unique_ptr<events_map_t> &events) const;


};


}  // end of sale namespace

#endif
