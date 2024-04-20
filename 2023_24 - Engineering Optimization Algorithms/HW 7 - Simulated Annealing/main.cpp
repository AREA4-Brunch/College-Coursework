/**
 * Place subset of 64 given files into 2 memories of 32MB with least
 * space wasted using 20 independant simulated annealing runs, each
 * with 10 iters of reannealing.
 */


#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <cmath>
#include <exception>
#include <fstream>
#include <chrono>  // measure exec time
#include <limits>



// ===============================================================
// Functions:



std::mt19937 initPSRNG(const unsigned int seed);

template<typename Func, typename... Args>
auto executeReturnAndMeasureExecTime(const Func &func, Args&&... args)
     -> decltype(func(std::forward<Args>(args)...));

template<typename Func, typename... Args>
void executeAndMeasureExecTime(const Func &func, Args&&... args);

std::vector<std::vector<int>> Solve(const std::vector<int> &files);

void storeHistoriesToFile(const std::vector<std::vector<int>> &costs_histories_all);



// ===============================================================
// Globals:



static std::mt19937 PSRNG = initPSRNG(static_cast<unsigned int> (time(NULL)));



// ===============================================================
// Classes:



template<typename Tx, typename TCost>
class SimulatedAnnealing {

 public:

    class CustomException: std::exception {
     private:
        const std::string msg;
     public:
        CustomException(const std::string &msg) : msg(msg) { }
        const char* what() const noexcept override { return this->msg.c_str(); }
    };

    struct Result {
        std::vector<Tx> res;
        std::vector<TCost> costs_history;
        double temperature = 0.;
        TCost min_cost;
    };


 private:

    struct Cost {

        const TCost c = ((TCost) 1) << 25;
        TCost f1 = c;
        TCost f2 = c;

        Cost() = default;
        Cost& operator= (Cost cost) { this->f1 = cost.f1; this->f2 = cost.f2; return *this; }

        TCost operator() (const std::vector<int> &files,
                          const std::vector<Tx> &x)
        {
            f1 = c, f2 = c;

            for (int i = 0, n = x.size(); i < n; ++i) {
                if (x[i] == 1) f1 -= files[i];
                else if (x[i] == 2) f2 -= files[i];
            }

            return cost();
        }

        TCost cost() const {
            // f1 + f2 iff f1, f2 >= 0, else 2^26
            return f1 >= 0 && f2 >= 0 ? f1 + f2 : c << 1;
        }

    };


public:

    SimulatedAnnealing(const std::mt19937 &psrng) : psrng(psrng) { }

    typename SimulatedAnnealing<Tx, TCost>::Result operator() (
        const std::vector<int> &files,
        const std::vector<Tx> x,
        const int num_iters,
        const double cur_temperature,
        const bool ret_min=true  // return min or last solution
    ) const {
        return this->anneal(files, x, num_iters, cur_temperature, ret_min);
    }

    typename SimulatedAnnealing<Tx, TCost>::Result anneal(
        const std::vector<int> &files,
        std::vector<Tx> x,
        const int num_iters,
        double cur_temperature,
        const bool ret_min=true  // return min or last solution
    ) const;


private:

    mutable std::mt19937 psrng;

};



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

    executeAndMeasureExecTime([&] () {
        printf("Performing simulated annealing..\n");
        const auto costs_histories_all = executeReturnAndMeasureExecTime(Solve, files);
        executeAndMeasureExecTime(storeHistoriesToFile, costs_histories_all);
    });


    return 0;
}


std::mt19937 initPSRNG(const unsigned int seed) {
    srand(seed);
    std::random_device rd;
    std::mt19937 mt(rd());
    return mt;
}


template<typename Func, typename... Args>
auto executeReturnAndMeasureExecTime(const Func &func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
    const auto start_time = std::chrono::steady_clock::now();
    auto res = func(std::forward<Args>(args)...);
    const auto end_time = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end_time - start_time);
    printf("Execution time: %lld ms\n", duration.count());
    return res;
}


template<typename Func, typename... Args>
void executeAndMeasureExecTime(const Func &func, Args&&... args) {
    const auto start_time = std::chrono::steady_clock::now();
    func(std::forward<Args>(args)...);
    const auto end_time = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end_time - start_time);
    printf("Execution time: %lld ms\n", duration.count());
}


