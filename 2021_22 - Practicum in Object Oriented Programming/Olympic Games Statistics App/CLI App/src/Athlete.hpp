#ifndef ATHLETE_HPP
#define ATHLETE_HPP


#include "HashingStructures.hpp"
#include "HashableData.hpp"
#include "Competitor.hpp"
#include <iostream>


// Declare Operators to Overload:

namespace sale { class Athlete; }
bool operator== (const sale::Athlete &x, const sale::Athlete &y);


namespace sale {


// Friend Declarations:

std::ostream& operator<< (std::ostream &os, const sale::Athlete &self);


class Athlete : public Competitor,
                public HashableData< const int,
                                     sale::hashing::HashInt,
                                     sale::hashing::EqualityInt >
{

 public:

    Athlete(const int id);
    Athlete(const int id, std::shared_ptr<sale::Event> event_ptr, const int oly_games_id);
    ~Athlete() override = default;


    // Setters:

    void setName(const std::string &name);
    void setGender(const char gender);
    void setAge(const int age);
    void setHeight(const int height);
    void setWeight(const float weight);

    // Getters:
    
    const std::string& getNameReference() const;
    std::string getName() const;
    int getAge() const;
    int getHeight() const;
    float getWeight() const;


    // HashableData overrides:

    TKey getKey() const override;


    // Operator Overloads:

    friend bool ::operator== (const Athlete &x, const Athlete &y);
    friend std::ostream& operator<< (std::ostream &os, const sale::Athlete &self);


 private:

     TKey id;
     std::string name = "";
     char gender = '-';  // M/F = male/female, dash for not defined
     int age = -1;         // -1 for not defined
     int height = -1;    // -1 for not defined
     float weight = -1.;    // -1 for not defined

};


}  // end of sale namespace


#endif
