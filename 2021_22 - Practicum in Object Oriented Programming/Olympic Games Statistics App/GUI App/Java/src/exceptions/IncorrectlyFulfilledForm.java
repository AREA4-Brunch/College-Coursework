package exceptions;

public class IncorrectlyFulfilledForm extends Exception {

	public IncorrectlyFulfilledForm() {
		super("User failed to correctly fulfill a form.");
	}

}
