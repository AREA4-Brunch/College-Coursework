/**
 * Place subset of 64 given files into 2 memories of 32MB with least
 * space wasted using 20 independant runs of genetic algo, and compare
 * results to the simulated annealing.
 */

#include<iostream>
#include<array>
#include<vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <functional>
#include <limits>
#include <string>
#include <chrono>  // measure exec time
#include <cstdlib>  // call plotting script after hyperparam search
#include <numeric>  // std::iota


// ===============================================================
// Functions:


unsigned int getRandomSeed();
std::mt19937 initPSRNG(unsigned int seed);

template<typename Func, typename... Args>
auto executeReturnAndMeasureExecTime(const Func &func, Args&&... args)
    -> decltype(func(std::forward<Args>(args)...));

template<typename Func, typename... Args>
void executeAndMeasureExecTime(const Func &func, Args&&... args);

void storeHistoriesToFile(
    const std::vector<std::vector<int>> &costs_histories_to_append,
    const bool do_append,
    const std::string &path_costs_histories
);

std::vector<std::vector<int>> geneticAlgoSolve(
    const std::vector<int> &files,
    const int population_size,
    const int num_gens,
    int &min_cost,
    std::vector<int> &min_cost_unit,
    std::vector<std::vector<int>> &min_cost_population,
    const double units_to_select_ratio,
    const double crossover_threshold,
    const double mutation_threshold,
    const double elitism_threshold,
    const int k_crossover,
    const int k_mutation,
    const unsigned int seed=0
);

void Solve(
    const std::vector<int> &files,
    const int num_reruns,
    const double units_to_select_ratio,
    const double crossover_threshold,
    const double mutation_threshold,
    const double elitism_threshold,
    const int k_crossover,
    const int k_mutation,
    const std::string &path_costs_histories
);

void searchBestHyperparams(
    const std::vector<int> &files,
    const int num_reruns,
    const int desired_search_duration_secs,
    const std::string &path_plot_py
);


// ===============================================================
// Globals:


// static std::mt19937 PSRNG = initPSRNG(0);


// ===============================================================
// RUNNER:



int main()
{
    const std::vector<int> files = {
        173669, 275487, 1197613, 1549805, 502334, 217684, 1796841, 274708,
        631252, 148665, 150254, 4784408, 344759, 440109, 4198037, 329673, 28602,
        144173, 1461469, 187895, 369313, 959307, 1482335, 2772513, 1313997, 254845,
        486167, 2667146, 264004, 297223, 94694, 1757457, 576203, 8577828, 498382,
        8478177, 123575, 4062389, 3001419, 196884, 617991, 421056, 3017627, 131936,
        1152730, 2676649, 656678, 4519834, 201919, 56080, 2142553, 326263, 8172117,
        2304253, 4761871, 205387, 6148422, 414559, 2893305, 2158562, 465972, 304078,
        1841018, 1915571
    };

    // Search best hyper params and compare results to simulated annealing

    // executeAndMeasureExecTime([&] () {
    //     const int num_reruns = 20;
    //     const int desired_search_duration_secs = 150;
    //     const std::string path_plot_py = "plot.py";

    //     printf("Performing generative algorithm hyperparam search.\n");
    //     printf("Set to last ~ %d seconds.\nStarting...\n\n", desired_search_duration_secs);
    //     searchBestHyperparams(
    //         files,
    //         num_reruns,
    //         desired_search_duration_secs,
    //         path_plot_py
    //     );
    //     printf("\n\nFinished !\n");
    // });

    executeAndMeasureExecTime([&] () {
         printf("Performing generative algorithm..\n");
         const int num_reruns = 20;
         const std::string path_costs_histories = "costs_histories.bin";

         Solve(
            files,
            num_reruns,
            0.28, // units_to_select_ratio,
            0.80, // crossover_threshold,
            0.105, // mutation_threshold,
            0.5,  // elitism_threshold,
            2,  // k_crossover,
            2,  // k_mutation,
            path_costs_histories
         );
     });


    return 0;
}



// ===============================================================


namespace sale {


template<typename TIter, typename T>
T accumulate(TIter start, TIter end, T sum) {
    for ( ; start != end; ++start) sum += *start;
    return sum;
}


}  // sale


unsigned int getRandomSeed() {
    static std::random_device rd;
    unsigned int seed = rd();
    return seed;
}


