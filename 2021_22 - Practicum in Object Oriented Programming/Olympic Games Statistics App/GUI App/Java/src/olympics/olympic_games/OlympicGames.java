package olympics.olympic_games;

import java.util.HashMap;
import java.util.List;
import java.util.Objects;
import java.util.LinkedList;
import java.util.Set;

import utils.Pair;

import olympics.competitors.Athlete;
import olympics.competitors.Team;
import olympics.countries.Country;



public class OlympicGames {

	// ==========================================
	// Attributes:


	static int ID;
	final int id = ++ID;

	int year;
	String type;
	String city;

	// map of athletes and countries they represented
	HashMap<Athlete, Country> athletes = new HashMap<>();

	// list of teams and countries they competed for on
	// these games
	List<Pair<Team, Country>> teams = new LinkedList<>();


	// ==========================================
	// Constructors:


	public OlympicGames(int year, String type, String city) {
		super();
		this.year = year;
		this.type = type;
		this.city = city;
	}


	// ==========================================
	// Adding Competitors:

	

	/**
	 * 	Adds given athlete representing given country individually
	 * 	if no such athlete represents any other country individually
	 * 	on these games, returns bool if athlete was added.
	 */
	public boolean addAthlete(Athlete athlete, Country country) {
		if (this.athletes.containsKey(athlete)) {
			return false;
		}
		this.athletes.put(athlete, country);
		return true;
	}

	public void addTeam(Team team, Country country) {
		this.teams.add(new Pair<Team, Country> (team, country));
	}


	// ==========================================
	// Removing competitors:


	/**
	 * Removes all entries in the athletes countries map that
	 * satisfy the criterion and returns number of those entries.
	 * @return Number of removed athlete-country pairs.
	 */
	public int removeFromAthletesCountries(lambdas.AthleteCountryCriterion criterion) {
		final int old_size = this.athletes.size();

		this.athletes.entrySet().removeIf(entry -> {
			return criterion.call(entry.getKey(), entry.getValue());
		});

		return old_size - this.athletes.size();
	}

	/**
	 * Removes all entries in the teams countries map that
	 * satisfy the criterion and returns number of those entries.
	 * @return Number of removed athlete-country pairs.
	 */
	public int removeFromTeamsCountries(lambdas.TeamCountryCriterion criterion) {
		final int old_size = this.teams.size();

		this.teams.removeIf(team_country -> {
			return criterion.call(team_country.first, team_country.second);
		});

		return old_size - this.teams.size();
	}

	/// removes all teams records from current games
	public void dropTeamsCountries() {
		this.teams.clear();
	}
	
	/// removes all individuals records from current games
	public void dropAthletesCountries() {
		this.athletes.clear();
	}

	
	// ==========================================
	// Getters:


	public int getID() {
		return this.id;
	}

	public int getYear() {
		return year;
	}

	public String getType() {
		return type;
	}

	public String getCity() {
		return city;
	}

	public HashMap<Athlete, Country> getAthletesCountries() {
		return athletes;
	}

	public List<Pair<Team, Country>> getTeamsCountries() {
		return teams;
	}

	/**
	 * Returns country the given athlete represented on these
	 * olympic games.
	 * @param athlete
	 * @return Country the given athlete represented on these
	 * 		   olympic games, null in case athlete is not present.
	 */
	public Country getAthletesCountry(Athlete athlete) {
		return this.athletes.get(athlete);
	}

	/**
	 * Returns set of athletes that participated in these games.
	 * @return Set of athletes that participated in these games.
	 */
	public Set<Athlete> getAllAthletes() {
		return this.athletes.keySet();
	}


	// ==========================================
	// Hashing:


	@Override
	public int hashCode() {
		return Objects.hash(id);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		OlympicGames other = (OlympicGames) obj;
		return id == other.id;
	}

}
