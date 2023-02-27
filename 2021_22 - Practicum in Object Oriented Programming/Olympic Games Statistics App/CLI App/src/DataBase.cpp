#include "DataBase.hpp"
#include "DataBaseExceptions.hpp"

#include <fstream>
//#include <regex>
#include <boost/regex.hpp>


// ================================
// Types used for data tables:


typedef std::unordered_map<
			sale::Athlete::TKey,  // key
			std::shared_ptr<sale::Athlete>,  // data
			sale::Athlete::KeyHashStruct,
			sale::Athlete::KeyEqStruct
		> athlete_map_t;

typedef std::unordered_map<
			sale::Sport::TKey,  // key
			std::shared_ptr<sale::Sport>,  // data
			sale::Sport::KeyHashStruct,
			sale::Sport::KeyEqStruct
		> sports_map_t;

#define countries_map_t std::unordered_map<\
			sale::Country<getAthletePtr>::TKey,\
			std::shared_ptr<sale::Country<getAthletePtr>>,\
			sale::Country<getAthletePtr>::KeyHashStruct,\
			sale::Country<getAthletePtr>::KeyEqStruct\
		>

#define olympic_games_map_t std::map< \
			int, \
			std::vector<std::shared_ptr<OlympicGames<getAthletePtr>>> \
		>


// ================================
// Public Static Methods:


sale::DataBase& sale::DataBase::getInstance()
{
	static sale::DataBase instance;
	return instance;
}

void sale::DataBase::clearInstance()
{
	DataBase &db = DataBase::getInstance();
	
	db.teams.clear();

	if (db.athletes) db.athletes->clear();
	else db.athletes.reset(new athlete_map_t());

	if (db.countries) db.countries->clear();
	else db.countries.reset(new countries_map_t());

	if (db.sports) db.sports->clear();
	else db.sports.reset(new sports_map_t());

	if (db.olympic_games) db.olympic_games->clear();
	else db.olympic_games.reset(new olympic_games_map_t());
}

void sale::DataBase::destroyInstance()
{
	DataBase &db = DataBase::getInstance();

	db.teams.clear();
	db.athletes.reset(nullptr);
	db.countries.reset(nullptr);
	db.sports.reset(nullptr);
	db.olympic_games.reset(nullptr);
}


// ================================
// Public Methods:


void sale::DataBase::loadInData(const std::string &competitors_file_path,
								const std::string &athletes_file_path,
								const int year,
								std::ostream &os_for_report,
								bool do_log_report)
{
	// wipe the already existant data
	this->clearInstance();

	// Load in the events and general olympic games data
	this->openAndParseFile(competitors_file_path, os_for_report, do_log_report,
		[&] (std::ifstream &in_file) -> int {
			return this->parseCompetitors(in_file, year, do_log_report);
		}
	);

	// Load in data about individuals/athletes
	this->openAndParseFile(athletes_file_path, os_for_report, do_log_report,
		[&] (std::ifstream &in_file) -> int {
			return this->parseAthletes(in_file, do_log_report);
		}
	);
}

