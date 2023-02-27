package filter;


import data_loaders.DataBase;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;

import utils.Pair;

import awards.Medal;

import olympics.competitors.Athlete;
import olympics.competitors.Competitor;
import olympics.competitors.Team;
import olympics.countries.Country;
import olympics.events.Event;
import olympics.sports.Sport;
import olympics.olympic_games.OlympicGames;

import javax.swing.JProgressBar;


public class Filter {

	// ==========================================
	// Attributes:


	private Integer update_progress_bar_freq = 20;


	DataBase db;
	JProgressBar progress_bar;

	private HashMap<Integer, Pair<Double, Integer>> summer_games_data;
	private HashMap<Integer, Pair<Double, Integer>> winter_games_data;


	// ==========================================
	// Constructors:


	public Filter(DataBase db,
				  JProgressBar progress_bar)
	{
		super();
		this.db = db;
		this.progress_bar = progress_bar;
	}


	// ==========================================
	// Public Filtering Methods:


	public void PerformFilter(int[] num_athletes,
							  int[] num_disciplines,
							  float[] avg_height,
							  float[] avg_weight,
							  HashMap<String, Number> num_participations,
							  Integer year,
							  String sport,
							  String country,
							  String event_type,
							  String medal)
	{
		ArrayList<String> participations_serialized = this.getParticipationsSerialized();
		// set serialized participations
		String[] serialized = participations_serialized.toArray(new String[0]);

		String[] filtered_serialized = this.getParticipationsDeserialized(
			serialized, year, sport, country, event_type, medal
		);

		// System.out.println("Got from cpp:");
		// for (String str : filtered_serialized) {
		// 	System.out.println(str);
		// }


		// go through filtered athlete ids and update progress bar
		final int num_lines = filtered_serialized[2].length() + filtered_serialized[3].length();
		this.update_progress_bar_freq = 20;
		int line_idx = 0;

		// Extract the data from filtered_serialized database:

		num_athletes[0] = Integer.parseInt(filtered_serialized[0]);
		num_disciplines[0] = Integer.parseInt(filtered_serialized[1]);

		// Load in countries and their number of participations
		Scanner scanner_country = new Scanner(filtered_serialized[3]);
		Scanner scanner_num_participations = new Scanner(filtered_serialized[4]);
		for (line_idx = 0; scanner_country.hasNextLine(); ++line_idx) {
			num_participations.put(
				scanner_country.nextLine(),
				scanner_num_participations.nextInt()
			);

			// update progress bar
			if (update_progress_bar_freq <= 0) {
				this.progress_bar.setValue(
					  this.progress_bar.getValue()
					+ Math.min(25, (int) (25. * line_idx / num_lines))
				);
				update_progress_bar_freq = 20;
			} else update_progress_bar_freq--;
		}
		scanner_country.close();
		scanner_num_participations.close();

		// calculate average athlete height and weight
		double total_height = 0.0;
		int height_count = 0;
		double total_weight = 0.0;
		int weight_count = 0;

		Scanner scanner = new Scanner(filtered_serialized[2]);
		for ( ; scanner.hasNextInt(); ++line_idx) {
			int id = scanner.nextInt();
			Athlete athlete = this.db.athletes.get(id);

			// skip NA values
			if (athlete.getWeight() > 0) {
				total_weight += athlete.getWeight();
				weight_count++;
			}

			// skip NA values
			if (athlete.getHeight() > 0) {
				total_height += athlete.getHeight();
				height_count++;
			}

			// update progress bar
			if (update_progress_bar_freq <= 0) {
				this.progress_bar.setValue(
					  this.progress_bar.getValue()
					+ Math.min(24, (int) (24. * line_idx / num_lines))
				);
				update_progress_bar_freq = 20;
			} else update_progress_bar_freq--;
		}
		scanner.close();

		if (weight_count > 0) avg_weight[0] = (float) (total_weight / weight_count);
		else avg_weight[0] = 0.f;

		if (height_count > 0) avg_height[0] = (float) (total_height / height_count);
		else avg_height[0] = 0.f;
	}
	

	// ==========================================
	// Private Filtering Methods:


