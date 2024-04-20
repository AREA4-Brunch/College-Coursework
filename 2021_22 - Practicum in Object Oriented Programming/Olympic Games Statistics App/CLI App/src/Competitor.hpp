#ifndef COMPETITOR_HPP
#define COMPETITOR_HPP


#include "Event.hpp"
#include "Medal.hpp"
#include <vector>
#include <memory>


namespace sale {


class Competitor
{

 public:

    Competitor(const int max_num_events);
    Competitor() = default;
    virtual ~Competitor() = 0;

    // Forbid copies of competitor
    Competitor(const Competitor &) = delete;
    Competitor& operator= (const Competitor&) = delete;

    // Getters:
    const std::vector<sale::Medal>& getMedals() const;
    const std::vector<std::pair<std::shared_ptr<sale::Event>, std::vector<int>>>& getEvents() const;

    // adds medal to the competitor if it is not `no medal` value
    void addMedal(const sale::Medal &medal);


    // Managing Events:

    // adds an event if it does not already exist
    void addEvent(std::shared_ptr<sale::Event> event_ptr, const int oly_games_id);

    // reduces all vectors down to take up only as much
    // as the elemnts inside them
    virtual void shrinkToFit();


 private:

    std::vector<sale::Medal> medals;

    // list of unique events competitor participated in
    std::vector<
        std::pair<
            std::shared_ptr<sale::Event>,  // discipline competitor competed in
            std::vector<int>  // ids of olympic games
        >
    > events;

};


inline Competitor::~Competitor() {}


}  // end of sale namespace


#endif
