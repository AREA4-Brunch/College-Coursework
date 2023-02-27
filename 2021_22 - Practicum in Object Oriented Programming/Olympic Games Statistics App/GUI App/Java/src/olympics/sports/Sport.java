package olympics.sports;

import olympics.events.Event;
import java.util.HashSet;
import java.util.Objects;


public class Sport {

	// ==========================================
	// Attributes:
	

	String name;

	// set of events
	HashSet<Event> events = new HashSet<>();


    // ==========================================
	// Constructors:

	
	public Sport(String name) {
		this.name = name;
	}


	// ==========================================
	// Managing Events:

	
	/**
	 * Adds given event to the set of events if it
	 * is not already present, returns whether
	 * the event was added.
	 * 
	 * @param event_
	 * @return true if set did not already contain
	 * 		   the event
	 */
	public boolean addEvent(Event event_) {
		return this.events.add(event_);
	}

	public boolean doesEventExist(Event event_) {
		return this.events.contains(event_);
	}
	
	
	// ==========================================
	// Getters:


	public String getName() {
		return name;
	}

	public HashSet<Event> getEvents() {
		return events;
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
		Sport other = (Sport) obj;
		return Objects.equals(name, other.name);
	}

}
