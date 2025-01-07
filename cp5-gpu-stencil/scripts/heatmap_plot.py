import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd

# Load data
data = pd.read_csv("data/data.csv")

# Rename columns for easier access
data.columns = ['Threads_per_block', 'Blocks', 'Runtime_ms', 'Achieved_Occupancy', 'Memory_Bandwidth_Utilization']

# Pivot tables for heatmap plotting
# Traceback (most recent call last):
#   File "/Users/shunusami/workspace/sfsu/csc746_high_performance_computing/csc746-cp5-gpu-stencil/heatmap_plot.py", line 14, in <module>
#     runtime_pivot = data.pivot("Threads_per_block", "Blocks", "Runtime_ms")
# TypeError: pivot() takes 1 positional argument but 4 were given
runtime_pivot = data.pivot(index="Threads_per_block", columns="Blocks", values="Runtime_ms")
occupancy_pivot = data.pivot(index="Threads_per_block", columns="Blocks", values="Achieved_Occupancy")
bandwidth_pivot = data.pivot(index="Threads_per_block", columns="Blocks", values="Memory_Bandwidth_Utilization")

# Plotting each heatmap
def plot_heatmap(data, title, xlabel, ylabel, color_map):
    plt.figure(figsize=(8, 6))
    sns.heatmap(data, annot=True, fmt=".2f", cmap=color_map, cbar_kws={'label': title})
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.xticks(rotation=45)
    plt.yticks(rotation=0)
    plt.tight_layout()
    plt.savefig(f"output/{title}.png")

# Runtime heatmap
plot_heatmap(runtime_pivot, "Runtime (ms)", "Blocks", "Threads per block", "coolwarm")

# Achieved Occupancy heatmap
plot_heatmap(occupancy_pivot, "Achieved Occupancy (%)", "Blocks", "Threads per block", "YlGnBu")

# Memory Bandwidth Utilization heatmap
plot_heatmap(bandwidth_pivot, "Memory Bandwidth Utilization (%)", "Blocks", "Threads per block", "OrRd")