	private ArrayList<String> getParticipationsSerialized() {
		// max number of lines after serialization: 219942
        this.update_progress_bar_freq = 20;  // every 20 lines

    	ArrayList<String> serialized_rows = new ArrayList<>();

		HashSet<Athlete> already_processed_athletes = new HashSet<>();

		// go through all olympic games
		this.db.olympic_games.forEach( (Integer year, ArrayList<OlympicGames> oly_games_group) -> {
			oly_games_group.forEach( (OlympicGames oly_games) -> {
				// loop through all individuals:
				oly_games.getAthletesCountries().forEach( (Athlete athlete, Country country) -> {
					if (already_processed_athletes.contains(athlete)) return;  // skip it
					already_processed_athletes.add(athlete);
					this.processCompetitor(serialized_rows, athlete, athlete.getId(), oly_games, country);
				});

				// No need to go through teams to reach the athletes if they are also
				// added as in athletes map of olympic games while loading in database

				// loop through all teams:
				oly_games.getTeamsCountries().forEach( (Pair<Team, Country> team_country) -> {
					Team team = team_country.first;
					Country country = team_country.second;

					team.getAthletes().forEach((Athlete athlete) -> {
						if (already_processed_athletes.contains(athlete)) return;  // skip it
						already_processed_athletes.add(athlete);
						this.processCompetitor(serialized_rows, athlete, athlete.getId(), oly_games, country);
					});

					this.processCompetitor(serialized_rows, team, -1 * team.getID(), oly_games, country);
				});
			});
		});

		return serialized_rows;
	}

	private void processCompetitor(ArrayList<String> serialized_rows, Competitor competitor,
								   int competitor_id, OlympicGames oly_games,
								   Country country)
	{
		competitor.getEvents().forEach( (Pair<Event, ArrayList<Integer>> event_games) -> {
			Event event_ = event_games.first;
			int oly_games_id = oly_games.getID();

			for (Integer games_id : event_games.second) {
				if (games_id != oly_games_id) continue;
				// found event competitor participated in on these games
				
				// find sport of this event
				Sport found_sport = null;
				for (Sport sport : this.db.sports.values()) {
					if (sport.doesEventExist(event_)) {
						found_sport = sport;
						break;
					}
				}

				// find a medal for this event if it was won
				Medal found_medal = null;

				for (Medal medal : competitor.getMedals()) {
					if (medal.getOlympicGamesID() != oly_games_id)
						continue;

					if (!medal.getCountryName().equals(country.getName()))
						continue;

					if (medal.getEvent().equals(event_)) {
						found_medal = medal;
						break;
					}
				}

		    	StringBuilder sb = new StringBuilder();

				// form the string to represent this event/entry

				// negative sign of id for tean ids
				sb.append(event_.getType() == Event.TEventType.Individual ? 0 : 1).append("!");  // 0/1 Individual/Team
				sb.append(oly_games.getYear()).append("!");  // year
				sb.append(found_medal != null ? found_medal.toString() : "NoMedal").append("!");  // medal
				sb.append(country.getName()).append("!");  // country
				sb.append(found_sport.getName()).append("!");  // sport
				sb.append(competitor_id).append("!");  // athlete_id or team_id
				sb.append(event_.getName()).append("!");  // discipline
				//sb.append(oly_games_id).append("!");  // oly_games id

				//sb.append("\n");

				// update progress bar
				if (update_progress_bar_freq <= 0) {
					this.progress_bar.setValue
						(Math.min(50, (int) (50 * serialized_rows.size() / 220000)));

					update_progress_bar_freq = 20;
				} else update_progress_bar_freq--;

				serialized_rows.add(sb.toString());
			}
		});
	}

