package exceptions;

public class LineInvalidFormat extends Exception {

	public LineInvalidFormat() {
		super("Line does not match regex");
	}

}
