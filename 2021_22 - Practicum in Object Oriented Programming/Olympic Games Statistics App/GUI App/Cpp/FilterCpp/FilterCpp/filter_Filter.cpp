#include "filter_Filter.h"

#include <string>
#include <unordered_set>
#include <unordered_map>



/**
* 
* @param serialized - serialized String[] of participations of strings:
* 						0 - Individual	Gold/.../NoMedal
* 						"0/1     ! year !  medal ! country ! sport ! ! athlete_id/team_id !  discipline ! 
* 
* @return String array after filtering:
* 			1st string "num_athletes"
* 			2nd string "tot_num_disc"
* 			3rd string "athlete_id\n" ...
* 			4th string "Country\n"
* 			5th string "num_participations_corresponding_to_country \n"
*/
JNIEXPORT jobjectArray JNICALL Java_filter_Filter_getParticipationsDeserialized
	(JNIEnv* env, jobject self,
		jobjectArray participations, jint year_, jstring sport_,
		jstring country_, jstring event_type_, jstring medal_)
{
	// Read in the arguments:
	const int num_participations = env->GetArrayLength(participations);

	const int year_filter = year_;
	const std::string sport_filter(env->GetStringUTFChars(sport_, 0));
	const std::string country_filter(env->GetStringUTFChars(country_, 0));
	const std::string event_type_filter(env->GetStringUTFChars(event_type_, 0));
	const std::string medal_filter(env->GetStringUTFChars(medal_, 0));

	bool is_year_filter_on = year_filter >= 0;
	bool is_sport_filter_on = sport_filter.length() > 0;
	bool is_country_filter_on = country_filter.length() > 0;
	bool is_event_type_filter_on = event_type_filter.length() > 0;
	bool is_medal_filter_on = medal_filter.length() > 0;

	bool should_be_team = std::strcmp(event_type_filter.c_str(), "Team") == 0;


	std::unordered_set<int> athletes_ids;
	std::unordered_set<std::string> disciplines_individual;
	std::unordered_set<std::string> disciplines_team;

	// country_participations[Country name] = num of participations
	std::unordered_map<std::string, int> country_participations;


	// process each line of the serialized participations
	for (int line_idx = 0; line_idx < num_participations; ++line_idx) {
		jstring line = (jstring) (env->GetObjectArrayElement(participations, line_idx));
		const char *line_raw = env->GetStringUTFChars(line, 0);

		std::string cur_line(line_raw);

		// parse the line and filter as you go:
		int prev_idx = 0;
		int index = 0;

		// read in event_type
		for ( ; cur_line[index] != '!'; ++index);
		bool is_team = cur_line[0] == '1';

		if (is_event_type_filter_on) {
			if (should_be_team && !is_team || !should_be_team && is_team) {
				// does not pass event_type_filter
				env->ReleaseStringUTFChars(line, line_raw);
				continue;
			}
		}

		// read in year of olympic games
		for (prev_idx = ++index; cur_line[index] != '!'; ++index);
		const int cur_year = std::atoi(cur_line.substr(prev_idx, index - prev_idx).c_str());

		if (is_year_filter_on) {
			if (cur_year != year_filter) {
				// does not pass the filter
				env->ReleaseStringUTFChars(line, line_raw);
				continue;
			}
		}

		// read in medal's name
		for (prev_idx = ++index; cur_line[index] != '!'; ++index);
		const std::string medal = cur_line.substr(prev_idx, index - prev_idx);

		if (is_medal_filter_on) {
			if (std::strcmp(medal_filter.c_str(), medal.c_str()) != 0) {
				// does not pass the filter
				env->ReleaseStringUTFChars(line, line_raw);
				continue;
			}
		}

		// read in country's name
		for (prev_idx = ++index; cur_line[index] != '!'; ++index);
		const std::string country_name = cur_line.substr(prev_idx, index - prev_idx);

		if (is_country_filter_on) {
			if (std::strcmp(country_filter.c_str(), country_name.c_str()) != 0) {
				// does not pass the filter
				env->ReleaseStringUTFChars(line, line_raw);
				continue;
			}
		}

		// read in sport's name
		for (prev_idx = ++index; cur_line[index] != '!'; ++index);
		const std::string sport_name = cur_line.substr(prev_idx, index - prev_idx);

		if (is_sport_filter_on) {
			if (std::strcmp(sport_filter.c_str(), sport_name.c_str()) != 0) {
				// does not pass the filter
				env->ReleaseStringUTFChars(line, line_raw);
				continue;
			}
		}

		// read in athlete/team_id
		for (prev_idx = ++index; cur_line[index] != '!'; ++index);
		const int id = std::atoi(cur_line.substr(prev_idx, index - prev_idx).c_str());

		// read in discipline's name
		for (prev_idx = ++index; cur_line[index] != '!'; ++index);
		const std::string discipline_name = cur_line.substr(prev_idx, index - prev_idx);


		// line has passed all filters:
		if (id >= 0) {  // process single athlete
			athletes_ids.insert(id);
		}
		
		if (is_team) disciplines_individual.insert(discipline_name);
		else disciplines_team.insert(discipline_name);

		// if team event type then only count if id indicates a team (is negative)
		// otherwise its definitely athlete id
		if ((is_team  && id < 0) || !is_team) {
			if (country_participations.count(country_name))
				++country_participations[country_name];
			else country_participations[country_name] = 1;
		}

		env->ReleaseStringUTFChars(line, line_raw);
	}


	// Set the strings as output

	jobjectArray filtered_participations
		= (jobjectArray) env->NewObjectArray(
			5, env->FindClass("java/lang/String"), env->NewStringUTF("")
		);

	int out_idx = 0;

	// set number of athletes
	env->SetObjectArrayElement(
		filtered_participations, out_idx++,
		env->NewStringUTF(std::to_string(athletes_ids.size()).c_str())
	);

	// set total number of disciplines
	env->SetObjectArrayElement(
		filtered_participations, out_idx++,
		env->NewStringUTF(std::to_string(disciplines_individual.size() + disciplines_team.size()).c_str())
	);

	// set all filtered of athletes ids
	std::string athletes_ids_string = "";
	for (const int id : athletes_ids) {
		athletes_ids_string += std::to_string(id) + "\n";
	}

	env->SetObjectArrayElement(
		filtered_participations, out_idx++,
		env->NewStringUTF(athletes_ids_string.c_str())
	);

	// set all countries
	std::string countries_str = "";
	std::string num_participations_string = "";

	for (const auto &kv : country_participations) {
		countries_str += kv.first + "\n";
		num_participations_string += std::to_string(kv.second) + "\n";
	}

	env->SetObjectArrayElement(
		filtered_participations, out_idx++,
		env->NewStringUTF(countries_str.c_str())
	);

	env->SetObjectArrayElement(
		filtered_participations, out_idx++,
		env->NewStringUTF(num_participations_string.c_str())
	);

	// for (int i = 0; i < 5; ++i) {
	// 	std::string msg = "Hello " + std::to_string(i);

	// 	if (year == 2021) {
	// 		msg += "_2021";
	// 	}

	// 	env->SetObjectArrayElement(filtered_participations, i, env->NewStringUTF(msg.c_str()));
	// }

	return filtered_participations;
}
