package olympics.events;

import java.util.Objects;

public class Event {

	public enum TEventType {
		Team, Individual;
	};


	// ==========================================
	// Attributes:


	TEventType event_type = TEventType.Team;
	String name;
	int oly_games_id = -1;


    // ==========================================
	// Constructors:


	/**
	 * 
	 * @param event_type
	 * @param name
	 * @param oly_games_id - optional, -1 is default
	 */
	public Event(String event_type_str,
				 String name,
				 Integer oly_games_id)
	{
		this.name = name;

		if (oly_games_id != null)
			this.oly_games_id = oly_games_id;

		if (event_type_str.equals("Team")) {
			this.event_type = TEventType.Team;
		} else {
			this.event_type = TEventType.Individual;
		}
	}

	/**
	 * 
	 * @param event_type
	 * @param name
	 * @param oly_games_id - optional, -1 is default
	 */
	public Event(TEventType event_type,
				 String name,
				 Integer oly_games_id)
	{
		this.event_type = event_type;
		this.name = name;
		if (oly_games_id != null)
			this.oly_games_id = oly_games_id;
	}

	
	// ==========================================
	// Getters:

	
	public TEventType getType() {
		return event_type;
	}

	public String getName() {
		return name;
	}

	public int getOlympicGamesID() {
		return oly_games_id;
	}

	
	// ==========================================
	// Hashing:


	@Override
	public int hashCode() {
		return Objects.hash(event_type, name);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Event other = (Event) obj;
		return event_type == other.event_type
			&& Objects.equals(name, other.name);
	}
	
}
