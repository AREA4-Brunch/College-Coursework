package exceptions;

public class WrongFileFormat extends Exception {
	public WrongFileFormat() {
		super("File is of wrong extension.");
	}
}
