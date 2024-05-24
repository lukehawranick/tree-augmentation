import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

# File path to the CSV file
file_path = "time20240522-041056.txt"

# Read the data from the CSV file into a pandas DataFrame
df = pd.read_csv(file_path, header=None, names=["Test", "Size", "Density", "Tree", "Randomized", "Exact"])

# Remove leading and trailing whitespace from the "Size" column
df["Size"] = df["Size"].str.strip()

# Melt the DataFrame to have a single runtime column
df_melted = pd.melt(df, id_vars=["Test", "Size", "Density", "Tree"], value_vars=["Randomized", "Exact"], var_name="Algorithm", value_name="Runtime")
df_melted["Runtime"] = pd.to_numeric(df_melted["Runtime"], errors="coerce")

# Set the style of seaborn
sns.set(style="whitegrid")

# Define the types of trees
tree_types = df_melted["Tree"].unique()[1:]

# Filter data for sizes 100 and 1000
df_size_100 = df_melted[df_melted["Size"] == "100"]
df_size_1000 = df_melted[df_melted["Size"] == "1000"]

print(df_size_100)

# Iterate over each size and create 2x3 KDE distribution plots
for size_df, size_name in zip([df_size_100, df_size_1000], ["size_100", "size_1000"]):
    # Create a grid of subplots with 2 rows and 3 columns
    fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(15, 10))

    # Iterate over each type of tree and plot KDE distributions
    for i, tree_type in enumerate(tree_types):
        if i < 6:  # Ensure i is within the bounds of axes array
            row = i // 3  # Determine the row index
            col = i % 3   # Determine the column index
            filtered_data = size_df[size_df["Tree"] == tree_type]
            if not filtered_data.empty:
                sns.kdeplot(data=filtered_data, x="Runtime", hue="Algorithm", fill=True, common_norm=False, ax=axes[row, col])
                axes[row, col].set_title(tree_type.capitalize())
                axes[row, col].set_ylabel('Density')
                axes[row, col].set_xlabel('Runtime (s)')
                axes[row, col].set_xlim(left=0)  # Set x-axis limit to start from 0
            else:
                axes[row, col].text(0.5, 0.5, "No Data", horizontalalignment='center', verticalalignment='center', fontsize=12, color='red')
                axes[row, col].axis('off')  # Hide the axes for empty subplots

    # Adjust layout with padding between subplots
    plt.tight_layout()

    # Specify the folder path for saving the plot
    folder_path = ""

    # Save the plot
    plt.savefig(folder_path + f"runtime_dist_{size_name}.png")

    # Show the plots
    #plt.show()