	public void getSummerWinterTotalNumDisciplines
		(ArrayList<Pair<Integer, Integer>> summer_games_data,
		 ArrayList<Pair<Integer, Integer>> winter_games_data,
		 final int year_start_, final int year_end_)
	{
		final int year_start = year_start_ < 0 ? Integer.MIN_VALUE : year_start_;
		final int year_end = year_end_ < 0 ? Integer.MAX_VALUE : year_end_;

        this.update_progress_bar_freq = 20;  // every 20 lines
		final int num_athletes = this.db.athletes.size();

		HashSet<Athlete> already_processed_athletes = new HashSet<>();
		// summer_games[year] = events memorized for these games
		HashMap<Integer, HashSet<Event>> summer_games = new HashMap<>();
		HashMap<Integer, HashSet<Event>> winter_games = new HashMap<>();

		// go through all olympic games
		this.db.olympic_games.forEach( (Integer year, ArrayList<OlympicGames> oly_games_group) -> {
			// filter by years:
			if (year < year_start || year > year_end)
				return;

			oly_games_group.forEach( (OlympicGames oly_games) -> {
				final HashMap<Integer, HashSet<Event>> cur_games
					= oly_games.getType().equals("Summer") ? summer_games : winter_games;

				// for each year create new group of events to track
				if (!cur_games.containsKey(year)) {
					cur_games.put(year, new HashSet<Event>());
				}

				// loop through all individuals:
				oly_games.getAthletesCountries().forEach( (Athlete athlete, Country country) -> {
					if (already_processed_athletes.contains(athlete)) return;  // skip it
					already_processed_athletes.add(athlete);
					this.processCompetitor(cur_games.get(year), athlete, oly_games);

					// update progress bar
					if (update_progress_bar_freq <= 0) {
						this.progress_bar.setValue
							(Math.min(99, (int) (100 * already_processed_athletes.size() / num_athletes)));

						update_progress_bar_freq = 20;
					} else update_progress_bar_freq--;

				});

				// No need to go through teams to reach the athletes if they are also
				// added as in athletes map of olympic games while loading in database

				// loop through all teams:
				oly_games.getTeamsCountries().forEach( (Pair<Team, Country> team_country) -> {
					Team team = team_country.first;
					//Country country = team_country.second;
					this.processCompetitor(cur_games.get(year), team, oly_games);
				});
			});
		});

		summer_games.forEach( (Integer year, HashSet<Event> events) -> {
			summer_games_data.add(new Pair<Integer, Integer> ( year, events.size() ));
		});

		winter_games.forEach( (Integer year, HashSet<Event> events) -> {
			winter_games_data.add(new Pair<Integer, Integer> ( year, events.size() ));
		});

	}

	private void processCompetitor(HashSet<Event> cur_games,
								   Competitor competitor,
								   OlympicGames oly_games)
	{
		competitor.getEvents().forEach( (Pair<Event, ArrayList<Integer>> event_games) -> {
			Event event_ = event_games.first;
			int oly_games_id = oly_games.getID();

			for (Integer games_id : event_games.second) {
				if (games_id != oly_games_id) continue;

				// found event competitor participated in on these games
				cur_games.add(event_);
			}
		});
	}

	public synchronized void getSummerWinterAvgHeight
		(ArrayList<Pair<Integer, Integer>> summer_games_data,
		 ArrayList<Pair<Integer, Integer>> winter_games_data,
		 final int year_start_, final int year_end_)
	{
		final int year_start = year_start_ < 0 ? Integer.MIN_VALUE : year_start_;
		final int year_end = year_end_ < 0 ? Integer.MAX_VALUE : year_end_;

		this.summer_games_data = new HashMap<>();
		this.winter_games_data = new HashMap<>();

		final int num_athletes = this.processAllAthletesOnce( (Athlete athlete, OlympicGames oly_games) -> {
			// in case of NA ignore them
			if (athlete.getHeight() < 0) return;

			if (oly_games.getYear() < year_start || oly_games.getYear() > year_end)
				return;

			if (oly_games.getType().equals("Summer")) {
				Pair<Double, Integer> sum_count_pair
					= Filter.this.summer_games_data.getOrDefault(oly_games.getYear(),
						new Pair<Double, Integer> (0., 0));

				sum_count_pair.first = sum_count_pair.first + athlete.getHeight();
				++sum_count_pair.second;

				Filter.this.summer_games_data.put(oly_games.getYear(), sum_count_pair);
			} else {  // they are Winter
				Pair<Double, Integer> sum_count_pair
					= Filter.this.winter_games_data.getOrDefault(oly_games.getYear(),
						new Pair<Double, Integer> (0., 0));

				sum_count_pair.first = sum_count_pair.first + athlete.getHeight();
				++sum_count_pair.second;

				Filter.this.winter_games_data.put(oly_games.getYear(), sum_count_pair);
			}
		});

		// add to arrays average values:
		this.summer_games_data.forEach( (Integer year, Pair<Double, Integer> sum_count_pair) -> {
			summer_games_data.add(new Pair<Integer, Integer> (
				year, sum_count_pair.second > 0 ? (int) (sum_count_pair.first / sum_count_pair.second) : 0
			));
		});

		this.winter_games_data.forEach( (Integer year, Pair<Double, Integer> sum_count_pair) -> {
			winter_games_data.add(new Pair<Integer, Integer> (
				year, sum_count_pair.second > 0 ? (int) (sum_count_pair.first / sum_count_pair.second) : 0
			));
		});

		// clear out the data
		this.summer_games_data = null;
		this.winter_games_data = null;
	}