/// @param seed - if 0 then function desides a seed
std::mt19937 initPSRNG(unsigned int seed) {
    if (seed == 0) seed = getRandomSeed();

    std::cout << "Seed: " << seed << std::endl;
    std::mt19937 mt(seed);
    return mt;
}


template<typename Func, typename... Args>
auto executeReturnAndMeasureExecTime(const Func &func, Args&&... args)
    -> decltype(func(std::forward<Args>(args)...))
{
    const auto start_time = std::chrono::steady_clock::now();
    auto res = func(std::forward<Args>(args)...);
    const auto end_time = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                          (end_time - start_time);
    printf("Execution time: %lld ms\n", duration.count());
    return res;
}


template<typename Func, typename... Args>
void executeAndMeasureExecTime(const Func &func, Args&&... args) {
    const auto start_time = std::chrono::steady_clock::now();
    func(std::forward<Args>(args)...);
    const auto end_time = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                          (end_time - start_time);
    printf("Execution time: %lld ms\n", duration.count());
}


void storeHistoriesToFile(
    const std::vector<std::vector<int>> &costs_histories_to_append,
    const bool do_append,
    const std::string &path_costs_histories
) {
    // store all histories into a file for Python plotting
    // const std::string path_costs_histories = "costs_histories.bin";
    printf("\nStoring histories into the file:\n%s\n", path_costs_histories.c_str());

    std::ofstream file(
        path_costs_histories,
        std::ios::binary | (do_append ? std::ios::app : std::ios::trunc)
    );

    if (!file.is_open()) throw "Failed to open file: " + path_costs_histories;

    size_t num_vectors = costs_histories_to_append.size();
    file.write(reinterpret_cast<const char*>(&num_vectors), sizeof(num_vectors));

    for (const auto &history : costs_histories_to_append) {
        size_t history_size = history.size();
        file.write(reinterpret_cast<const char *>(&history_size), sizeof(history_size));
        file.write(reinterpret_cast<const char *>(history.data()), history_size * sizeof(int));
    }

    file.close();
    printf("\nDone storing histories into the file:\n%s\n", path_costs_histories.c_str());
}


int cost(const std::vector<int> &x, const std::vector<int> &files) {
    constexpr const int c = 1 << 25;
    int costs[] = { 0, c, c };

    for (int i = 0, n = files.size(); i < n; ++i) {
        if (x[i]) costs[x[i]] -= files[i];
    }

    const int f1 = costs[1];
    const int f2 = costs[2];
    return f1 >= 0 && f2 >= 0 ? f1 + f2 : c << 1;
}


void Solve(
    const std::vector<int> &files,
    const int num_reruns,
    const double units_to_select_ratio,
    const double crossover_threshold,
    const double mutation_threshold,
    const double elitism_threshold,
    const int k_crossover,
    const int k_mutation,
    const std::string &path_costs_histories
) {
    constexpr const int population_size = 20000;
    const int num_gens = 50;

    int min_cost = std::numeric_limits<int>::max();
    std::vector<int> best_unit;
    // std::vector<std::vector<int>> best_population;

    unsigned int seed = getRandomSeed();
    for (int run = 1; run <= num_reruns; ++run) {
        int cur_min_cost = std::numeric_limits<int>::max();
        std::vector<int> cur_best_unit;
        std::vector<std::vector<int>> best_population;

        printf("\n\nRun #%d / %d\n", run, num_reruns);

        std::vector<std::vector<int>> costs = geneticAlgoSolve(
            files, population_size, num_gens, cur_min_cost, cur_best_unit, best_population,
            units_to_select_ratio,
            crossover_threshold,
            mutation_threshold,
            elitism_threshold,
            k_crossover,
            k_mutation,
            seed++
        );

        if (cur_min_cost < min_cost) {
            min_cost = cur_min_cost;
            std::swap(best_unit, cur_best_unit);
        }

        printf("Overall min cost: %d\n", min_cost);
 
        storeHistoriesToFile(costs, run != 1, path_costs_histories);
    }

    printf("\n\nMinimum cost out of all runs: %d\n", min_cost);
    printf("described by the best solution x:\n");
    for (const auto xi : best_unit) {
        std::cout << xi << ", ";
    }
    printf("\n\n");
}


