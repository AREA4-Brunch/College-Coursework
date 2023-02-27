package data_loaders;


import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;

import java.util.HashMap;
import java.util.TreeMap;
import java.util.ArrayList;

import java.util.stream.Stream;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JProgressBar;

import exceptions.LineInvalidFormat;

import awards.Medal;
import olympics.competitors.Athlete;
import olympics.competitors.Team;
import olympics.countries.Country;
import olympics.events.Event;
import olympics.sports.Sport;
import olympics.olympic_games.OlympicGames;


public class DataBase {

	// ==========================================
	// Attributes:


    private static DataBase instance;

    private static Integer line_idx = 0;
    private static Integer skipped_lines_count = 0;
    private static Integer update_progress_bar_freq = 20;


    //public HashMap<Integer, Team> teams = new HashMap<>();
    public HashMap<Integer, Athlete> athletes = new HashMap<>();
    public HashMap<String, Country> countries = new HashMap<>();
    public HashMap<String, Sport> sports = new HashMap<>();
    public TreeMap <
    	Integer,  // year
		ArrayList<OlympicGames>  // Summer and Winter games
    > olympic_games = new TreeMap<>();


	// ==========================================
	// Constructors and Singleton:


    public static DataBase getInstance() {
        if (DataBase.instance == null)
            DataBase.instance = new DataBase();

        return DataBase.instance;
    }

    public void clearInstance() {
        System.out.println("CLEARING THE DATABASE");

        //this.teams = new HashMap<>();
        this.athletes = new HashMap<>();
        this.countries = new HashMap<>();
        this.sports = new HashMap<>();
        this.olympic_games = new TreeMap<>();
    }

    private DataBase() {

    }


