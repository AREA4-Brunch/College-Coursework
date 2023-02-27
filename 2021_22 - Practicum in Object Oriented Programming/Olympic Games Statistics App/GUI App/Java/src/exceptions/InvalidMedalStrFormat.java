package exceptions;

public class InvalidMedalStrFormat extends Exception {

	public InvalidMedalStrFormat() {
		super("Unrecognized string defining a medal.");
	}

}
