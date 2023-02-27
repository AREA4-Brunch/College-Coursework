package lambdas;

import olympics.competitors.Team;
import olympics.countries.Country;


@FunctionalInterface
public interface TeamCountryCriterion {

	boolean call(Team team, Country country);

}
