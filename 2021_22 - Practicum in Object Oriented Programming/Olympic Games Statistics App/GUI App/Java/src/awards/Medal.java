package awards;

import java.util.Objects;
import exceptions.InvalidMedalStrFormat;
import olympics.events.Event;


public class Medal implements Comparable<Medal> {

	public enum TMedal {
		Gold(0), Silver(1), Bronze(2), NoMedal(3);

		private int value;
		private TMedal(final int value) { this.value = value; }
		private int getValue() { return this.value; }
	};


	// ==========================================
	// Attributes:


	private TMedal medal = Medal.TMedal.NoMedal;
	private int olympic_games_id;
	private boolean is_shared_by_team = false;
	private String country_name;
	private Event event_;


	// ==========================================
    // Constructors:

	
	public Medal(TMedal medal, final int oly_games_id,
				String country_name, Event event_)
	{
		this.medal = medal;
		this.olympic_games_id = oly_games_id;
		this.country_name = country_name;
		this.event_ = event_;
	}


	public Medal(String medal_str, final int oly_games_id,
				 String country_name, Event event_)
		throws InvalidMedalStrFormat
	{
		this.olympic_games_id = oly_games_id;
		this.country_name = country_name;
		this.event_ = event_;

		if (medal_str.equals("")) {
			this.medal = TMedal.NoMedal;
		} else if (medal_str.equals("Gold")) {
			this.medal = TMedal.Gold;
		} else if (medal_str.equals("Silver")) {
			this.medal = TMedal.Silver;
		} else if (medal_str.equals("Bronze")) {
			this.medal = TMedal.Bronze;
		} else {
			throw new InvalidMedalStrFormat();
		}
	}


	// ==========================================
	// Getters:


	public int getMedalScore() {
		return this.medal.getValue();
	}

	public int getOlympicGamesID() {
		return this.olympic_games_id;
	}

	public TMedal getMedal() {
		return this.medal;
	}

	public boolean isSharedByTeam() {
		return this.is_shared_by_team;
	}

	public String getCountryName() {
		return this.country_name;
	}
	
	public Event getEvent() {
		return this.event_;
	}


	// ==========================================
	// Setters:


	public void setAsSharedByTeam() {
		this.is_shared_by_team = true;
	}

	
	// ==========================================
	// Comparisons:


	@Override
	public int hashCode() {
		return Objects.hash(medal);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Medal other = (Medal) obj;
		return medal.getValue() == other.medal.getValue();
	}

	@Override
	public int compareTo(Medal other) {
		return other.medal.getValue() - this.medal.getValue();
	}
	
	@Override
	public String toString() {
		return this.medal.toString();
	}

}
