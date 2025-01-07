"""

E. Wes Bethel, Copyright (C) 2022

October 2022

Description: This code loads a .csv file and creates a 3-variable plot

Inputs: the named file "sample_data_3vars.csv"

Outputs: displays a chart with matplotlib

Dependencies: matplotlib, pandas modules

Assumptions: developed and tested using Python version 3.8.8 on macOS 11.6

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


def save_basic_speedup(
    plot_fname,
    plot_title,
    problem_sizes,
    serial_runtime,
    parallel_runtimes,
    num_threads,
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    plt.xticks(xlocs, problem_sizes)

    ideal_speedups = [[num_threads for i in range(len(problem_sizes))] for num_threads in [4, 16, 64]]
    actual_speedups = [
        [serial / parallel for serial, parallel in zip(serial_runtime, parallel_runtime)]
        for parallel_runtime in parallel_runtimes
    ]

    for i in range(len(parallel_runtimes)):
        plt.plot(actual_speedups[i], PLOT_FORMATS[i])
        for j, (size, speedup) in enumerate(zip(problem_sizes, actual_speedups[i])):
            # x = 1, the data point of omp-4, omp-16, omp-64 are too close, move them to avoid overlap
            if j in [1, 2, 3] and i in [1, 2, 3]:
                xytext = (0, 10 * (i - 2))
            else:
                xytext = (0, 5)
            plt.annotate(
                round(speedup, 1),
                (j, speedup),
                textcoords="offset points",
                xytext=xytext,
                ha="center",
                size=8,
                # b to blue, r to red, g to green, k to black, m to magenta, c to cyan, y to yellow
                color=COLOR_LIST[i],
            )
    for i, ideal_speedup in enumerate(ideal_speedups):
        plt.plot(ideal_speedup, PLOT_FORMATS[i][0] + "--")

    # plt.xscale("log")
    # plt.yscale("log")
    # Show y label in log scale
    plt.yticks([1, 4, 16, 64])
    plt.gca().set_yticklabels([str(i) for i in [1, 4, 16, 64]])

    plt.xlabel("Matrix Size (N)")
    plt.ylabel("Speedup")

    # ideal speedups and actual speedups
    # plt.legend(["ideal speedup for omp-1", "ideal speedup for omp-4", "ideal speedup for omp-16", "ideal speedup for omp-64", "omp-1", "omp-4", "omp-16", "omp-64"], loc="best")
    # omit legend for ideal speedup
    plt.legend(["omp-4", "omp-16", "omp-64"], loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()

def save_blocked_speedup(
    plot_fname,
    plot_title,
    problem_sizes,
    blocked4_serial_runtime,
    blocked4_parallel_runtimes,
    blocked16_serial_runtime,
    blocked16_parallel_runtimes,
    num_threads,
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    plt.xticks(xlocs, problem_sizes)

    ideal_speedups = [[num_threads for i in range(len(problem_sizes))] for num_threads in [4, 16, 64]]
    actual_speedups = [
        [serial / parallel for serial, parallel in zip(blocked4_serial_runtime, parallel_runtime)]
        for parallel_runtime in blocked4_parallel_runtimes
    ] + [
        [serial / parallel for serial, parallel in zip(blocked16_serial_runtime, parallel_runtime)]
        for parallel_runtime in blocked16_parallel_runtimes
    ]

    for i in range(len(blocked4_parallel_runtimes) + len(blocked16_parallel_runtimes)):
        fmt = PLOT_FORMATS[i // 3][0] + PLOT_FORMATS[i % 3][1:]
        plt.plot(actual_speedups[i], fmt)
        for j, (size, speedup) in enumerate(zip(problem_sizes, actual_speedups[i])):
            # x = 1, the data point of omp-4, omp-16, omp-64 are too close, move them to avoid overlap
            if j in [1, 2, 3] and i in [1, 2, 3]:
                xytext = (0, 10 * (i - 2))
            else:
                xytext = (0, 5)
            plt.annotate(
                round(speedup, 1),
                (j, speedup),
                textcoords="offset points",
                xytext=xytext,
                ha="center",
                size=8,
                # b to blue, r to red, g to green, k to black, m to magenta, c to cyan, y to yellow
                color=COLOR_LIST[i // 3],
            )
    for i, ideal_speedup in enumerate(ideal_speedups):
        plt.plot(ideal_speedup, PLOT_FORMATS[i][0] + "--")

    # plt.xscale("log")
    # plt.yscale("log")
    # Show y label in log scale
    plt.yticks([1, 4, 16, 64])
    plt.gca().set_yticklabels([str(i) for i in [1, 4, 16, 64]])

    plt.xlabel("Matrix Size (N)")
    plt.ylabel("Speedup")

    # ideal speedups and actual speedups
    # plt.legend(["ideal speedup for omp-1", "ideal speedup for omp-4", "ideal speedup for omp-16", "ideal speedup for omp-64", "omp-1", "omp-4", "omp-16", "omp-64"], loc="best")
    # omit legend for ideal speedup
    plt.legend(["BMMCO-omp-4 (b=4)", "BMMCO-omp-16 (b=4)", "BMMCO-omp-64 (b=4)", "BMMCO-omp-4 (b=16)", "BMMCO-omp-16 (b=16)", "BMMCO-omp-64 (b=16)"], loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()

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

def save_figures():
    fname = f"data/FLOPS_DP.csv"
    df = pd.read_csv(fname, comment="#")
    basic_df = df[df['Method'] == 'Basic']
    blocked_df = df[df['Method'] == 'Blocked']
    cblas_df = df[df['Method'] == 'CBLAS']
    print(basic_df)
    print(blocked_df)
    print(cblas_df)

    var_names = list(df.columns)

    print("var names =", var_names)

    # split the df into individual vars
    problem_sizes = basic_df[basic_df["Threads"] == 1]["N"].tolist()
    basic_omp_1_time = basic_df[basic_df["Threads"] == 1]["RDTSC Runtime [s]"].tolist()
    basic_omp_4_time = basic_df[basic_df["Threads"] == 4]["RDTSC Runtime [s]"].tolist()
    basic_omp_16_time = basic_df[basic_df["Threads"] == 16]["RDTSC Runtime [s]"].tolist()
    basic_omp_64_time = basic_df[basic_df["Threads"] == 64]["RDTSC Runtime [s]"].tolist()

    # speedup
    save_basic_speedup(
        "Basic_Speedup.png",
        "Basic Speedup (omp-4, omp-16, omp-64)",
        problem_sizes,
        basic_omp_1_time,
        [basic_omp_4_time, basic_omp_16_time, basic_omp_64_time],
        64,
    )

    blocked4_df = blocked_df[blocked_df["Block Size"] == 4]
    blocked4_omp_1_time = blocked4_df[blocked4_df["Threads"] == 1]["RDTSC Runtime [s]"].tolist()
    blocked4_omp_4_time = blocked4_df[blocked4_df["Threads"] == 4]["RDTSC Runtime [s]"].tolist()
    blocked4_omp_16_time = blocked4_df[blocked4_df["Threads"] == 16]["RDTSC Runtime [s]"].tolist()
    blocked4_omp_64_time = blocked4_df[blocked4_df["Threads"] == 64]["RDTSC Runtime [s]"].tolist()

    blocked16_df = blocked_df[blocked_df["Block Size"] == 16]
    blocked16_omp_1_time = blocked16_df[blocked16_df["Threads"] == 1]["RDTSC Runtime [s]"].tolist()
    blocked16_omp_4_time = blocked16_df[blocked16_df["Threads"] == 4]["RDTSC Runtime [s]"].tolist()
    blocked16_omp_16_time = blocked16_df[blocked16_df["Threads"] == 16]["RDTSC Runtime [s]"].tolist()
    blocked16_omp_64_time = blocked16_df[blocked16_df["Threads"] == 64]["RDTSC Runtime [s]"].tolist()

    save_blocked_speedup(
        "Blocked_Speedup.png",
        "Blocked Speedup (omp-4, omp-16, omp-64)",
        problem_sizes,
        blocked4_omp_1_time,
        [blocked4_omp_4_time, blocked4_omp_16_time, blocked4_omp_64_time],
        blocked16_omp_1_time,
        [blocked16_omp_4_time, blocked16_omp_16_time, blocked16_omp_64_time],
        64,
    )


if __name__ == "__main__":
    save_figures()
    save_speedup_chrono_figure()


# EOF
