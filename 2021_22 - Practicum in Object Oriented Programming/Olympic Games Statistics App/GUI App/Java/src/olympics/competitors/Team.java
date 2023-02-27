package olympics.competitors;

import java.util.ArrayList;

import olympics.events.Event;


public class Team extends Competitor {
	
	// ==========================================
	// Attributes:


	static int ID = 0;
	final int id = ++ID;

	// athletes in the team
	ArrayList<Athlete> athletes = new ArrayList<>();


	// ==========================================
    // Constructors:


	public Team(Event event_, final int oly_games_id) {
		this.addEvent(event_, oly_games_id);
	}

	public Team(final int max_num_athletes,
				Event event_, final int oly_games_id)
	{
		this(event_, oly_games_id);
		this.athletes.ensureCapacity(max_num_athletes);
	}


	// ==========================================
	// Public Methods:


	public void addAthlete(Athlete athlete) {
		this.athletes.add(athlete);
	}
	
	
	// ==========================================
	// Getters:


	public int getID() {
		return this.id;
	}

	public ArrayList<Athlete> getAthletes() {
		return this.athletes;
	}

	@Override
	public void shrinkToFit() {
		super.shrinkToFit();
		this.athletes.trimToSize();
	}
	
}
