package olympics.competitors;


import olympics.events.Event;
import awards.Medal;
import utils.Pair;

import java.util.ArrayList;


public abstract class Competitor {


	// ==========================================
	// Attributes:


    private ArrayList<Medal> medals = new ArrayList<>();

	// list of unique events competitor participated in
	private ArrayList<
		Pair <
			Event,  // discipline competitor competed in
			ArrayList<Integer>  // ids of olympic games
		>
	> events = new ArrayList<>();


	// ==========================================
    // Constructors:


    public Competitor() {

    }

    public Competitor(final int max_num_events) {
    	this.events.ensureCapacity(max_num_events);
    }


    // ==========================================
	// Getters:


	public ArrayList<Medal> getMedals() {
		return this.medals;
	}

	public ArrayList<Pair<Event, ArrayList<Integer>>> getEvents() {
		return this.events;
	}

	// adds medal to the competitor if it is not `no medal` value
	public void addMedal(Medal medal) {
		if (medal.getMedal() == Medal.TMedal.NoMedal) return;
		this.medals.add(medal);
	}


    // ==========================================
	// Managing Events:

	// adds an event if it does not already exist
	public void addEvent(Event event_, int oly_games_id) {
		// find given event in the vector
		int found_idx = 0;
		for (found_idx = 0; found_idx < this.events.size(); ++found_idx) {
			if (this.events.get(found_idx).first.equals(event_)) {
				break;  // found the event
			}
		}

		if (found_idx <= 0 || found_idx >= this.events.size()) {
			// event does not exist
			ArrayList<Integer> games_ids = new ArrayList<Integer>();
			games_ids.add(oly_games_id);
			this.events.add(new Pair<Event, ArrayList<Integer>>(event_, games_ids));
		} else {  // event does exist
			Pair<Event, ArrayList<Integer>> found = this.events.get(found_idx);
			found.second.add(oly_games_id);
		}
	}

	// reduces all vectors down to take up only as much
	// as the elements inside them
	public void shrinkToFit() {
		this.events.trimToSize();
		this.medals.trimToSize();
	}

}
