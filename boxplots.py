import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def boxPlot(filename, size):
    # Read data from the text file
    data = pd.read_csv(filename)

    # Define the types of trees
    tree_types = [' caterpillar', ' path', ' star', ' lobster', ' random', ' starlike']

    # Find the index of the "frederickson" column
    frederickson_idx = data.columns.get_loc(" frederickson")

    # Define the columns to be plotted (columns to the right of "frederickson")
    columns_to_plot = data.columns[frederickson_idx:]

    # Create a figure to hold all subplots
    fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(15, 10))
    fig.suptitle('Box and Whiskers Plot for Each Tree Type')

    # Iterate over each type of tree
    for row_idx, tree_type in enumerate(tree_types):
        # Filter data for the current tree type and size 's'
        filtered_data = data[(data[' tree'] == tree_type) & (data[' size'] == size)]
        
        # Calculate subplot position
        row = row_idx // 3
        col = row_idx % 3

        # Iterate over each column to plot
        for algorithm_idx, column in enumerate(columns_to_plot):
            # Get data for the current column
            column_data = filtered_data[column]

            # Create box and whiskers plot for the current column
            axes[row, col].boxplot(column_data, positions=[algorithm_idx + 1])

            # Mark the mean with an 'X' inside the box
            mean_value = np.mean(column_data)
            axes[row, col].scatter(algorithm_idx + 1, mean_value, color='r', marker='x', s=100)

        axes[row, col].set_title(tree_type.capitalize())
        axes[row, col].set_ylabel('Value')
        axes[row, col].set_xlabel('Algorithm')
        axes[row, col].set_xticks(range(1, len(columns_to_plot) + 1))
        axes[row, col].set_xticklabels([col.capitalize() for col in columns_to_plot], rotation=45, ha='right')

    # Adjust layout with padding between subplots
    plt.subplots_adjust(wspace=0.4, hspace=0.4)

    # Save the figure
    plt.savefig(f'BoxPlot{size}-{filename[:-3]}.png')


if __name__ == '__main__':
    boxPlot("memory20240412-072026.txt", 10)
    boxPlot("memory20240412-072026.txt", 50)