package lambdas;

import olympics.competitors.Athlete;


@FunctionalInterface
public interface ProcessAthlete {

	void call(Athlete athlete);

}
