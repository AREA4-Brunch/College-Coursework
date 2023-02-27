#ifndef OPTIONS_MENU_HPP
#define OPTIONS_MENU_HPP

#include "Menu.hpp"
#include <vector>
#include <functional>
#include <string>


namespace sale {


/* Main menu of the application. */
class OptionsMenu : public Menu
{

 public:

	// ================================
	// Constructors and Destructor:

	OptionsMenu();
	virtual ~OptionsMenu() override = default;
	

	// ================================
	// Functionality of the menu:

	// Prompts user for option's ordinal and executes chosen option.
	void selectOption() override;

	// Prompts user for option's ordinal and executes chosen option.
	void selectOption() const override;

	// Adds to the menu an option described by function and
	// its description which is to be displayed in the menu.
	void addOption(const std::string &option_desc,
				   std::function<void ()> option_func);

 protected:


	// ================================
	// Display of the menu:

	// Logs the menu msg then
	// logs each option and its ordinal in new line,
	std::ostream& display(std::ostream &os) override;

	// Logs the menu msg then
	// logs each option and its ordinal in new line
	std::ostream& display(std::ostream &os) const override;

	// sets message to be displayed before the options every
	// time an option has been added
	virtual void updateMenuMsg();


	// ================================
	// Attributes:

	// list of options represented as pairs of action
	// function called when option is selected and its
	// description/display message
	std::vector< std::pair< std::function<void ()>, std::string > > options;

	// message displayed before the options
	std::string menu_msg;

};


}  // end of sale namespace

#endif
