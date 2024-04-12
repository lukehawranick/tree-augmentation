import pandas as pd
import matplotlib.pyplot as plt

"""
FILE NEEDS REFACTORING
"""

# Read the data from the file
data = pd.read_csv("time20240412-072026.txt")

# Filter the data for size 10 and size 50
data_size_10 = data[data[' size'] == 10]
data_size_50 = data[data[' size'] == 50]

# Create separate stacked area plots for size 10 and size 50
plt.figure(figsize=(12, 6))

# Plot for size 10
plt.subplot(1, 2, 1)
for col in [' frederickson', ' randomized', ' exact', ' even']:
    plt.stackplot(data_size_10.index, data_size_10[col], labels=[col])
plt.xlabel('Test')
plt.ylabel('Time')
plt.title('Size 10')
plt.legend(loc='upper left')

# Save the plot for size 10
plt.savefig('size_10_plot.png')

# Plot for size 50
plt.subplot(1, 2, 2)
for col in [' frederickson', ' randomized', ' exact', ' even']:
    plt.stackplot(data_size_50.index, data_size_50[col], labels=[col])
plt.xlabel('Test')
plt.ylabel('Time')
plt.title('Size 50')
plt.legend(loc='upper left')

# Save the plot for size 50
plt.savefig('size_50_plot.png')

plt.tight_layout()
plt.show()
