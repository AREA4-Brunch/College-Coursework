#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>


namespace sale {


class Menu;
std::ostream& operator<< (std::ostream &os, Menu &self);
std::ostream& operator<< (std::ostream &os, const Menu &self);


/* Interface for the Menus, it is displayable and an option
   can be selected.
*/
class Menu
{

 public:

	virtual ~Menu() = 0;


	// Functionality of the menu:

	virtual void selectOption() = 0;
	virtual void selectOption() const = 0;


	// Display of menu:

	friend std::ostream& operator<< (std::ostream& os, Menu& self) {
		return self.display(os);
	}

	friend std::ostream& operator<< (std::ostream& os, const Menu& self) {
		return self.display(os);
	}


 protected:

	 virtual std::ostream& display(std::ostream &os) = 0;
	 virtual std::ostream& display(std::ostream &os) const = 0;

};


inline Menu::~Menu() {}


}  // end of sale namespace

#endif
