package main;


public class Main {

	public static void main(String[] args) {
		// include dlls
		System.loadLibrary("FilterCpp");
		
		new app.App();
	}

}