	public synchronized void getSummerWinterAvgWeight
		(ArrayList<Pair<Integer, Integer>> summer_games_data,
		 ArrayList<Pair<Integer, Integer>> winter_games_data,
		 final int year_start_, final int year_end_)
	{
		final int year_start = year_start_ < 0 ? Integer.MIN_VALUE : year_start_;
		final int year_end = year_end_ < 0 ? Integer.MAX_VALUE : year_end_;

		this.summer_games_data = new HashMap<>();
		this.winter_games_data = new HashMap<>();

		final int num_athletes = this.processAllAthletesOnce( (Athlete athlete, OlympicGames oly_games) -> {
			// in case of NA ignore them
			if (athlete.getWeight() < 0.f) return;

			if (oly_games.getYear() < year_start || oly_games.getYear() > year_end)
				return;

			if (oly_games.getType().equals("Summer")) {
				Pair<Double, Integer> sum_count_pair
					= Filter.this.summer_games_data.getOrDefault(oly_games.getYear(),
						new Pair<Double, Integer> (0., 0));

				sum_count_pair.first = sum_count_pair.first + athlete.getWeight();
				++sum_count_pair.second;

				Filter.this.summer_games_data.put(oly_games.getYear(), sum_count_pair);
			} else {  // they are Winter
				Pair<Double, Integer> sum_count_pair
					= Filter.this.winter_games_data.getOrDefault(oly_games.getYear(),
						new Pair<Double, Integer> (0., 0));

				sum_count_pair.first = sum_count_pair.first + athlete.getWeight();
				++sum_count_pair.second;

				Filter.this.winter_games_data.put(oly_games.getYear(), sum_count_pair);
			}
		});

		// add to arrays average values:
		this.summer_games_data.forEach( (Integer year, Pair<Double, Integer> sum_count_pair) -> {
			summer_games_data.add(new Pair<Integer, Integer> (
				year, sum_count_pair.second > 0 ? (int) (sum_count_pair.first / sum_count_pair.second) : 0
			));
		});

		this.winter_games_data.forEach( (Integer year, Pair<Double, Integer> sum_count_pair) -> {
			winter_games_data.add(new Pair<Integer, Integer> (
				year, sum_count_pair.second > 0 ? (int) (sum_count_pair.first / sum_count_pair.second) : 0
			));
		});

		// clear out the data
		this.summer_games_data = null;
		this.winter_games_data = null;
	}

	/**
	 * @return number of athletes processed
	 */
	private int processAllAthletesOnce(lambdas.ProcessAthleteOlympicGames process_athlete_func) {
        this.update_progress_bar_freq = 20;  // every 20 lines
		final int num_athletes = this.db.athletes.size();

		HashSet<Athlete> already_processed_athletes = new HashSet<>();

		// go through all olympic games
		this.db.olympic_games.forEach( (Integer year, ArrayList<OlympicGames> oly_games_group) -> {
			oly_games_group.forEach( (OlympicGames oly_games) -> {
				// loop through all individuals:
				oly_games.getAthletesCountries().forEach( (Athlete athlete, Country country) -> {
					if (already_processed_athletes.contains(athlete)) return;  // skip it
					already_processed_athletes.add(athlete);
					process_athlete_func.call(athlete, oly_games);

					// update progress bar
					if (update_progress_bar_freq <= 0) {
						this.progress_bar.setValue
							(Math.min(99, (int) (100 * already_processed_athletes.size() / num_athletes)));

						update_progress_bar_freq = 20;
					} else update_progress_bar_freq--;

				});

				// No need to go through teams to reach the athletes if they are also
				// added as in athletes map of olympic games while loading in database

				// loop through all teams:
				oly_games.getTeamsCountries().forEach( (Pair<Team, Country> team_country) -> {
					Team team = team_country.first;
					//Country country = team_country.second;

					team.getAthletes().forEach((Athlete athlete) -> {
						if (already_processed_athletes.contains(athlete)) return;  // skip it
						already_processed_athletes.add(athlete);
						process_athlete_func.call(athlete, oly_games);

						// update progress bar
						if (update_progress_bar_freq <= 0) {
							this.progress_bar.setValue
								(Math.min(99, (int) (100 * already_processed_athletes.size() / num_athletes)));

							update_progress_bar_freq = 20;
						} else update_progress_bar_freq--;

					});
				});
			});
		});

		return already_processed_athletes.size();
	}


	// ==========================================
	// Private Native Filtering Methods:


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
	private native String[] getParticipationsDeserialized
		(String[] serialized, int year, String sport,
		 String country, String event_type, String medal);

}
