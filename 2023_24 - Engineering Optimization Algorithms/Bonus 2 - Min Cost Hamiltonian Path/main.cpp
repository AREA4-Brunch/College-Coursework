/**
 * Find min cost Hamiltonian Path (TSP without returning to start)
 * for 20 given points.
 * Works up to 23 points using float, 22 points using double, later bad_alloc.
 */


#include <iostream>
#include <vector>
#include <complex>
#include <limits>
#include <chrono>
// #include <unordered_map>


template<typename Func, typename... Args>
void executeAndMeasureExecTime(const Func &func, Args&&... args);

template <typename TCost, typename TIterator>
void Solve(
    const TIterator &points_start,
    const TIterator &points_end
);

template<
    typename T,
    typename TSetEncoding=unsigned int,
    typename TEncoding=unsigned int
>
std::pair<T, std::vector<int>> bellmanHeldKarp(
    const std::vector<std::vector<T>> &weights,
    const bool end_in_starting_point=true
);



int main()
{
    using TPoints = double;  // up to 22 points
    // using TPoints = float;  // up to 23 points

    const std::vector<std::complex<TPoints>> points = {
        { 2.7, 33.1 },
        { 2.7, 56.8 },
        { 9.1, 40.3 },
        { 9.1, 52.8 },
        { 15.1, 49.6 },
        { 15.3, 37.8 },
        { 21.5, 45.8 },
        { 22.9, 32.7 },
        { 33.4, 60.5 },
        { 28.4, 31.7 },
        { 34.7, 26.4 },
        { 45.7, 25.1 },
        { 34.7, 45.1 },
        { 46.0, 45.1 },
        { 54.2, 29.1 },
        { 57.7, 42.1 },
        { 67.9, 19.6 },
        { 51.7, 56.0 },
        { 57.5, 56.0 },
        { 62.0, 58.4 },

        // Stress testing, for TPoints = double, up to 22 points, else bad alloc
        // { 13.2, 17.1 },
        // { 24.3, 29.7 },

        // Stress testing, for TPoints = float, up to 23 points, else bad alloc
        // { 38.1, 26.0 },
    };

    executeAndMeasureExecTime([&] () {
        Solve<TPoints>(points.begin(), points.begin() + 8);

        // a) For first 8 points:
        // Optimal total distance: 60.435828
        // Point #1 (2.7, 33.1)
        // Point #3 (9.1, 40.3)
        // Point #6 (15.3, 37.8)
        // Point #8 (22.9, 32.7)
        // Point #7 (21.5, 45.8)
        // Point #5 (15.1, 49.6)
        // Point #4 (9.1, 52.8)
        // Point #2 (2.7, 56.8)
    });

    executeAndMeasureExecTime([&] () {
        Solve<TPoints>(points.begin(), points.begin() + 12);

        // b) For first 12 points:
        // Optimal total distance: 114.773773
        // Point #9 (33.4, 60.5)
        // Point #7 (21.5, 45.8)
        // Point #5 (15.1, 49.6)
        // Point #4 (9.1, 52.8)
        // Point #2 (2.7, 56.8)
        // Point #1 (2.7, 33.1)
        // Point #3 (9.1, 40.3)
        // Point #6 (15.3, 37.8)
        // Point #8 (22.9, 32.7)
        // Point #10 (28.4, 31.7)
        // Point #11 (34.7, 26.4)
        // Point #12 (45.7, 25.1)
    });

    executeAndMeasureExecTime([&] () {
        Solve<TPoints>(points.begin(), points.end());

        // Optimal total distance for 20 points: 205.126333
        // Point #17 (67.9, 19.6)
        // Point #15 (54.2, 29.1)
        // Point #12 (45.7, 25.1)
        // Point #11 (34.7, 26.4)
        // Point #10 (28.4, 31.7)
        // Point #8 (22.9, 32.7)
        // Point #6 (15.3, 37.8)
        // Point #3 (9.1, 40.3)
        // Point #1 (2.7, 33.1)
        // Point #2 (2.7, 56.8)
        // Point #4 (9.1, 52.8)
        // Point #5 (15.1, 49.6)
        // Point #7 (21.5, 45.8)
        // Point #13 (34.7, 45.1)
        // Point #14 (46.0, 45.1)
        // Point #16 (57.7, 42.1)
        // Point #20 (62.0, 58.4)
        // Point #19 (57.5, 56.0)
        // Point #18 (51.7, 56.0)
        // Point #9 (33.4, 60.5)
    });

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


template <typename TCost, typename TIterator>
void Solve(
    const TIterator &points_start,
    const TIterator &points_end
) {
    using T = typename TIterator::value_type;
    const int num_points = std::distance(points_start, points_end);

    // calc euclidean distances between points
    std::vector<std::vector<TCost>>
        distances(num_points, std::vector<TCost> (num_points));

    TIterator row = points_start;
    for (int i = 0, n = num_points; i < n; ++i, ++row) {
        TIterator col = points_start;
        for (int j = 0, m = num_points; j < m; ++j, ++col) {
            distances[i][j] = std::abs(*row - *col);
        }
    }

    const std::pair<TCost, std::vector<int>> res = bellmanHeldKarp(distances, false);
    const TCost min_distance = res.first;
    const std::vector<int> &path = res.second;

    printf("Optimal total distance for %d points: %lf\n", num_points, min_distance);
    for (const int idx : path) {
        const std::complex<TCost> &point = *(points_start + idx);
        printf("Point #%d (%.1lf, %.1lf)\n", idx + 1, point.real(), point.imag());
    }
}


/// @tparam TSetEncoding - e.g. set_t = uint = 32bits, uses N bits, N=weights.size()
/// @tparam TEncoding - e.g. enc_t = uint = 32bits, max >= (1 + 1 << N) * N
/// @param weights - weights[src][dst] = weight of edge between src->dst
/// @return (min_cost, vertices making up the path)
template<typename T, typename TSetEncoding, typename TEncoding>
std::pair<T, std::vector<int>> bellmanHeldKarp(
    const std::vector<std::vector<T>> &weights,
    const bool end_in_starting_point
) {
    using ull = unsigned long long;
    constexpr const T inf = std::numeric_limits<T>::max();

    const int n = weights.size();

    using set_t = TSetEncoding;  // set_t = uint = 32bits, uses n-1 bits
    using enc_t = TEncoding;  // must handle (1 + 1 << (N - 1)) * N

    // check if max set encoding val in can be constructed
    if ( static_cast<ull> (std::numeric_limits<set_t>::max())
      < (1ULL << (n)) - 1ULL
    ) {
        std::cerr << "Cannot handle num of vertices V=" << n
                  << "with TSetEncoding's num of bits, required at least" << n
                  << " checked using max val: "
                  << static_cast<ull> (std::numeric_limits<set_t>::max())
                  << " which must be >= " << (1ULL << (n)) - 1ULL
                  << std::endl;
        return { inf, { } };
    }

    const set_t all_vertices = (((set_t) 1) << (n)) - 1;
    // exclusive, encoding in [1, max_enc_val), +1ULL to fit +vertex part of encoding
    const ull max_enc_val = (1ULL + all_vertices) * n;  // == ((2^n - 1) + 1) * n

    // check if max encoding val in costs can be constructed
    if ( static_cast<ull> (std::numeric_limits<enc_t>::max())
       < max_enc_val - 1ULL
    ) {
        std::cerr << "Cannot handle num of vertices V=" << n
                  << "with TEncoding's num of bits since max val: "
                  << std::numeric_limits<set_t>::max()
                  << " which must be >= " << max_enc_val - 1ULL
                  << std::endl;
        return { inf, { } };
    }

    // costs[v_set] = min cost of path that passes all vertices
    // in `set` and ends at vertex `v`,
    // store set as int, i-th bit == 1 mean i-th vertex is in the set
    // encode pairs int vertex and int set since vertex < N as:
    // enc = set * N + vertex => vertex = enc % N, set = enc / N
    std::vector<T> costs(max_enc_val);
    // std::unordered_map<enc_t, T> costs;
    // costs.reserve(static_cast<ull> (n) * (all_vertices + 1));

    auto was_visited = [] (const set_t set, const int vertex) {
        return set & (((set_t) 1) << vertex);
    };

    auto add_to_set = [] (const set_t set, const int vertex) -> set_t {
        return set | (((set_t) 1) << vertex);
    };

    auto remove_from_set = [] (const set_t set, const int vertex) -> set_t {
        return set ^ (((set_t) 1) << vertex);
    };

    /// @param vertex - vertex in [0, n)
    auto encode_vertex_set = [&] (const set_t set, const int vertex) -> enc_t {
        return static_cast<enc_t>(static_cast<enc_t>(set) * n + vertex);
    };

    // best_previous_vertices[set_dst] = vertex that comes before `dst` in path `set`
    std::vector<int> best_previous_vertices(max_enc_val);
    // std::unordered_map<enc_t, int> best_previous_vertices(
    //     static_cast<ull> (n) * (all_vertices + 1)
    // );

    // init paths with length 1, n -> vertex
    for (int dst = 0; dst < n; ++dst) {  // all but n
        const set_t v_set = ((set_t) 1) << dst;
        const enc_t v_set_key = encode_vertex_set(v_set, dst);
        costs[v_set_key] = (T) 0;
    }

    // go up to < all_vertices, afterwards no more vertices left to connect
    for (set_t set = 1; set < all_vertices; ++set) {
        // go through all vertices and connect everything
        // not in cur path/set to vertices in cur path
        for (int dst = 0; dst < n; ++dst) {
            if (was_visited(set, dst)) continue;

            const set_t added_dst_to_path = add_to_set(set, dst);
            const enc_t dst_set_enc = encode_vertex_set(added_dst_to_path, dst);
            costs[dst_set_enc] = inf;

            // connect cur path/set through each possible end/src
            // to new vertex dst at lowest cost
            for (int src = 0; src < n; ++src) {
                if (!was_visited(set, src)) continue;

                const enc_t src_set_key = encode_vertex_set(set, src);

                if (costs[src_set_key] < costs[dst_set_enc] - weights[src][dst]) {
                    costs[dst_set_enc] = costs[src_set_key] + weights[src][dst];
                    best_previous_vertices[dst_set_enc] = src;
                }
            }
        }
    }

    // of all optimally minimized ends choose best one
    T full_path_cost = inf;
    int best_end_vertex = -1;
    for (int vertex = 0; vertex < n; ++vertex) {
        const enc_t vertex_all_enc = encode_vertex_set(all_vertices, vertex);
        T edge_cost = (T) 0;

        if (end_in_starting_point) {
            // find the starting point and the cost to close the cycle
            set_t cur_set = all_vertices;
            int starting_vertex = vertex;
            for (int i = n - 2; i >= 0; --i) {
                const enc_t v_set_key = encode_vertex_set(cur_set, starting_vertex);
                cur_set = remove_from_set(cur_set, starting_vertex);
                starting_vertex = best_previous_vertices[v_set_key];
            }
            edge_cost = weights[vertex][starting_vertex];
        }

        if (costs[vertex_all_enc] < full_path_cost - edge_cost) {
            full_path_cost = costs[vertex_all_enc] + edge_cost;
            best_end_vertex = vertex;
        }
    }

    std::vector<int> path(end_in_starting_point ? n + 1 : n);
    path[n - 1] = best_end_vertex;

    set_t cur_set = all_vertices;
    for (int i = n - 2; i >= 0; --i) {
        const int cur_end = path[i + 1];
        const enc_t v_set_key = encode_vertex_set(cur_set, cur_end);
        path[i] = best_previous_vertices[v_set_key];
        cur_set = remove_from_set(cur_set, cur_end);
    }

    if (end_in_starting_point) path[n] = path[0];

    return { full_path_cost, std::move(path) };
}
