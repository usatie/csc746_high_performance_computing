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
import math

RESULT_DIR = "images/"
PLOT_FORMATS = [
    "r-o",
    "b-x",
    "g-^",
    "k-s",
    "m-v",
    "c-+",
    "y-d",
]
COLOR_LIST = ["red", "blue", "green", "black", "magenta", "cyan", "yellow"]


def save_speedup_chrono_figure():
    fname = f"data/speedup-chrono.csv"
    df = pd.read_csv(fname, comment="#")
    print(df)
    var_names = list(df.columns)
    print("var names =", var_names)
    problem_sizes = df["N"].tolist()
    basic_omp_1_time = df["omp-1"].tolist()
    basic_omp_4_time = df["omp-4"].tolist()
    basic_omp_16_time = df["omp-16"].tolist()
    basic_omp_64_time = df["omp-64"].tolist()

    save_basic_speedup(
        "Basic_Speedup_chrono.png",
        "Basic Speedup by std::chrono (omp-4, omp-16, omp-64)",
        problem_sizes,
        basic_omp_1_time,
        [basic_omp_4_time, basic_omp_16_time, basic_omp_64_time],
        64,
    )

    fname = f"data/speedup-chrono-blocked.csv"
    df = pd.read_csv(fname, comment="#")
    print(df)
    var_names = list(df.columns)
    print("var names =", var_names)
    problem_sizes = df["N"].tolist()
    b4_omp_1_time = df["omp-1 (b=4)"].tolist()
    b4_omp_4_time = df["omp-4 (b=4)"].tolist()
    b4_omp_16_time = df["omp-16 (b=4)"].tolist()
    b4_omp_64_time = df["omp-64 (b=4)"].tolist()
    b16_omp_1_time = df["omp-1 (b=16)"].tolist()
    b16_omp_4_time = df["omp-4 (b=16)"].tolist()
    b16_omp_16_time = df["omp-16 (b=16)"].tolist()
    b16_omp_64_time = df["omp-64 (b=16)"].tolist()

    save_blocked_speedup(
        "Blocked_Speedup_chrono.png",
        "BMMCO Speedup by std::chrono (omp-4, omp-16, omp-64)",
        problem_sizes,
        b4_omp_1_time,
        [b4_omp_4_time, b4_omp_16_time, b4_omp_64_time],
        b16_omp_1_time,
        [b16_omp_4_time, b16_omp_16_time, b16_omp_64_time],
        64,
    )


def save_speedup_chart(fname, title, rank_sizes, df):
    plt.title(title)
    plt.xlabel("Total Ranks") # 4, 9, 16, 25, 36, 49, 64, 81
    plt.ylabel("Speedup")     # 2ms ~ 50ms
    xlocs = [i for i in range(len(rank_sizes))]
    plt.xticks(xlocs, rank_sizes)
    #plt.yticks([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
    #plt.gca().set_yticklabels([str(i) for i in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]])

    print(fname)
    print(df)
    sobel_speedup = df["Sobel time (ms)"].iloc[0] / df["Sobel time (ms)"]
    scatter_speedup = df["Scatter time (ms)"].iloc[0] / df["Scatter time (ms)"]
    gather_speedup = df["Gather time (ms)"].iloc[0] / df["Gather time (ms)"]
    plt.plot(xlocs, sobel_speedup, PLOT_FORMATS[0], label="Sobel")
    plt.plot(xlocs, scatter_speedup, PLOT_FORMATS[1], label="Scatter")
    plt.plot(xlocs, gather_speedup, PLOT_FORMATS[2], label="Gather")
    plt.plot(xlocs, [rank / rank_sizes[0] for rank in rank_sizes], "k--", label="Ideal Speedup")
    plt.grid(axis="both")
    plt.legend(loc="best")
    #plt.show()
    plt.savefig(RESULT_DIR + fname, dpi=300)
    plt.clf()

def save_figures():
    fname = f"data/performance.csv"
    df = pd.read_csv(fname, comment="#")
    row_df = df[df['Decomposition'] == 'Row-slab']
    col_df = df[df['Decomposition'] == 'Column-slab']
    tile_df = df[df['Decomposition'] == 'Tiled']
    var_names = list(df.columns)

    print("var names =", var_names)

    # split the df into individual vars
    rank_sizes = row_df["Total Ranks"].tolist()

    # speedup
    save_speedup_chart('row_speedup.png', 'Row-slab Speedup', rank_sizes, row_df)
    save_speedup_chart('col_speedup.png', 'Column-slab Speedup', rank_sizes, col_df)
    save_speedup_chart('tile_speedup.png', 'Tiled Speedup', rank_sizes, tile_df)


if __name__ == "__main__":
    save_figures()


# EOF

