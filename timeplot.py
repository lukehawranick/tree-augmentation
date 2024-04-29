import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

# File path to the CSV file
file_path = "results/20240429-062207/memory20240429-062207.txt"

# Read the data from the CSV file into a pandas DataFrame
df = pd.read_csv(file_path, header=None, names=["Test", "Size", "Density", "Tree", "Frederickson", "Randomized", "Exact", "Even"])

# Melt the DataFrame to have a single runtime column
df_melted = pd.melt(df, id_vars=["Test", "Tree"], value_vars=["Frederickson", "Randomized", "Exact", "Even"], var_name="Algorithm", value_name="Runtime")
df_melted["Runtime"] = pd.to_numeric(df_melted["Runtime"], errors="coerce")
# Set the style of seaborn
sns.set(style="whitegrid")

# Plot the KDE distributions for each algorithm
plt.figure(figsize=(10, 6))
sns.kdeplot(data=df_melted, x="Runtime", hue="Algorithm", fill=True, common_norm=False)
plt.title('Distribution of Runtimes for 100 Tests Across Algorithms')
plt.xlabel('Runtime')
plt.ylabel('Density')

# Set x-axis limit to start from 0
plt.xlim(left=0)

# Specify the folder path
folder_path = "results/20240429-062207/"

# Save the plot to the specified folder
plt.savefig(folder_path + "mem.png")
