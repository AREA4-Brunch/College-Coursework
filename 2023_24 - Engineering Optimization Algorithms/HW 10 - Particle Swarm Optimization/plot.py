import matplotlib.pyplot as plt


def plot_3d_points(points):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    for point, label in points:
        ax.scatter(point[0], point[1], point[2], label=label, s=50)

    # Labeling points with their names
    for point, label in points:
        ax.text(point[0], point[1], point[2], label)

    ax.set_xlabel('X Axis')
    ax.set_ylabel('Y Axis')
    ax.set_zlabel('Z Axis')

    plt.legend()
    plt.show()


# Example usage
points_data = [
    (( 1, 4, 0 ), 'A'),
    (( 3, 2, 0 ), 'B'),
    (( 2, 7, 1 ), 'C'),
    (( 6, 3, 3 ), 'D'),
    (( 7, 6, 5 ), 'E'),
    (( 5, 7, 4 ), 'F'),

    # Solution:
    (( 2.28914, 4.43436, 0.735747 ), 'S1'),
    (( 5.40295, 5.46739, 3.59841 ), 'S2'),
]


plot_3d_points(points_data)


points_data_normalized = [
    ((0, 0.4, 0), 'A'),
    ((0.333333, 0, 0), 'B'),
    ((0.166667, 1, 0.2), 'C'),
    ((0.833333, 0.2, 0.6), 'D'),
    ((1, 0.8, 1), 'E'),
    ((0.666667, 1, 0.8), 'F'),

    # Solution:
    (( 0.214857, 0.486872, 0.147149 ), 'S1'),
    (( 0.733825, 0.693478, 0.719682 ), 'S2'),
]

plot_3d_points(points_data_normalized)
