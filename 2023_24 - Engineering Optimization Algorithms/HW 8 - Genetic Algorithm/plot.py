import sys
import gc
import numpy as np
import matplotlib.pyplot as plt
import struct


NUM_RUNS = 20


def main():
    if len(sys.argv) == 1:
        homework()

    elif sys.argv[1] == '--compare_hyperparam_searches':
        compare_hyperparam_searches(
            start_search_idx=int(sys.argv[2]),
            end_search_idx=int(sys.argv[3])
        )

    else:
        raise Exception(f'Invalid script arg: {sys.argv[1]}')

    return


def homework():
    avg_best_costs = plot_cumulative_min_and_preprocess('costs_histories.bin')
    plot_avg_best_costs(avg_best_costs)
    # compare_simulated_annealing_with_genetic_algo(avg_best_costs)


def compare_hyperparam_searches(
    start_search_idx: int,
    end_search_idx: int
):
    """ Indices range is inclusive. """

    path_base = 'costs_histories'

    fig, ax = plt.subplots(1)
    for search in range(start_search_idx, 1 + end_search_idx):
        print(f'\nSearch #{search}')
        path = f'{path_base}_{search}.bin'
        avg_best_costs = fetch_and_compute_avg_best_costs(path, True)

        ax.loglog(
            range(1, 1 + len(avg_best_costs)),
            avg_best_costs,
            label=f'Avg min cost search #{search}'
        )

    ax.set_title('Average mins of cost function per iteration for all runs per hyperparam search')
    ax.set_xlabel('iter')
    ax.set_ylabel('cost / optimization func value')
    ax.legend(loc='upper right', fontsize='small')
    plt.show()


def plot_cumulative_min_and_preprocess(path_costs_histories: str):
    fig, ax = plt.subplots(1)

    avg_best_costs = [ 0 ]
    with open(path_costs_histories, 'rb') as file:
        # read the number of vectors, Q - unsigned long long, 8 bytes;
        # i - integer 4 bytes
        num_runs = NUM_RUNS

        costs_per_generation = 20000
        for run in range(1, 1 + num_runs):
            print(f'processing run #{run} ...')

            costs = []
            num_gens = struct.unpack('Q', file.read(8))[0]
            for _ in range(num_gens):
                batch_size = struct.unpack('Q', file.read(8))[0]
                assert batch_size == costs_per_generation
                batch = struct.unpack(f'{batch_size}i', file.read(batch_size * 4))
                costs.extend(batch)

            # store cumulative mins in costs, and sum them up for avgs calc
            avg_best_costs[0] += costs[0]
            for i in range(1, len(costs)):
                costs[i] = min(costs[i], costs[i - 1])
                # add costs[i] to avg_best_costs, expand array if adding
                # ith sample for 1st time
                if i >= len(avg_best_costs): avg_best_costs.append(costs[i])
                else: avg_best_costs[i] += costs[i];

            ax.loglog(range(1, 1 + len(costs)), costs, label=f'run #{run}')

    ax.set_title('Cumulative min of cost function per iteration')
    ax.set_xlabel('iter')
    ax.set_ylabel('cost / optimization func value')
    ax.legend(loc='upper right', fontsize='small')
    plt.show()

    gc.collect()
    avg_best_costs = np.array(avg_best_costs) / num_runs
    return avg_best_costs


def plot_avg_best_costs(avg_best_costs):
    fig, ax = plt.subplots(1)

    ax.loglog(
        range(1, 1 + len(avg_best_costs)),
        avg_best_costs,
        label='Average minimum cost'
    )
    ax.set_title('Average min of cost function per iteration for all runs')
    ax.set_xlabel('iter')
    ax.set_ylabel('cost / optimization func value')
    ax.legend(loc='upper right', fontsize='small')
    plt.show()


def compare_simulated_annealing_with_genetic_algo(
    avg_best_costs_genetic=None
):
    if avg_best_costs_genetic is None:
        avg_best_costs_genetic = fetch_and_compute_avg_best_costs(
            'costs_histories.bin',
            is_reading_from_generative_algo_output=True
        )

    avg_best_costs_sim_annealing = fetch_and_compute_avg_best_costs(
        'costs_histories_old.bin',
        is_reading_from_generative_algo_output=False
    )

    fig, ax = plt.subplots(1)
    ax.loglog(
        range(1, 1 + len(avg_best_costs_genetic)),
        avg_best_costs_genetic,
        label='Average minimum cost genetic algo'
    )
    ax.loglog(
        range(1, 1 + len(avg_best_costs_sim_annealing)),
        avg_best_costs_sim_annealing,
        label='Average minimum cost simmulated (re)annealing'
    )
    ax.set_title('Compared avg min cost per iteration for all runs of genetic and sim. annealing')
    ax.set_xlabel('iter')
    ax.set_ylabel('cost / optimization func value')
    ax.legend(loc='upper right', fontsize='small')
    plt.show()


def fetch_and_compute_avg_best_costs(
    path: str,
    is_reading_from_generative_algo_output: bool
):
    avg_best_costs = [ 0 ]
    with open(path, 'rb') as file:
        # read the number of vectors, Q - unsigned long long, 8 bytes;
        # i - integer 4 bytes
        num_runs = NUM_RUNS
        if not is_reading_from_generative_algo_output:
            total_num_anneals = struct.unpack('Q', file.read(8))[0]

        for run in range(1, 1 + num_runs):
            print(f'processing run #{run} ...')

            costs = []
            if is_reading_from_generative_algo_output:
                num_gens = struct.unpack('Q', file.read(8))[0]
            else:
                num_gens = 10

            for _ in range(num_gens):
                batch_size = struct.unpack('Q', file.read(8))[0]
                batch = struct.unpack(f'{batch_size}i', file.read(batch_size * 4))
                costs.extend(batch)

            # store cumulative mins in costs, and sum them up for avgs calc
            avg_best_costs[0] += costs[0]
            for i in range(1, len(costs)):
                costs[i] = min(costs[i], costs[i - 1])
                # add costs[i] to avg_best_costs, expand array if adding
                # ith sample for 1st time
                if i >= len(avg_best_costs): avg_best_costs.append(costs[i])
                else: avg_best_costs[i] += costs[i];

    gc.collect()
    avg_best_costs = np.array(avg_best_costs) / num_runs
    return avg_best_costs



if __name__ == '__main__':
    main()
