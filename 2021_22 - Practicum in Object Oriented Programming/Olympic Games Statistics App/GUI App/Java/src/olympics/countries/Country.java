package olympics.countries;

import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.LinkedList;

import olympics.competitors.Athlete;
import olympics.competitors.Team;


public class Country {

	// ==========================================
	// Attributes:


	// name of the country
	private String name;

	// set of athletes that played for this country
	private HashSet<Athlete> athletes = new HashSet<>();

	// list of teams that competed for this country
	List<Team> teams = new LinkedList<>();


	// ==========================================
    // Constructors:


	public Country(String name) {
		this.name = name;
	}
	

	// ==========================================
	// Manage Competitors:
	
	
	public void addAthlete(Athlete athlete) {
		this.athletes.add(athlete);
	}

	public void addTeam(Team team) {
		this.teams.add(team);
	}


	// ==========================================
	// Getters:


	public String getName() {
		return name;
	}


	public HashSet<Athlete> getAthlete() {
		return athletes;
	}


	public List<Team> getTeams() {
		return teams;
	}


	// ==========================================
	// Hashing:
	

	@Override
	public int hashCode() {
		return Objects.hash(name);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Country other = (Country) obj;
		return Objects.equals(name, other.name);
	}

}
