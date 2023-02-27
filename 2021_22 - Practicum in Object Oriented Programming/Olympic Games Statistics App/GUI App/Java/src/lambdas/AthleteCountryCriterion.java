package lambdas;

import olympics.competitors.Athlete;
import olympics.countries.Country;


@FunctionalInterface
public interface AthleteCountryCriterion {

	boolean call(Athlete athlete, Country country);

}
