import gc
import numpy as np
import matplotlib.pyplot as plt
import struct




def main():
    avg_best_costs = plot_cumulative_min_and_preprocess()
    plot_avg_best_costs(avg_best_costs)

    return


def plot_cumulative_min_and_preprocess():
    fig, ax = plt.subplots(1)

    avg_best_costs = []
    with open('costs_histories.bin', 'rb') as file:
        # read the number of vectors, Q - unsigned long long, 8 bytes;
        # i - integer 4 bytes
        num_costs = struct.unpack('Q', file.read(8))[0]

        batches_per_run = 10
        num_runs = num_costs // batches_per_run
        for idx in range(num_runs):
            costs = []
            for batch in range(batches_per_run):
                batch_size = struct.unpack('Q', file.read(8))[0]
                batch = struct.unpack(f'{batch_size}i', file.read(batch_size * 4))
                costs += list(batch)

            # store cumulative mins in costs, and sum them up for avgs calc
            for i in range(1, len(costs)):
                costs[i] = min(costs[i], costs[i - 1])
                # add costs[i] to avg_best_costs, expand array if adding
                # ith sample for 1st time
                if i >= len(avg_best_costs): avg_best_costs.append(costs[i])
                else: avg_best_costs[i] += costs[i];

            ax.loglog(range(1, 1 + len(costs)), costs, label=f'run #{idx + 1}')

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

    ax.set_xlabel('iter')
    ax.set_ylabel('cost / optimization func value')
    ax.legend(loc='upper right', fontsize='small')
    plt.show()


if __name__ == '__main__':
    main()
