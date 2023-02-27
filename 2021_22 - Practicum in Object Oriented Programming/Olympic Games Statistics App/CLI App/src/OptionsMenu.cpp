#include "OptionsMenu.hpp"

#include "MenuExceptions.hpp"
#include <algorithm>  // std::for_each


// ================================
// Constructors and Destructor:


sale::OptionsMenu::OptionsMenu()
{
	this->updateMenuMsg();  // important to open after options
}


// ================================
// Constructor Helpers:


void sale::OptionsMenu::updateMenuMsg()
{
	this->menu_msg = this->options.size() == 0 ?
					"The main menu offers no options at this time."
				  : "Choose one of the following options:";
}


// ================================
// Public Methods:


void sale::OptionsMenu::selectOption()
{
	const_cast<const sale::OptionsMenu*>(this)->selectOption();
}

void sale::OptionsMenu::selectOption() const
{
	int option_idx = -1;
	std::cout << "\nEnter the ordinal of an option: ";
	std::cin >> option_idx;

	// log error message
	if (option_idx < 0 || option_idx >= this->options.size())
		throw sale::exceptions::InvalidOptionOrdinalException();

	// select option_idx-th option
	this->options[option_idx].first();
}


// ================================
// Protected Methods:


void sale::OptionsMenu::addOption(const std::string &option_desc,
							   std::function<void()> option_func)
{
	this->options.push_back({std::move(option_func), option_desc});
	this->updateMenuMsg();
}

std::ostream& sale::OptionsMenu::display(std::ostream& os)
{
	return const_cast<const sale::OptionsMenu*>(this)->display(os);
}

std::ostream& sale::OptionsMenu::display(std::ostream& os) const
{
	os << std::endl << this->menu_msg << std::endl << std::endl;

	// log ordinal, colon and the option
	using func_str = std::pair<std::function<void ()>, std::string>;

	int option_idx = 0;  // ordinal of each option
	auto display_func = [&os, &option_idx] (const func_str &option) {
		os << option_idx++ << ": " << option.second << std::endl;
	};

	std::for_each(this->options.begin(), this->options.end(), display_func);
	
	return os;
}
