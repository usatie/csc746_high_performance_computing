"""

Shun Usami, Copyright (C) 2024

November 2024

Description: This code loads a .csv file and creates a 3-variable plot

Inputs: the named file "data/performance.csv"

Outputs: displays charts with matplotlib

Dependencies: matplotlib, pandas modules

Assumptions: developed and tested using Python version 3.9.1 on macOS 14.5

"""

import pandas as pd
import matplotlib.pyplot as plt
import os

RESULT_DIR = "images/"
PLOT_FORMATS = ["r-o", "b-x", "g-^", "k-s", "m-v", "c-+", "y-d"]

def load_data(file_path):
    df = pd.read_csv(file_path, comment="#")
    print(df)
    return df

def save_chart(fname, title, x_values, y_values_list, labels, xlabel, ylabel):
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    xlocs = [i for i in range(len(x_values))]
    plt.xticks(xlocs, x_values)

    for idx, y_values in enumerate(y_values_list):
        plt.plot(xlocs, y_values, PLOT_FORMATS[idx], label=labels[idx])

    plt.plot(xlocs, [rank / x_values[0] for rank in x_values], "k--", label="Ideal Speedup")
    plt.grid(axis="both")
    plt.legend(loc="best")

    os.makedirs(RESULT_DIR, exist_ok=True)
    plt.savefig(os.path.join(RESULT_DIR, fname), dpi=300)
    plt.clf()

def process_speedup_data(df, title, decomposition_label):
    selected_df = df[df['Decomposition'] == decomposition_label]
    rank_sizes = selected_df["Total Ranks"].tolist()

    # Extracting time columns for speedup calculations
    sobel_time = selected_df["Sobel time (ms)"]
    scatter_time = selected_df["Scatter time (ms)"]
    gather_time = selected_df["Gather time (ms)"]

    # Calculate speedups
    sobel_speedup = sobel_time.iloc[0] / sobel_time
    scatter_speedup = scatter_time.iloc[0] / scatter_time
    gather_speedup = gather_time.iloc[0] / gather_time

    save_chart(
        f"{decomposition_label.lower()}_speedup.png",
        f"{decomposition_label} Speedup",
        rank_sizes,
        [sobel_speedup, scatter_speedup, gather_speedup],
        ["Sobel", "Scatter", "Gather"],
        "Total Ranks",
        "Speedup"
    )

def save_figures():
    data_file = "data/performance.csv"
    df = load_data(data_file)
    for decomposition in ["Row-slab", "Column-slab", "Tiled"]:
        process_speedup_data(df, f"{decomposition} Speedup", decomposition)

if __name__ == "__main__":
    save_figures()

# EOF

