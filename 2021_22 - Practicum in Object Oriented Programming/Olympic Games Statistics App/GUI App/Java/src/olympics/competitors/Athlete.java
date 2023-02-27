package olympics.competitors;

import java.util.Objects;

import olympics.events.Event;


public class Athlete extends Competitor {
	
	// ==========================================
	// Attributes:


	private Integer id;
	private String name;
	private char gender = '-';  // M/F = male/female, dash for not defined
	private int age = -1;		 // -1 for not defined
	private int height = -1;    // -1 for not defined
	private float weight = -1.f;    // -1 for not defined


	// ==========================================
    // Constructors:


	public Athlete(final int id) {
		this.id = id;
	}

	public Athlete(final int id, Event event_,
				   final int oly_games_id)
	{
		this(id);
		this.addEvent(event_, oly_games_id);
	}

    
	// ==========================================
	// Getters:
	
	
	public Integer getId() {
		return id;
	}

	public String getName() {
		return name;
	}

	public char getGender() {
		return gender;
	}

	public int getAge() {
		return age;
	}

	public int getHeight() {
		return height;
	}

	public float getWeight() {
		return weight;
	}

	
	// ==========================================
	// Setters:
	
	
	public void setId(Integer id) {
		this.id = id;
	}

	public void setName(String name) {
		this.name = name;
	}

	public void setGender(char gender) {
		this.gender = gender;
	}

	public void setAge(int age) {
		this.age = age;
	}

	public void setHeight(int height) {
		this.height = height;
	}

	public void setWeight(float weight) {
		this.weight = weight;
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
		if (getClass() != obj.getClass())
			return false;
		Athlete other = (Athlete) obj;
		return id == other.id;
	}

}