void searchBestHyperparams(
    const std::vector<int> &files,
    const int num_reruns,
    const int desired_search_duration_secs,
    const std::string &path_plot_py
) {
    std::random_device rd;
    const unsigned int seed = rd();
    std::cout << "Hyperparam search seed: " << seed << std::endl;
    std::mt19937 psrng(seed);

    std::uniform_real_distribution<double>
        // random_selection_ratio(0.70, 0.70);  // elit. 1 cross, 1 mut
        // random_selection_ratio(0.67, 0.72);  // elit. 2 cross, 2 mut
        random_selection_ratio(0.28, 0.28);  // elit. 2 cross, 2 mut

    std::uniform_real_distribution<double>
        random_crossover_threshold(0.80, 0.80);  // now
        // random_crossover_threshold(0.78, 0.82);  // in general

    std::uniform_real_distribution<double>
        random_mutation_threshold(0.105, 0.105);  // 10-11
        // random_mutation_threshold(0.10, 0.11);  // elit. 2 cross, 2 mut
        // random_mutation_threshold(0.078, 0.082);  // in general

    std::uniform_real_distribution<double>
        random_elitism_threshold(0.50, 0.50);  // selection ~0.7
        // random_elitism_threshold(0.31, 0.33);  // selection ~0.7

    std::uniform_int_distribution<int>
        random_k_crossover(1, 2);

    std::uniform_int_distribution<int>
        random_k_mutation(1, 2);

    const auto start_time = std::chrono::steady_clock::now();
    int search = 0;
    for (int k_crossover = 2; k_crossover <= 2; ++k_crossover) {
        for (int k_mutation = 2; k_mutation <= 2; ++k_mutation) {
            for (search++; ; ++search) {
                printf("\n\nSearch: %d\n", search);

                // embed search idx into file path
                std::string path_costs_histories = "costs_histories";
                path_costs_histories += "_" + std::to_string(search) + ".bin";

                const double units_to_select_ratio = random_selection_ratio(psrng);
                const double crossover_threshold = random_crossover_threshold(psrng);
                const double mutation_threshold = random_mutation_threshold(psrng);
                const double elitism_threshold = random_elitism_threshold(psrng);
                // const int k_crossover = random_k_crossover(psrng);
                // const int k_mutation = random_k_mutation(psrng);

                printf("\n\nTrying hyper params:\n");
                std::cout << "units_to_select_ratio: " << units_to_select_ratio << std::endl;
                std::cout << "crossover_threshold: " << crossover_threshold << std::endl;
                std::cout << "mutation_threshold: " << mutation_threshold << std::endl;
                std::cout << "elitism_threshold: " << elitism_threshold << std::endl;
                std::cout << "k_crossover: " << k_crossover << std::endl;
                std::cout << "k_mutation: " << k_mutation << std::endl;

                executeAndMeasureExecTime(
                    Solve,
                    files,
                    num_reruns,
                    units_to_select_ratio,
                    crossover_threshold,
                    mutation_threshold,
                    elitism_threshold,
                    k_crossover,
                    k_mutation,
                    path_costs_histories
                );

                const auto end_time = std::chrono::steady_clock::now();
                const auto duration = std::chrono::duration_cast<std::chrono::seconds>
                                    (end_time - start_time);
                if (duration.count() > desired_search_duration_secs) {
                    break;
                }
            }
        }
    }

    executeAndMeasureExecTime([&] () {
        std::string plot_cmnd = "python ";
        plot_cmnd += path_plot_py + " --compare_hyperparam_searches"
                  + " " + std::to_string(1)  // start search idx
                  + " " + std::to_string(search);  // end search idx

        int res = std::system(plot_cmnd.c_str());
        if (res == 0) {
            printf("Done plotting as well.\n");
        } else {
            printf("Python failed you my lord :(\n");
        }
    });
}