int sale::DataBase::parseCompetitors(std::ifstream &in_file,
									 const int year_filter,
									 bool do_log_errors)
{
	const std::string year_format_str = year_filter < 0 ?
									    "(\\d+)"
									  : "(" + std::to_string(year_filter) + ")";

	const std::string reg_exp_common_str = year_format_str
		+ " ([^!]+)!([^!]+)!([^!]+)!([^!]+)!((?:Team)|(?:Individual))!([^!]+)!([^!]+)!((?:Gold)|(?:Silver)|(?:Bronze)|)";

	const boost::regex reg_exp_common(reg_exp_common_str);

	int line_idx = 1;
	int skipped_lines_count = 0;

	// read line by line
	for (std::string line; std::getline(in_file, line); ++line_idx) {
		// skip blank lines
		if (line.length() == 0) continue;

		try {
			boost::smatch reg_result;

			if (!boost::regex_match(line, reg_result, reg_exp_common)) {
				if (year_filter < 0)
					throw sale::exceptions::LineInvalidFormat();

				// could be skipping because the year does not match
				// so do not throw excpetion
				++skipped_lines_count;
				continue;
			}

			// do not read in the year from regex if it was
			// already set as the filter
			int year = year_filter >= 0 ?
					   year_filter
					 : std::atoi(reg_result.str(1).c_str());
			
			// groups: 2 - game type, 3 - city
			// check if this game matches previous, then reuse prev ptr
			std::shared_ptr<sale::OlympicGames<getAthletePtr>> olympic_games
				= this->getGamePtr(year, reg_result.str(2).c_str());

			if (!olympic_games) {  // create new olympic games
				olympic_games.reset(new sale::OlympicGames<getAthletePtr>(
					year,
					std::move(reg_result.str(2)),  // game type
					std::move(reg_result.str(3))   // city name
				));

				// add to the year's group in database:
				if (!this->olympic_games->count(year)) {  // create new group
					(*this->olympic_games)[year] = { olympic_games };
				}
				else {  // add new games to the group
					(*this->olympic_games)[year].emplace_back(olympic_games);
				}
			}  // else olympic_games points to appropriate games
			
			// group 4 - sport's name
			std::shared_ptr<const std::string> sport_name
				= std::make_shared<const std::string>(std::move(reg_result.str(4)));

			// create sport object or use one from database if exists
			if (!this->sports->count(sport_name)) {
				(*this->sports)[sport_name]
					= std::make_shared<sale::Sport>(sport_name);
			}
			std::shared_ptr<sale::Sport> &sport = (*this->sports)[std::move(sport_name)];

			// groups: 5 - event's name, 6 - type of event(team/individual)
			std::shared_ptr<const std::string> event_name
				= std::make_shared<const std::string>(std::move(reg_result.str(5)));
			
			sale::Event::TEventType event_type
					= std::strcmp(reg_result.str(6).c_str(), "Team") == 0 ?
					  sale::Event::TEventType::Team
					: sale::Event::TEventType::Individual;

			if (!sport->doesEventExist(event_name, event_type)) {
				std::shared_ptr<sale::Event> event_ptr =
					std::make_shared<sale::Event>(event_type, event_name);
				
				sport->addEvent(std::move(event_ptr));
			}

			const std::shared_ptr<sale::Event> &event_
				= sport->getEvent(std::move(event_name), event_type);

			// groups: 7 - country
			std::shared_ptr<const std::string> country_name
				= std::make_shared<const std::string>(std::move(reg_result.str(7)));

			// create country object or use one from database if exists
			if (!this->countries->count(country_name)) {
				(*this->countries)[country_name]
					= std::make_shared<sale::Country<getAthletePtr>>(country_name);
			}
			auto country = (*this->countries)[country_name];

			// group 9 - medal
			sale::Medal medal(std::move(reg_result.str(9)), olympic_games->getID(),
							  std::move(country_name), event_);

			// ! important to update the line last
			// group 8 - ids
			line = reg_result.str(8);  // athlete's/teams' id data

			// read ids of team's players or individual:
			if (event_type == sale::Event::TEventType::Individual) {
				// read id of Individual
				const int athlete_id = std::atoi(line.c_str());
				
				country->addAthlete(athlete_id);
				olympic_games->addAthlete(athlete_id, country);

				if (this->athletes->count(athlete_id)) {  // exists in db
					auto &athlete = (*this->athletes)[athlete_id];
					athlete->addMedal(std::move(medal));
					athlete->addEvent(event_, olympic_games->getID());
				}
				else {  // new ahtlete
					std::shared_ptr<sale::Athlete> athlete
						= std::make_shared<sale::Athlete>
							(athlete_id, event_, olympic_games->getID());
					athlete->addMedal(std::move(medal));
					(*this->athletes)[athlete_id] = std::move(athlete);
				}
			}
			else {  // read ids of team players
				const boost::regex team_info("'(\\d+)'(?:, )?");
				
				// count number of found ids to reserve space in team
				boost::sregex_iterator iter(line.begin(), line.end(), team_info);
				const boost::sregex_iterator end;

				// create a team
				std::shared_ptr<sale::Team<getAthletePtr>> team
					= std::make_shared<sale::Team<getAthletePtr>> (
						event_, olympic_games->getID()
						//(std::distance(iter, end),  // reserve num of athletes
						// event_)
					);

				medal.setAsSharedByTeam();
				team->addMedal(medal);
				country->addTeam(team);
				olympic_games->addTeam(team, country);

				for ( ; iter != end; ++iter)  // add each athlete in team
				{
					const int athlete_id = std::atoi(iter->str(1).c_str());
					
					if (!this->athletes->count(athlete_id)) {
						std::shared_ptr<sale::Athlete> athlete
							= std::make_shared<sale::Athlete>
								(athlete_id, event_, olympic_games->getID());
						// add medal and store in db
						athlete->addMedal(medal);
						(*this->athletes)[athlete_id] = std::move(athlete);
					}
					else {  // add medal to existing athlete
						this->athletes->at(athlete_id)->addMedal(medal);
						this->athletes->at(athlete_id)->addEvent(event_, olympic_games->getID());
					}

					team->addAthlete(athlete_id);
				}

				// store all the ptrs in appropriate data tables:
				this->teams.emplace_back(std::move(team));
			}
		}
		catch (const std::exception &err) {
			if (do_log_errors) {
				std::cout << std::endl << "Failed to read line: "
						  << line_idx << std::endl;
				std::cout << "Err msg: " << err.what() << std::endl;
			}

			++skipped_lines_count;
		}
		catch (...) {
			if (do_log_errors) {
				std::cout << std::endl << "Failed to read line: "
						  << line_idx << std::endl;
			}
			++skipped_lines_count;
		}
	}

	// do not close here
	//if (in_file.is_open()) in_file.close();

	return skipped_lines_count;
}