	// ================================
	// Helpers for loading in data:

    
	// Loads in competitors from specified year,
	// if given year is negative loads in all of them.
	// Returns number of unsuccessfully read lines.
	// Does not close the file.
    public int parseCompetitors(File in_file,
                                final int year_filter,
                                JProgressBar progress_bar,
                                boolean do_log_errors)
    	throws java.io.FileNotFoundException
    {
    	BufferedReader br = null;
    	try {
            br = new BufferedReader(new FileReader(in_file));
    	} catch (java.io.FileNotFoundException err) {
    		throw err;
    	}

        Stream<String> stream = br.lines();

        String year_format_str = year_filter < 0?
                                 "(\\d+)"
                               : "(" + year_filter + ")";

        String reg_exp_common_str = year_format_str
    		+ " ([^!]+)!([^!]+)!([^!]+)!([^!]+)!((?:Team)|(?:Individual))!([^!]+)!([^!]+)!((?:Gold)|(?:Silver)|(?:Bronze)|)";
        Pattern reg_exp_common = Pattern.compile( reg_exp_common_str );

        DataBase.line_idx = 0;
        DataBase.skipped_lines_count = 0;
        DataBase.update_progress_bar_freq = 20;  // every 20 lines
        int file_length = 187500;  // just a guess

        stream.forEach(line -> {
            ++line_idx;
		    if (line.length() == 0) return;  // continue to next line

            // update progress bar
            if (update_progress_bar_freq <= 0) {
			    progress_bar.setValue(Math.min(99, (int) (100. * line_idx / file_length)));
                update_progress_bar_freq = 20;
            } else update_progress_bar_freq--;

            // Parse the line:
            try {
                Matcher reg_result = reg_exp_common.matcher(line);
                if (!reg_result.matches()) {
                    if (year_filter < 0) throw new LineInvalidFormat();

                    // could be skipping because the year does not match
                    // so do not throw exception
                    ++skipped_lines_count;
                    return;  // continue to next line
                }

                // do not read in the year from regex if it was
                // already set as the filter
                int year = year_filter >= 0 ?
                            year_filter
                         : Integer.parseInt(reg_result.group(1));

    			// groups: 2 - game type, 3 - city
                OlympicGames oly_games = this.fetchOlympicGames(year, reg_result.group(2));
                if (oly_games == null) {  // these games are new
                	oly_games = new OlympicGames(
            			year,
            			reg_result.group(2),  // game type 
            			reg_result.group(3)   // city
        			);

    				// add to the year's group in database:
    				if (!this.olympic_games.containsKey(year)) {  // create new group
    					ArrayList<OlympicGames> new_group = new ArrayList<>();
    					new_group.add(oly_games);
    					this.olympic_games.put(year, new_group);
    				}
    				else {  // add new games to existing group
    					this.olympic_games.get(year).add(oly_games);
    				}
                }  // else olympic_games points to appropriate games

    			// group 4 - sport's name
                String sport_name = reg_result.group(4);
    			// create sport object or use one from database if exists
            	Sport sport = this.sports.containsKey(sport_name) ?
                			  this.sports.get(sport_name)
                			: new Sport(sport_name);
            	this.sports.put(sport_name, sport);

    			// groups: 5 - event's name,
            	// 		   6 - type of event(team/individual)
        		Event event_ = new Event(
    				reg_result.group(6),
    				reg_result.group(5),
    				oly_games.getID()
				);

        		// add event to the sport, does nothing if
        		// it already exists
    			sport.addEvent(event_);

    			// groups: 7 - country
    			// create country object or use one from database if exists
                String country_name = reg_result.group(7);
    			// create sport object or use one from database if exists
            	Country country = this.countries.containsKey(country_name) ?
                			  	  this.countries.get(country_name)
            			  	    : new Country(country_name);
            	this.countries.put(country_name, country);

    			// group 9 - medal
            	Medal medal = new Medal(
        			reg_result.group(9),
        			oly_games.getID(),
        			country_name,
        			event_
    			);

    			// ! important to update the line last
    			// group 8 - ids
            	line = reg_result.group(8);

    			// read ids of team's players or individual:
            	if (event_.getType().equals(Event.TEventType.Individual)) {
    				// read id of Individual
                    final int athlete_id = Integer.parseInt(line);

                    // create athlete object or use one from database if exists
                    Athlete athlete = this.athletes.containsKey(athlete_id) ?
                                      this.athletes.get(athlete_id)
                                    : new Athlete(athlete_id);

                    country.addAthlete(athlete);
                    oly_games.addAthlete(athlete, country);
                    athlete.addMedal(medal);
                    athlete.addEvent(event_, oly_games.getID());
                    this.athletes.put(athlete_id, athlete);

                } else {  // read ids of team players
                    Team team = new Team(event_, oly_games.getID());
                    //this.teams.put(team.getID(), team);
                    medal.setAsSharedByTeam();
                    team.addMedal(medal);
                    country.addTeam(team);
                    oly_games.addTeam(team, country);

            		Pattern team_ids_regex = Pattern.compile("'(\\d+)'(?:, )?");
                    Matcher reg_team_res = team_ids_regex.matcher(line);

                    while (reg_team_res.find()) {
                        // read id of Individual
                        final int athlete_id = Integer.parseInt(reg_team_res.group(1));

                        // create athlete object or use one from database if exists
                        Athlete athlete = this.athletes.containsKey(athlete_id) ?
                                          this.athletes.get(athlete_id)
                                        : new Athlete(athlete_id);

                        team.addAthlete(athlete);

                        //country.addAthlete(athlete);
                        //oly_games.addAthlete(athlete, country);
                        athlete.addMedal(medal);
                        athlete.addEvent(event_, oly_games.getID());
                        this.athletes.put(athlete_id, athlete);
                    }
            	}

            } catch(LineInvalidFormat err) {
                ++skipped_lines_count;
            } catch (Throwable err) {
            	if (do_log_errors) {
            		System.out.println("" + err);
            	}
                ++skipped_lines_count;
            }
        });


        try {
        	br.close();
        } catch (IOException err) {
        	// ignore it
        }

//        System.out.println("Size teams: " + teams.size());
//        System.out.println("Size athletes: " + athletes.size());
//        System.out.println("Size countries: " + countries.size());
//        System.out.println("Size sports: " + sports.size());
//        System.out.println("Size olympic_games: " + olympic_games.size());

        return skipped_lines_count;
    }

