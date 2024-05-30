import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def boxPlot(filename, size):
    # Read data from the text file
    data = pd.read_csv(filename)

    # Define the types of trees
    tree_types = [' caterpillar', ' path', ' star', ' lobster', ' random', ' starlike']

    # Find the index of the "exact" column
    exact_idx = data.columns.get_loc(" exact")

    # Define the columns to be plotted (columns to the right of "exact")
    columns_to_plot = data.columns[exact_idx+1:]

    # Create a figure to hold all subplots
    fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(15, 10))
    fig.suptitle(f'Approximation Ratio for each tree type (n = {size})')

    # Iterate over each type of tree
    for row_idx, tree_type in enumerate(tree_types):
        # Filter data for the current tree type and size 's'
        filtered_data = data[(data[' tree'] == tree_type) & (data[' size'] == size)]
        
        # Calculate subplot position
        row = row_idx // 3
        col = row_idx % 3

        # Compute the ratios
        ratios = filtered_data[columns_to_plot].div(filtered_data[" exact"], axis=0)

        # Create box and whiskers plot for the ratios
        axes[row, col].boxplot([ratios[column].dropna() for column in columns_to_plot], positions=range(1, len(columns_to_plot) + 1))

        # Mark the mean with an 'X' inside the box
        for algorithm_idx, column in enumerate(columns_to_plot):
            mean_value = ratios[column].mean()
            axes[row, col].scatter(algorithm_idx + 1, mean_value, color='r', marker='x', s=100)

        axes[row, col].set_title(tree_type.capitalize())
        axes[row, col].set_ylabel('Approximation Ratio')
        axes[row, col].set_xlabel('Algorithm')
        axes[row, col].set_xticks(range(1, len(columns_to_plot) + 1))
        axes[row, col].set_xticklabels([col.capitalize() for col in columns_to_plot], rotation=45, ha='right')

    # Adjust layout with padding between subplots
    plt.subplots_adjust(wspace=0.4, hspace=0.4)

    # Save the figure
    plt.savefig(f'{filename[:-4]}_Boxplot_{size}.png')

if __name__ == '__main__':
    boxPlot("results20240530-042421.txt", 100)
