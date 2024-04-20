""" Generate 10^6 points, calculate their costs using some func and find
    the Pareto front.
"""

import numpy as np
import matplotlib.pyplot as plt
import gc
import time  # measure exec time


SEED = None
# SEED = 1
NUM_RAND_X = int(1e6)
# NUM_RAND_X = int(5e1)



def main():
    start_time = time.time()
    Solve()
    end_time = time.time()
    print(f'Execution time: {end_time - start_time} seconds')



def Solve():
    # maximize H, minimze R
    axis_sorting_dirs = [ 'max', 'min' ]

    points_costs, points = generate_random_points_costs(
        calc_costs,
        len(axis_sorting_dirs),
        NUM_RAND_X,
        do_return_orig_points=True,
        seed=SEED,
    )

    is_pareto, points_costs, sorted_indices = find_pareto_indices(
        axis_sorting_dirs,
        points_costs
    )

    # Display the pareto costs front:
    non_pareto_costs = points_costs[:, ~is_pareto]
    pareto_costs = points_costs[:, is_pareto]
    gc.collect()
    print(f'Num pareto costs: {pareto_costs.shape[1]}')
    plot_pareto_costs(pareto_costs, non_pareto_costs)

    # Display the pareto points front:
    if points is not None:
        points = points[:, sorted_indices]
        non_pareto_points = points[:, ~is_pareto]
        pareto_points = points[:, is_pareto]
        gc.collect()
        plot_pareto_points(pareto_points, non_pareto_points)


def generate_random_points_costs(
    calc_costs: callable,
    cost_dimensionality,
    num_points,
    do_return_orig_points=False,
    seed=None
):
    """ Generate points and store their costs.
        Return costs and optionally the generated points.
    """
    start_time = time.time()

    psrng = np.random if seed is None else np.random.RandomState(seed)

    def gen_x():
        a = psrng.uniform(1e-2, 5e-2)  # [m]
        b = psrng.uniform(0.1, 0.4)  # [m]
        S = psrng.uniform(0.5 * 1e-6, 3 * 3e-6)  # [m^2]

        return (a, b, S)

    if do_return_orig_points:
        x_dimensionality = 3
        points = np.empty(shape=(x_dimensionality, num_points))
    else:
        points = None;

    points_costs = np.empty(shape=(cost_dimensionality, num_points))
    for i in range(points_costs.shape[1]):
        if do_return_orig_points:
            points[:, i] = gen_x()
            points_costs[:, i] = calc_costs(*points[:, i])
        else:
            points_costs[:, i] = calc_costs(*gen_x())


    end_time = time.time()
    print(f'Execution time of generate_random_points_costs: {end_time - start_time} seconds')

    return points_costs, points


def calc_costs(a, b, S):
    sigma = 58 * 1e6  # [Siemens/m]
    I = 1  # [A]
    delta = 1e-2

    def calc_d(S):
        return np.sqrt(4 * S / np.pi)

    def calc_N(b, S):
        return b * np.sqrt(np.pi / (4. * S))

    def calc_z0(b):
        return b / 2. + delta

    def calc_H(a, b, S, z0):
        cost = I / 2. * np.sqrt(np.pi / (4. * S))
        cost *= (z0 + b / 2.) / np.sqrt(a * a + (z0 + b / 2.) ** 2) \
              - (z0 - b / 2.) / np.sqrt(a * a + (z0 - b / 2.) ** 2)
        return cost

    def calc_R(a, S, N):
        return N * 2 * np.pi * a / (sigma * S)

    z0 = calc_z0(b)
    N = calc_N(b, S)
    H = calc_H(a, b, S, z0)
    R = calc_R(a, S, N)

    return (H, R)


def find_pareto_indices(
    axis_sorting_dirs: list[str],
    points_costs: np.array
):
    """
    Args:
        axis_sorting_dirs (list[str]): [ 'min', 'max' ] => minimize axis 0,
                                                           maximize axis 1
        points_costs (np.array): Modifies the points_costs.

    Returns:
        pareto_indices: 1D bool array with value True at indices
                        corresponding to points_costs of the Pareto front
        points_costs:   provided `points_costs` sorted in place
        sorted_indices: np.array of indices representing reordering/sorting order
                        of points_costs, can reorder points to fit the returned
                        `pareto_indices`: points = points[:, sorted_indices]
    """

    start_time = time.time()

    # flip the first axis target sorting order
    axis_sorting_dirs_modified = [ el for el in axis_sorting_dirs ]
    axis_sorting_dirs_modified[0] = 'max' if axis_sorting_dirs[0] == 'min' else 'min'

    # for maximizing multiply with -1
    for dim, dir in enumerate(axis_sorting_dirs_modified):
        if dir == 'max': points_costs[dim, :] *= -1;

    sorted_indices = np.lexsort(points_costs)
    points_costs = points_costs[:, sorted_indices]

    # undo the change since sorting is over
    for dim, dir in enumerate(axis_sorting_dirs_modified):
        if dir == 'max': points_costs[dim, :] *= -1;

    pareto_indices = np.zeros(shape=(points_costs.shape[1]), dtype=bool)
    cur_min_max_axis0 = float('inf') if axis_sorting_dirs[0] == 'min' \
                        else float('-inf')

    for i, cur_axis0 in enumerate(points_costs[0]):
        if axis_sorting_dirs[0] == 'min':
            if cur_axis0 < cur_min_max_axis0:  # !important strictly less
                pareto_indices[i] = True
                cur_min_max_axis0 = cur_axis0
        else:
            if cur_axis0 > cur_min_max_axis0:  # !important strictly greater
                pareto_indices[i] = True
                cur_min_max_axis0 = cur_axis0

    end_time = time.time()
    print(f'Execution time of find_pareto_indices: {end_time - start_time} seconds')

    return pareto_indices, points_costs, sorted_indices


def plot_pareto_costs(
    pareto_costs: np.array,
    non_pareto_costs: np.array
):
    fig, ax = plt.subplots(1)

    ax.scatter(
        non_pareto_costs[0],
        non_pareto_costs[1],
        color='blue',
        label='not in pareto front',
        s=25,
    )

    ax.scatter(
        pareto_costs[0],
        pareto_costs[1],
        color='red',
        label='in pareto front',
        s=25,
    )

    ax.set_title('Pareto front costs, maximize H, minimize R')
    ax.set_xlabel('cost H')
    ax.set_ylabel('cost R')
    ax.legend(loc='upper left', fontsize='small')

    plt.show()


def plot_pareto_points(
    pareto_points,
    non_pareto_points
):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    ax.scatter(
        non_pareto_points[0],
        non_pareto_points[1],
        non_pareto_points[2],
        label='not in pareto front',
        color='blue',
        s=25,
    )

    ax.scatter(
        pareto_points[0],
        pareto_points[1],
        pareto_points[2],
        label='in pareto front',
        color='red',
        s=25,
    )

    ax.set_xlabel('a')
    ax.set_ylabel('b')
    ax.set_zlabel('S')
    ax.legend(loc='upper left', fontsize='small')

    plt.legend()
    plt.show()


if __name__ == '__main__':
    main()