	// Loads in all data about the athletes whose keys
	// are present in the athletes table of this database
	// Returns number of unsuccessfully read lines.
    public int parseAthletes(File in_file,
                             JProgressBar progress_bar,
                             boolean do_log_errors)
    	throws java.io.FileNotFoundException
    {
    	BufferedReader br = null;
    	try {
            br = new BufferedReader(new FileReader(in_file));
    	} catch (java.io.FileNotFoundException err) {
    		throw err;
    	}

        Stream<String> stream = br.lines();
        Pattern reg_exp_common = Pattern.compile( "(\\d+)!([^!]+)!(M|F)!((?:NA)|\\d+)!((?:NA)|\\d+)!((?:NA)|(?:\\d+.\\d*))" );

        DataBase.line_idx = 0;
        DataBase.skipped_lines_count = 0;
        DataBase.update_progress_bar_freq = 20;  // every 20 lines
        int file_length = 135580;  // just a guess

        stream.forEach(line -> {
            ++line_idx;
		    if (line.length() == 0) return;  // continue to next line

            // update progress bar
            if (update_progress_bar_freq <= 0) {
			    progress_bar.setValue(Math.min(99, (int) (100. * line_idx / file_length)));
                update_progress_bar_freq = 20;
            } else update_progress_bar_freq--;

            // Parse the line:
            try {
                Matcher reg_result = reg_exp_common.matcher(line);
                if (!reg_result.matches()) {
                    throw new LineInvalidFormat();
                }

                final int id = Integer.parseInt(reg_result.group(1));

    			// in case this athlete is not in database ignore him
                if (!this.athletes.containsKey(id)) return;
                
                final Athlete athlete = this.athletes.get(id);

                // group 2 - name
                athlete.setName(reg_result.group(2));

    			// group 3 - M/F
                athlete.setGender(reg_result.group(3).charAt(0));

    			// group 4 - age
    			final int age = reg_result.group(4).equals("NA") ?
    							-1  // flag for age was unspecified
    						  : Integer.parseInt(reg_result.group(4));

    			// group 5 - height
    			final int height =  reg_result.group(5).equals("NA") ?
									-1  // flag for age was unspecified
								  : Integer.parseInt(reg_result.group(5));

    			// group 6 - weight
    			final float weight = reg_result.group(6).equals("NA") ?
    								  -1.f  // flag for age was unspecified
								    : Float.parseFloat(reg_result.group(6));

    			athlete.setAge(age);
    			athlete.setHeight(height);
    			athlete.setWeight(weight);

            } catch(LineInvalidFormat err) {
                ++skipped_lines_count;
            } catch (Throwable err) {
            	if (do_log_errors) {
            		System.out.println("" + err);
            	}
                ++skipped_lines_count;
            }
        });
			

        try {
        	br.close();
        } catch (IOException err) {
        	// ignore it
        }
        
        return skipped_lines_count;
    }

	// return olympic game object that has given params
	// if such exists, null otherwise
    private OlympicGames fetchOlympicGames(final int year, String game_type) {
    	if (!this.olympic_games.containsKey(year))
    		return null;  // no such olympic games exist

    	ArrayList<OlympicGames> year_group = this.olympic_games.get(year);
    	for (int i = year_group.size() - 1; i >= 0; --i) {
    		if (year_group.get(i).getType().equals(game_type)) {
    			// found the games
    			return year_group.get(i);
    		}
    	}

    	// there are no such games in db yet
    	return null;
    }

}
