import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

# File path to the CSV file
file_path = "results/20240429-113440/memory20240429-113440.txt"

# Read the data from the CSV file into a pandas DataFrame
df = pd.read_csv(file_path, header=None, names=["Test", "Size", "Density", "Tree", "Frederickson", "Randomized", "Even"])

# Remove leading and trailing whitespace from the "Size" column
df["Size"] = df["Size"].str.strip()

# Melt the DataFrame to have a single runtime column
df_melted = pd.melt(df, id_vars=["Test", "Size", "Density", "Tree"], value_vars=["Frederickson", "Randomized", "Even"], var_name="Algorithm", value_name="Runtime")
df_melted["Runtime"] = pd.to_numeric(df_melted["Runtime"], errors="coerce")

# Print unique values in the "Size" column
print("Unique sizes in the DataFrame:")
print(df_melted["Size"].unique())

# Set the style of seaborn
sns.set(style="whitegrid")

# Filter the DataFrame for size 100
df_size_100 = df_melted[df_melted["Size"] == "100"]

# Check if there's any data in the filtered DataFrame before plotting
if not df_size_100.empty:
    # Plot the KDE distributions for size 100
    plt.figure(figsize=(10, 6))
    sns.kdeplot(data=df_size_100, x="Runtime", hue="Algorithm", fill=True, common_norm=False)
    plt.title('Distribution of Memory Usage for Size 100 Across Algorithms')
    plt.xlabel('Memory Usage (B)')
    plt.ylabel('Density')
    plt.xlim(left=0)  # Set x-axis limit to start from 0

    # Specify the folder path for saving the plot
    folder_path = "results/20240429-113440/"

    # Save the plot for size 100
    plt.savefig(folder_path + "mem_size_100.png")

    # Show the plot
    plt.show()
else:
    print("No data available for size 100.")

# Filter the DataFrame for size 1000
df_size_1000 = df_melted[df_melted["Size"] == "1000"]

# Check if there's any data in the filtered DataFrame before plotting
if not df_size_1000.empty:
    # Plot the KDE distributions for size 1000
    plt.figure(figsize=(10, 6))
    sns.kdeplot(data=df_size_1000, x="Runtime", hue="Algorithm", fill=True, common_norm=False)
    plt.title('Distribution of Memory Usage for Size 1000 Across Algorithms')
    plt.xlabel('Memory Usage (B)')
    plt.ylabel('Density')
    plt.xlim(left=0)  # Set x-axis limit to start from 0

    # Save the plot for size 1000
    plt.savefig(folder_path + "mem_size_1000.png")

    # Show the plot
    plt.show()
else:
    print("No data available for size 1000.")
