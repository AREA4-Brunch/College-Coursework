#include "Athlete.hpp"


// ================================
// Constructors:


sale::Athlete::Athlete(const int id)
	: id(id)
{
}

sale::Athlete::Athlete(const int id,
					   std::shared_ptr<sale::Event> event_ptr,
					   const int oly_games_id)
	: id(id)
{
	this->addEvent(std::move(event_ptr), oly_games_id);
}


// ================================
// Public Setters:


void sale::Athlete::setName(const std::string& name)
{
	this->name = name;
}

void sale::Athlete::setGender(const char gender)
{
	this->gender = gender;
}

void sale::Athlete::setAge(const int age)
{
	this->age = age;
}

void sale::Athlete::setHeight(const int height)
{
	this->height = height;
}

void sale::Athlete::setWeight(const float weight)
{
	this->weight = weight;
}

const std::string& sale::Athlete::getNameReference() const
{
	return this->name;
}

std::string sale::Athlete::getName() const
{
	return this->name;
}

int sale::Athlete::getAge() const
{
	return this->age;
}

int sale::Athlete::getHeight() const
{
	return this->height;
}

float sale::Athlete::getWeight() const
{
	return this->weight;
}


// ================================
// HashableData overrides:


sale::Athlete::TKey sale::Athlete::getKey() const
{
	return this->id;
}


// ================================
// Operator Overloads:


bool operator==(const sale::Athlete& x, const sale::Athlete& y)
{
	return x.id == y.id;
}


namespace sale{


std::ostream& operator<<(std::ostream& os, const sale::Athlete& self)
{
	return os << self.getNameReference() << " (" << self.getKey() << ")";
}


}  // end of sale namespace