template<typename Tx, typename TCost>
typename SimulatedAnnealing<Tx, TCost>::Result SimulatedAnnealing<Tx, TCost>::anneal(
    const std::vector<int> &files,
    std::vector<Tx> x,
    const int num_iters,
    double cur_temperature,
    const bool ret_min
) const
{
    if (cur_temperature == 0) return { {}, {}, cur_temperature };
    if (cur_temperature < 0) {
        throw CustomException("cur_temperature must be >= 0");
    }

    static std::uniform_real_distribution<double>
        random_prob(0, 1);

    static std::uniform_int_distribution<>
        random_file_idx(0, x.size() - 1);

    Cost cur_cost {};
    TCost min_cost = cur_cost(files, x);
    Result result = {
        x, { min_cost }, cur_temperature, min_cost
    };

    const int hmin = 1, hmax = 15;
    for (int iter = 1; iter <= num_iters; ++iter, cur_temperature *= 0.95) {
        // generate next point for consideration and calc its cost
        const Cost prev_cost = cur_cost;
        const int max_changes = (hmin - hmax) / (num_iters - 1)
                              * (iter - 1)
                              + hmax;
        std::uniform_int_distribution<>
            random_num_changes(0, max_changes);

        int allowed_changes = random_num_changes(this->psrng);
        std::vector<std::pair<int, int>> restore_x(allowed_changes);
        for ( ; allowed_changes > 0; )
        {
            const int idx = random_file_idx(this->psrng);
            const int update_by = std::min(
                allowed_changes,
                random_prob(this->psrng) > 0.5 ? 1 : 2
            );

            if (x[idx] == 1) cur_cost.f1 += files[idx];
            else if (x[idx] == 2) cur_cost.f2 += files[idx];
            restore_x[allowed_changes - 1] = { idx, x[idx] };

            x[idx] = (x[idx] + update_by) % 3;

            if (x[idx] == 1) cur_cost.f1 -= files[idx];
            else if (x[idx] == 2) cur_cost.f2 -= files[idx];

            allowed_changes -= update_by;
        }

        // delta_energy = cur_cost - next_point_cost
        const TCost cur_cost_val = cur_cost.cost();
        const TCost delta_energy = prev_cost.cost() - cur_cost_val;

        // next_point_cost < cur_cost immediately accept next_point
        // else probabilistically accept next_point
        if (delta_energy > 0) {  // x = next_point;
            // update cur min if user wants min cost sol
            if (cur_cost_val < min_cost) {
                min_cost = cur_cost_val;
                if (ret_min) result.res = x;
            }
        } else {
            // cur_t != 0 (cause of factor 0.95)
            const double prob = std::exp(delta_energy / cur_temperature);

            if (random_prob(this->psrng) > prob) {
                // next_point is rejected, restore x and cur_cost
                cur_cost = prev_cost;
                for (const auto &idx_val : restore_x) {
                    x[idx_val.first] = idx_val.second;
                }
            }
        }

        result.costs_history.push_back(cur_cost_val);
    }

    // if not x min then store last x
    if (!ret_min) result.res = std::move(x);
    result.temperature = cur_temperature;
    result.min_cost = min_cost;
    return result;
}


std::vector<std::vector<int>> Solve(const std::vector<int> &files) {
    const double init_temperature = 64. * 1024 * 1024;  // T0 = 64MB
    const int num_iters = 100000;

    auto gen_random_sol = [&] () {
        std::uniform_int_distribution<> random_int(0, 2);

        std::vector<int> x(files.size());
        for (auto &el : x) el = random_int(PSRNG);
        return x;
    };


    SimulatedAnnealing<int, int> anneal (PSRNG);
    std::vector<std::vector<int>> costs_histories_all;
    int best_cost = std::numeric_limits<int>::max();
    std::vector<int> best_res;

    for (int independent_run = 1; independent_run <= 20; ++independent_run) {
        printf("\nIndependent run #%d\n", independent_run);

        SimulatedAnnealing<int, int>::Result cur_best = {
            gen_random_sol(), {}, init_temperature
        };

        for (int anneal_idx = 1; anneal_idx <= 10; ++anneal_idx) {
            cur_best = anneal(
                files,
                cur_best.res,
                num_iters,
                cur_best.temperature,
                true
            );

            printf("\t#%d %d) Cur min cost found: %d\n", independent_run, anneal_idx, cur_best.min_cost);
            costs_histories_all.emplace_back(std::move(cur_best.costs_history));

            if (cur_best.min_cost < best_cost) {
                best_cost = cur_best.min_cost;
                best_res = cur_best.res;
            }
        }
    }

    printf("\n\nMinimum cost out of all iterations: %d\n", best_cost);
    printf("described by the best solution x:\n");
    for (const auto xi : best_res) {
        std::cout << xi << ", ";
    }
    printf("\n\n");

    return costs_histories_all;
}


// void storeHistoriesToFile(
//     const std::vector<std::vector<int>> &costs_histories_all
// ) {
//     // store all histories into a file for Python plotting
//     printf("\nStoring histories into the file..\n");

//     const std::string path_costs_histories = "costs_histories.txt";
//     std::ofstream file(path_costs_histories);
//     if (!file.is_open()) throw "Failed to open file: " + path_costs_histories;

//     for (const std::vector<int> &history : costs_histories_all) {
//         for (int i = 0, n = history.size(); i < n - 1; ++i) {
//             file << history[i] << " ";
//         }
//         file << history.back() << '\n';
//     }

//     file.close();

//     printf("Done storing histories into the file.\n");
// }


void storeHistoriesToFile(
    const std::vector<std::vector<int>> &costs_histories_all
) {
    // store all histories into a file for Python plotting
    printf("\nStoring histories into the file..\n");

    const std::string path_costs_histories = "costs_histories.bin";
    std::ofstream file(path_costs_histories, std::ios::binary);

    if (!file.is_open()) throw "Failed to open file: " + path_costs_histories;

    size_t num_vectors = costs_histories_all.size();
    file.write(reinterpret_cast<const char*>(&num_vectors), sizeof(num_vectors));

    for (const auto &history : costs_histories_all) {
        size_t history_size = history.size();
        file.write(reinterpret_cast<const char *>(&history_size), sizeof(history_size));
        file.write(reinterpret_cast<const char *>(history.data()), history_size * sizeof(int));
    }

    file.close();
    printf("Done storing histories into the file.\n");
}