int sale::DataBase::parseAthletes(std::ifstream &in_file, bool do_log_errors)
{
	const boost::regex reg_exp
		("(\\d+)!([^!]+)!(M|F)!((?:NA)|\\d+)!((?:NA)|\\d+)!((?:NA)|(?:\\d+.\\d*))");

	// check if integer field has NaN value
	auto isNaN = [] (const char * const c_str) -> bool {
		return std::strcmp(c_str, "NA") == 0;
	};

	auto logException = [] (const std::exception &err) {
		std::cout << std::endl << std::endl
				  << "Error: " << err.what() << std::endl;
	};

	// read line by line:
	int line_idx = 1;
	int skipped_lines_count = 0;
	int num_athletes_to_add = this->athletes->size();

	for (std::string line;
		 std::getline(in_file, line) && num_athletes_to_add > 0;
		 ++line_idx)
	{
		// skip blank lines
		if (line.length() == 0) continue;

		try {
			boost::smatch reg_result;

			if (!boost::regex_match(line, reg_result, reg_exp)) {
				throw sale::exceptions::LineInvalidFormat();
			}

			// do not read in the year from regex if it was
			// already set as the filter
			const int id = std::atoi(reg_result.str(1).c_str());

			// in case this athlete is not in database ignore him
			if (!this->athletes->count(id)) continue;

			auto &athlete = *this->athletes->at(id);

			// group 2 - name
			athlete.setName(std::move(reg_result.str(2)));

			// group 3 - M/F
			char gender = reg_result.str(3)[0];

			// group 4 - age
			const int age = reg_result.str(4) == "NA" ?
							-1  // flag for age was unspecifed
						  : std::atoi(reg_result.str(4).c_str());

			// group 5 - height
			const int height = reg_result.str(5) == "NA" ?
								-1  // falg for age was unspecifed
							 : std::atoi(reg_result.str(5).c_str());

			// group 6 - weight
			const float weight = reg_result.str(6) == "NA" ?
								  -1.  // falg for age was unspecifed
								: std::atof(reg_result.str(6).c_str());

			athlete.setGender(gender);
			athlete.setAge(age);
			athlete.setHeight(height);
			athlete.setWeight(weight);

			--num_athletes_to_add;
		}
		catch (const sale::exceptions::LineInvalidFormat &err) {
			logException(err);
			std::cout << "Line" << line_idx
					  << " did not match regex" << std::endl;
			++skipped_lines_count;
		}
		catch (const std::exception &err) {
			logException(err);
			std::cout << std::endl << "Failed to read line: "
					  << line_idx << std::endl;
			++skipped_lines_count;
		}
		catch (...) {
			std::cout << std::endl << "Failed to read line: "
					  << line_idx << std::endl;
			++skipped_lines_count;
		}
	}

	//if (in_file.is_open()) in_file.close();

	return skipped_lines_count;
}

void sale::DataBase::openAndParseFile
	(const std::string& file_path,
	 std::ostream& os_for_report,
	 bool do_log_report,
	 const std::function<int(std::ifstream &) > &parsing_func)
{
	// using shorter variable name
	std::ostream &os = os_for_report;

	if (do_log_report) {
		os << std::endl << std::endl;
		os << "Opening file:" << std::endl << file_path;
		os << std::endl;
	}

	std::ifstream in_file(file_path);

	if (!in_file.is_open()) {
		throw sale::exceptions::FailedToOpenFile(file_path);
	}

	if (do_log_report) {
		os << "Successfully opened file." << std::endl;
		os << "Loading in and parsing the data.." << std::endl;
	}

	int corrupt_lines_count = parsing_func(in_file);

	if (in_file.is_open()) in_file.close();

	if (do_log_report) {
		os << "Finished loading in and parsing data. (failed to parse "
			<< corrupt_lines_count << " lines)" << std::endl;
	}
}

std::shared_ptr<sale::OlympicGames<sale::DataBase::getAthletePtr>>
	sale::DataBase::getGamePtr(const int year, const char* const game_type) const
{
	if (!this->olympic_games->count(year)) return nullptr;

	auto &year_group = (*this->olympic_games)[year];

	auto does_match =
		[&] (const std::shared_ptr<OlympicGames<getAthletePtr>> &el) -> bool
	{
		return 0 == std::strcmp(game_type, el->getTypeReference().c_str());
	};

	auto found = std::find_if(year_group.rbegin(), year_group.rend(), does_match);

	if (found == year_group.rend()) return nullptr;
	return *found;
}
