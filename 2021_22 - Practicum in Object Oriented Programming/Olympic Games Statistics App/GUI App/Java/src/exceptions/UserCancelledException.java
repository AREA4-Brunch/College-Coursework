package exceptions;

public class UserCancelledException extends Exception {

	public UserCancelledException() {
		super("User cancelled in middle of necessary action.");
	}

}
