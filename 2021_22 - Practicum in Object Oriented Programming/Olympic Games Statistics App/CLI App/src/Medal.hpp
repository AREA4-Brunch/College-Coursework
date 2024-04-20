#ifndef MEDAL_HPP
#define MEDAL_HPP


#include <exception>
#include <string>
#include <memory>


namespace sale {


class Event;


class InvalidMedalStrFormat : public std::exception {
 public:
    const char* what() const noexcept override {
        return "Unrecognized string defining a medal.";
    }
};


class Medal
{

 public:

    enum class TMedal : char {
        Gold, Silver, Bronze, NoMedal
    };

    Medal(const TMedal &medal, const int olympic_games_id,
          std::shared_ptr<const std::string> country_name,
          std::shared_ptr<sale::Event> event_);

    Medal(const std::string &medal_str, const int olympic_games_id,
          std::shared_ptr<const std::string> country_name,
          std::shared_ptr<sale::Event> event_);
    
    Medal(const char* const medal_str, const int olympic_games_id,
          std::shared_ptr<const std::string> country_name,
          std::shared_ptr<sale::Event> event_);
    
    ~Medal() = default;

    // Operator Overloads:
    bool operator== (const Medal &other) const;
    bool operator!= (const Medal &other) const;
    bool operator> (const Medal &other) const;
    bool operator< (const Medal &other) const;
    bool operator>= (const Medal &other) const;
    bool operator<= (const Medal &other) const;


    // Getters:
    int getMedalScore() const;
    int getOlympicGamesID() const;
    TMedal getMedal() const;
    bool isSharedByTeam() const;
    const std::string& getCountryNameReference() const;
    std::shared_ptr<const std::string> getCountryName() const;
    std::shared_ptr<sale::Event> getEvent() const;

    // Setters:

    // sets is_shared_by_team as true
    void setAsSharedByTeam();

 private:
    
    TMedal medal = TMedal::NoMedal;
    const int olympic_games_id;
    bool is_shared_by_team = false;
    std::shared_ptr<const std::string> country_name;
    std::shared_ptr<sale::Event> event_;

};


}  // end of sale namespace

#endif
