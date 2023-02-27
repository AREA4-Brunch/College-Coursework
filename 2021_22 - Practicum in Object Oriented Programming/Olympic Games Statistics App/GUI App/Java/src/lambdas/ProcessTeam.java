package lambdas;

import olympics.competitors.Team;


@FunctionalInterface
public interface ProcessTeam {

	void call(Team team);

}
