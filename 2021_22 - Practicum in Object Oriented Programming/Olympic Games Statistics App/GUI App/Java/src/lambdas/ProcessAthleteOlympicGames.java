package lambdas;

import olympics.competitors.Athlete;
import olympics.olympic_games.OlympicGames;

@FunctionalInterface
public interface ProcessAthleteOlympicGames {

	void call(Athlete athlete, OlympicGames oly_games);

}