std::vector<std::vector<int>> geneticAlgoSolve(
    const std::vector<int> &files,
    const int population_size,
    const int num_gens,
    int &min_cost,
    std::vector<int> &min_cost_unit,
    std::vector<std::vector<int>> &min_cost_units_population,
    const double units_to_select_ratio,
    const double crossover_threshold,
    const double mutation_threshold,
    const double elitism_threshold,
    const int k_crossover,
    const int k_mutation,
    const unsigned int seed
) {
    constexpr const int num_file_groups = 3;

    // hyperparams:
    const int units_to_select = (int) (units_to_select_ratio * population_size);
    const int selected_units_to_inherit = (int) (units_to_select * elitism_threshold);
    // const int units_to_select = (int) (0.5 * population_size);
    // const double crossover_threshold = 0.85;
    // const double mutation_threshold = 7.;


    // std::mt19937 &psrng = ::PSRNG;
    std::mt19937 psrng = initPSRNG(seed);
    std::uniform_int_distribution<int> random_file_group(0, num_file_groups - 1);
    std::uniform_real_distribution<> random_prob(0, 1);
    std::uniform_int_distribution<int> random_unit_el_idx(0, files.size() - 1);

    // create initial population and allocate space for next one
    std::vector<std::vector<std::vector<int>>> population_cur_next(
        2,
        std::vector<std::vector<int>> (
            population_size, std::vector<int> (files.size())
        )
    );

    // reuse from previous run the starting point that had best unit
    // if (min_cost != std::numeric_limits<int>::max()) {
    //     population_cur_next[0] = min_cost_units_population;
    // } else {
    //     for (auto &unit : population_cur_next[0]) {
    //         for (auto &file_group : unit) {
    //             file_group = random_file_group(psrng);
    //         }
    //     }
    // }
    for (auto &unit : population_cur_next[0]) {
        for (auto &file_group : unit) {
            file_group = random_file_group(psrng);
        }
    }

    // elitism selection, also implemnted later when looping in generations
    std::vector<int> cur_selection_indices(population_size);
    std::iota(cur_selection_indices.begin(), cur_selection_indices.end(), 0);

    auto elitism_selection = [&] (std::vector<int> &costs) {
        std::sort(
            cur_selection_indices.begin(), cur_selection_indices.end(),
            [&] (const int idx1, const int idx2) {
                return costs[idx1] < costs[idx2];
            }
        );

        return units_to_select;
    };

    // decimation selection
    // std::vector<int> cur_selection_indices(population_size);
    // std::iota(cur_selection_indices.begin(), cur_selection_indices.end(), 0);

    auto decimation_selection = [&] (std::vector<int> &costs) {
        std::sort(
            cur_selection_indices.begin(), cur_selection_indices.end(),
            [&] (const int idx1, const int idx2) {
                return costs[idx1] > costs[idx2];
            }
        );

        return units_to_select;
    };

    /// roulet selection
    std::vector<double> fitness_ratios(population_size);
    // std::vector<int> cur_selection_indices(units_to_select);
    auto roulet_selection = [&] (std::vector<int> &costs, const long long total_cost) {
        // const int units_to_select = (int) (0.7 * population_size);
        // const int total_cost = std::accumulate(costs.begin(), costs.end(), 0);  // C++20
        // const int total_cost = sale::accumulate(costs.begin(), costs.end(), 0);

        // compute prob for each unit in cur population to be in next one
        fitness_ratios[0] = 1. * costs[0] / total_cost;
        for (int i = 1; i < population_size; ++i) {
            fitness_ratios[i] = fitness_ratios[i - 1] + costs[i] / total_cost;
        }

        // add units from cur into next population
        for (int i = 0; i < units_to_select; ++i) {
            const double selected_unit_prob = random_prob(psrng);
            auto it = std::upper_bound(fitness_ratios.begin(),
                                       fitness_ratios.end(),
                                       selected_unit_prob);
            if (it == fitness_ratios.end()) it--;
            const int selected_unit_idx = it - fitness_ratios.begin();
            cur_selection_indices[i] = selected_unit_idx;
        }

        return units_to_select;
    };

    // tournament selection
    // std::vector<int> cur_selection_indices(units_to_select);
    auto tournament_selection = [&] (std::vector<int> &costs) {
        std::vector<bool> is_selected(population_size, false);  // can save space using next population instead
        std::uniform_int_distribution<int> random_unit(0, population_size - 1);

        for (auto &idx : cur_selection_indices) {
            // choose 2 random indices that have not yet been selected
            int unit1_idx = random_unit(psrng);
            for ( ; is_selected[unit1_idx]; unit1_idx = random_unit(psrng));
            int unit2_idx = random_unit(psrng);
            for ( ;
                is_selected[unit2_idx] && unit1_idx == unit2_idx;
                unit1_idx = random_unit(psrng)
            );
            idx = costs[unit1_idx] < costs[unit2_idx] ?
                  unit1_idx : unit2_idx;
        }
        return units_to_select;
    };

    /// k-point crossover
    auto k_point_crossover = [&] (
        std::vector<std::vector<int>> &cur_population,
        std::vector<std::vector<int>> &next_population,
        const int units_selected,
        const int num_crossovers
    ) {
        // const double crossover_threshold = 0.6;
        std::uniform_int_distribution<int> random_unit(0, units_selected - 1);

        for (int unit_idx = 0; unit_idx < population_size; ++unit_idx) {
            auto &unit = next_population[unit_idx];

            for (int k = 0, prev_idx = -1; k < num_crossovers; ++k) {
                // random unit, try to crossover to other one to form `unit`
                const int x_idx = cur_selection_indices[random_unit(psrng)];

                // check if crossover should happen, 80% prob it should
                if (random_prob(psrng) > crossover_threshold) {
                    // no crossover, cur unit goes into next population
                    // if (k == num_crossovers - 1) unit = cur_population[x_idx];
                    unit = cur_population[x_idx];
                    // continue;
                    break;
                }

                const int y_idx = cur_selection_indices[random_unit(psrng)];
                int crossover_idx = random_unit_el_idx(psrng);
                int max_tries = 5;
                for ( ; 
                     max_tries > 0 && crossover_idx <= prev_idx;
                     crossover_idx = random_unit_el_idx(psrng),
                     --max_tries
                );
                if (max_tries <= 0) continue;

                // perform the crossover with x parts
                for (int i = 0; i < crossover_idx; ++i) {
                    unit[i] = next_population[x_idx][i];
                }

                // perform the crossover with y parts
                for (int i = crossover_idx, n = unit.size(); i < n; ++i) {
                    unit[i] = next_population[y_idx][i];
                }

                prev_idx = crossover_idx;
            }
        }
    };

    auto mutation = [&] (
        std::vector<std::vector<int>> &population,
        const int num_mutations
    ) {
        // const double mutation_threshold = 7.;

        for (auto &unit : population) {
            if (random_prob(psrng) >= mutation_threshold) continue;

            for (int i = 0; i < num_mutations; ++i) {
                const int mutation_idx = random_unit_el_idx(psrng);
                const int increment = random_prob(psrng) > 0.5 ? 1 : 2;
                unit[mutation_idx] = (unit[mutation_idx] + increment)
                                   % num_file_groups;
            }
        }
    };

    auto min_population_cost = std::numeric_limits<long long>::max();
    auto calc_costs = [&] (
        std::vector<std::vector<int>> &population,
        std::vector<int> &costs
    ) -> long long {
        long long total_cost = 0LL;
        bool found_new_min = false;

        for (int i = 0; i < population_size; ++i) {
            total_cost += costs[i] = cost(population[i], files);

            if (costs[i] < min_cost) {  // update overall best
                found_new_min = true;
                min_cost = costs[i];
                min_cost_unit = population[i];
                // std::cout << "Min cost: " << min_cost << std::endl;
            }
        }

        // if (found_new_min) {
        // if (total_cost < min_population_cost) {
        // if (total_cost < min_population_cost || found_new_min) {
        //     min_population_cost = total_cost;
        //     // std::cout << "min_population_cost: " << min_population_cost << std::endl;
        //     min_cost_units_population = population;
        // }

        return total_cost;
    };

    std::vector<std::vector<int>> costs(
        num_gens,
        std::vector<int> (population_size)
    );
    int cur_population_idx = 0, next_population_idx = 1;
    for (int generation = 0; generation < num_gens; ++generation) {
        // std::cout << "Generation: " << generation << std::endl;

        const auto total_cost = calc_costs(
            population_cur_next[cur_population_idx],
            costs[generation]
        );

        const int units_selected = elitism_selection(costs[generation]);

        k_point_crossover(
            population_cur_next[cur_population_idx],
            population_cur_next[next_population_idx],
            units_selected,
            k_crossover
        );

        mutation(population_cur_next[next_population_idx], k_mutation);

        // leave top portion of the selected units in the next gen
        for (int i = 0; i < selected_units_to_inherit; ++i) {
            std::swap(
                population_cur_next[next_population_idx][i],
                population_cur_next[cur_population_idx][cur_selection_indices[i]]
            );
        }

        // space for next to write into becomes cur, cur becomes next
        std::swap(cur_population_idx, next_population_idx);
    }

    return costs;
}
