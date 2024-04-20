/**
 * Steiner tree problem, find 2 points to connect given 6 points
 * in 3D space with minimal cost/length of the path that connects them.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <functional>
#include <chrono>



template<typename Func, typename... Args>
void executeAndMeasureExecTime(const Func &func, Args&&... args);


namespace sale {


template<typename T>
struct Point3D;

template<typename T>
std::ostream& operator<< (std::ostream &os, const sale::Point3D<T> &self);

template<typename T>
Point3D<T> operator* (const Point3D<T> &P, const double scalar);

template<typename T>
Point3D<T> operator* (const double scalar, const Point3D<T> &P);


template<typename T>
struct Point3D {
    T x, y, z;

    Point3D operator- (const Point3D &other) const {
        return Point3D {
            x - other.x,
            y - other.y,
            z - other.z
        };
    }

    Point3D operator+ (const Point3D &other) const {
        return Point3D {
            x + other.x,
            y + other.y,
            z + other.z
        };
    }

    friend Point3D operator* (const Point3D &P, const double scalar) {
        return Point3D {
            scalar * P.x,
            scalar * P.y,
            scalar * P.z
        };
    }

    friend Point3D operator* (const double scalar, const Point3D &P) {
        return Point3D {
            scalar * P.x,
            scalar * P.y,
            scalar * P.z
        };
    }

    T& operator[] (const int axis) {
        switch (axis)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:
                throw std::exception("Point3D axis out of bounds.");
        }
    }

    const T& operator[] (const int axis) const {
        switch (axis)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:
                throw std::exception("Point3D axis out of bounds.");
        }
    }

    friend std::ostream& operator<< (std::ostream &os, const sale::Point3D<T> &self) {
        return os << "(" << self.x << ", " << self.y << ", " << self.z << ")";
    }

};


}  // sale


namespace std {


template<typename T>
T abs(const sale::Point3D<T> &P) {
    return std::sqrt(
        P.x * P.x
      + P.y * P.y
      + P.z * P.z
    );
}


}  // std



template<typename T>
struct X;

template<typename T>
X<T> operator* (const double scalar, const X<T> &x);

template<typename T>
X<T> operator* (const X<T> &x, const double scalar);

template<typename T>
X<T> operator+ (
    const std::vector<double> &vect,
    const X<T> &x
);

template<typename T>
X<T> operator+ (
    const X<T> &x,
    const std::vector<double> &vect
);

template<typename T>
std::vector<double>& operator+= (std::vector<double> &dst, const X<T> &x);

template<typename T>
std::ostream& operator<< (std::ostream &os, const X<T> &self);

template<typename T>
struct CostFunc;

template<typename T>
class MinMaxScaler;

template<typename T>
void Solve(const std::vector<sale::Point3D<T>> &points, const double precision);

template<typename TCost, typename TX>
std::pair<TCost, TX> ParticleSwarmOptimization(
    std::vector<TX> cur_population,
    std::function<TCost (const TX &x)> cost_func,
    const double precision,
    std::mt19937 &psrng,
    const double inertia_coef,
    const double cognitive_coef,
    const double social_coef,
    const double max_velocity,
    const int max_convergence_iters,
    const int num_dimensions
);

unsigned int generateRandomSeed();



int main()
{
    using T = double;

    const std::vector<sale::Point3D<T>> points = {
        { 1, 4, 0 },
        { 3, 2, 0 },
        { 2, 7, 1 },
        { 6, 3, 3 },
        { 7, 6, 5 },
        { 5, 7, 4 }
    };

    executeAndMeasureExecTime(
        Solve<T>,
        points,
        1e-3  // precision
    );

    return 0;
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


unsigned int generateRandomSeed() {
    static std::random_device rd;
    const unsigned int seed = rd();
    return seed;
}


template<typename T>
struct X {
    sale::Point3D<T> S1;
    sale::Point3D<T> S2;

    X operator- (const X& other) const {
        return { S1 - other.S1, S2 - other.S2 };
    }

    X operator+ (const X& other) const {
        return { S1 + other.S1, S2 + other.S2 };
    }

    friend std::vector<double>& operator+= (std::vector<double> &dst, const X &x) {
        dst[0] += x.S1.x;
        dst[1] += x.S1.y;
        dst[2] += x.S1.z;
        dst[3] += x.S2.x;
        dst[4] += x.S2.y;
        dst[5] += x.S2.z;
        return dst;
    }

    friend X operator* (const double scalar, const X &x) {
        return { scalar * x.S1, scalar * x.S2 };
    }

    friend X operator* (const X &x, const double scalar) {
        return { scalar * x.S1, scalar * x.S2 };
    }

    friend X operator+ (
        const std::vector<double> &vect,
        const X &x
    ) {
        return {
            vect[0] + x.S1.x,
            vect[1] + x.S1.y,
            vect[2] + x.S1.z,
            vect[3] + x.S2.x,
            vect[4] + x.S2.y,
            vect[5] + x.S2.z
        };
    }

    friend X operator+ (
        const X &x,
        const std::vector<double> &vect
    ) {
        return {
            vect[0] + x.S1.x,
            vect[1] + x.S1.y,
            vect[2] + x.S1.z,
            vect[3] + x.S2.x,
            vect[4] + x.S2.y,
            vect[5] + x.S2.z
        };
    }

    friend std::ostream& operator<< (std::ostream &os, const X &self) {
        return os << "S1: " << self.S1 << ", "
                  << "S2: " << self.S2;
    }

};


template<typename T>
struct CostFunc
{
    const std::vector<sale::Point3D<T>> &points;

    CostFunc(const std::vector<sale::Point3D<T>> &points)
        : points(points) {}

    T operator() (const X<T> &x) const {
        const auto &S1 = x.S1;
        const auto &S2 = x.S2;

        T cost = (T) 0;

        // connect the dots to S1
        for (int i = 0; i < 3; ++i) {
            cost += std::abs(S1 - this->points[i]);
        }

        // connect the dots to S2
        for (int i = 3, n = points.size(); i < n; ++i) {
            cost += std::abs(S2 - this->points[i]);
        }

        cost += std::abs(S1 - S2);

        return cost;
    }

};


template<typename TCost, typename TX>
std::pair<TCost, TX> ParticleSwarmOptimization(
    std::vector<TX> cur_population,
    std::function<TCost (const TX &x)> cost_func,
    const double precision,
    std::mt19937 &psrng,
    const double inertia_coef,
    const double cognitive_coef,
    const double social_coef,
    const double max_velocity,
    const int max_convergence_iters,
    const int num_dimensions
) {
    constexpr const TCost inf = std::numeric_limits<TCost>::max();
    const int population_size = cur_population.size();

    std::uniform_real_distribution<float> coin(0., 1.);
    std::vector<TX> next_population = cur_population;
    std::vector<TX> personal_best_units(population_size);
    std::vector<TCost> personal_best_costs(population_size, inf);

    // find global best unit and its cost
    TCost global_best_cost = cost_func(cur_population[0]);
    TX global_best_unit = cur_population[0];

    auto update_best = [&] (
        const std::vector<TX> &population,
        const std::function<void (int idx)> process_unit
    ) {
        int best_unit_idx = -1;

        for (int i = 0, n = population.size(); i < n; ++i) {
            process_unit(i);

            const TCost cur_cost = cost_func(population[i]);

            // update personal best
            if (cur_cost < personal_best_costs[i]) {
                personal_best_costs[i] = cur_cost;
                personal_best_units[i] = population[i];
            }

            // update global best
            if (cur_cost < global_best_cost) {
                global_best_cost = cur_cost;
                best_unit_idx = i;
            }
        }

        if (best_unit_idx >= 0) {  // if new global best was found, update
            global_best_unit = population[best_unit_idx];
        }
    };

    // find the best in the initial random population
    update_best(cur_population, [] (int) {});

    std::vector<std::vector<double>> prev_velocities(
        population_size, std::vector<double> (num_dimensions, 0.)
    );

    auto scalar_vector_mul = [] (
        const double scalar, std::vector<double> &vect
    ) -> std::vector<double> &
    {
        for (auto &el : vect) el *= scalar;
        return vect;
    };

    auto update_velocity = [&] (const int unit_idx) {
        scalar_vector_mul(inertia_coef, prev_velocities[unit_idx]);
        prev_velocities[unit_idx] += 
            cognitive_coef * coin(psrng) * (personal_best_units[unit_idx] - cur_population[unit_idx])
          + social_coef    * coin(psrng) * (global_best_unit - cur_population[unit_idx]);

        for (auto &velocity : prev_velocities[unit_idx]) {
            velocity = std::min(velocity, max_velocity);
        }
    };

    TCost prev_best_cost = inf;
    for (int iter = 0; iter < max_convergence_iters; prev_best_cost=global_best_cost) {
        update_best(next_population, [&] (int unit_idx) {
            update_velocity(unit_idx);
            next_population[unit_idx] = cur_population[unit_idx]
                                      + prev_velocities[unit_idx];
        });

        // exit when after max_convergence_iters the best cost has not improved
        // more than precision amount
        if (prev_best_cost - global_best_cost >= precision) {
            iter = 0;
        } else {
            iter++;
        }

        std::swap(cur_population, next_population);
    }

    return { global_best_cost, global_best_unit };
}


/// @brief Axis-wise MinMaxScaler
/// @tparam T - min/max data type
template<typename T>
class MinMaxScaler {

 public:

    MinMaxScaler() = default;

    template<typename TDataFrame>
    void init(const TDataFrame &data, const int num_axis) {
        this->min_axis_vals = std::vector<T>(num_axis, std::numeric_limits<T>::max());
        this->max_axis_vals = std::vector<T>(num_axis, std::numeric_limits<T>::min());

        for (const auto &point : data) {
            for (int axis = 0; axis < num_axis; ++axis) {
                min_axis_vals[axis] = std::min(min_axis_vals[axis], point[axis]);
                max_axis_vals[axis] = std::max(max_axis_vals[axis], point[axis]);
            }
        }
    }

    template<typename TDataFrame>
    void normalize(TDataFrame &data) const {
        if (!this->min_axis_vals.size() || !this->max_axis_vals.size()) {
            throw std::exception("MinMaxScaler has not been initialized.");
        }

        for (auto &point : data) {
            for (int axis = 0, m = min_axis_vals.size(); axis < m; ++axis) {
                point[axis] = (point[axis] - min_axis_vals[axis])
                            / (max_axis_vals[axis] - min_axis_vals[axis]);
            }
        }
    }

    template<typename TDataFrame>
    void denormalize(TDataFrame &data) const {
        if (!this->min_axis_vals.size() || !this->max_axis_vals.size()) {
            throw std::exception("MinMaxScaler has not been initialized.");
        }

        for (auto &point : data) {
            for (int axis = 0, m = min_axis_vals.size(); axis < m; ++axis) {
                point[axis] = point[axis] * (max_axis_vals[axis] - min_axis_vals[axis])
                            + min_axis_vals[axis];
            }
        }
    }


 private:

    std::vector<T> min_axis_vals;
    std::vector<T> max_axis_vals;

};


template<typename T>
void Solve(const std::vector<sale::Point3D<T>> &points, const double precision) {
    const constexpr int population_size = 100;

    const auto seed = generateRandomSeed();
    std::cout << "Using seed: " << seed << std::endl;
    std::mt19937 psrng(seed);

    // Create randmom initial population
    // std::uniform_real_distribution<T> random_x_coo(1., 7.);
    // std::uniform_real_distribution<T> random_y_coo(2., 7.);
    // std::uniform_real_distribution<T> random_z_coo(0., 5.);
    std::uniform_real_distribution<T> random_normalized_coo(0., 1.);

    auto generate_random_point = [&] () {
        return sale::Point3D<T> {
            random_normalized_coo(psrng),
            random_normalized_coo(psrng),
            random_normalized_coo(psrng)
        };
    };

    std::vector<X<T>> random_population(population_size);
    for (int i = 0; i < population_size; ++i) {
        random_population[i] = {
            generate_random_point(),
            generate_random_point()
        };
    }

    std::vector<sale::Point3D<T>> normalized_points = points;
    MinMaxScaler<T> scaler;
    scaler.init(points, 3);
    scaler.normalize(normalized_points);

    const auto cost_func_normalized = CostFunc<T>(normalized_points);
    const auto cost_func = CostFunc<T>(points);

    const std::pair<T, X<T>> res
        = ParticleSwarmOptimization<T, X<T>> (
            std::move(random_population),
            cost_func_normalized,
            precision,  // precision
            psrng,
            0.729,  // inertia_coef,
            1.494,  // cognitive_coef,
            1.494,  // social_coef,
            0.2,  // max_velocity
            (int) 1e3,  // max_convergence_iters
            6  // num_dimensions
        );

    const T &normalized_cost = res.first;
    const X<T> &best_x_normalized = res.second;  // normalized solution

    // display normalized result:
    // printf("\nMin normalized cost found: %lf for normalized solution:\n", normalized_cost);
     // std::cout << best_x_normalized << std::endl;
    // printf("Normalized points:\n");
    // for (const auto &point : normalized_points) {
    //     std::cout << point << ", ";
    // }

    // denormalize solution
    std::vector<sale::Point3D<T>> to_denormalize = {
        best_x_normalized.S1,
        best_x_normalized.S2
    };

    scaler.denormalize(to_denormalize);
    const X<T> best_x = { to_denormalize[0], to_denormalize[1] };

    printf("\nMin cost found: %lf for solution:\n", cost_func(best_x));
     std::cout << best_x << std::endl << std::endl;
}
