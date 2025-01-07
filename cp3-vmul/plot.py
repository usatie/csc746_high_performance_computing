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


def get_exponent(n):
    exponent = 0
    while n > 1:
        n >>= 1
        exponent += 1
    return exponent


def save_elapsed_time(
    plot_fname,
    plot_title,
    problem_sizes,
    code_times,
    var_names,
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    plt.xticks(xlocs, problem_sizes)

    for i in range(len(code_times)):
        plt.plot(code_times[i], PLOT_FORMATS[i])

    # plt.xscale("log")
    # plt.yscale("log")

    plt.xlabel("Matrix Size (N)")
    plt.ylabel("Elapsed Time")

    plt.legend(var_names, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()


def save_speedup(
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

    ideal_speedups = [[num_threads for i in range(len(problem_sizes))] for num_threads in [1, 4, 16, 64]]
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
    plt.yscale("log")
    # Show y label in log scale
    plt.yticks([1, 2, 4, 8, 16, 32, 64])
    plt.gca().set_yticklabels([str(i) for i in [1, 2, 4, 8, 16, 32, 64]])

    plt.xlabel("Matrix Size (N)")
    plt.ylabel("Speedup")

    # ideal speedups and actual speedups
    # plt.legend(["ideal speedup for omp-1", "ideal speedup for omp-4", "ideal speedup for omp-16", "ideal speedup for omp-64", "omp-1", "omp-4", "omp-16", "omp-64"], loc="best")
    # omit legend for ideal speedup
    plt.legend(["omp-1", "omp-4", "omp-16", "omp-64"], loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()

def calc_mvmul_mflops(elapsed_time, problem_size, gflops=False):
    if gflops:
        return 2 * pow(problem_size, 2) / elapsed_time / 1_000_000_000
    else:
        return 2 * pow(problem_size, 2) / elapsed_time / 1_000_000

def save_mflops(
    plot_fname,
    plot_title,
    problem_sizes,
    code_times,
    var_names,
    gflops=False,
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    plt.xticks(xlocs, problem_sizes)

    code_mflops = [
        [calc_mvmul_mflops(t, size, gflops) for (t, size) in zip(code_time, problem_sizes)]
        for code_time in code_times
    ]
    for mflops_arr, fmt in zip(code_mflops, PLOT_FORMATS):
        plt.plot(mflops_arr, fmt)
        for i, (size, mflops) in enumerate(zip(problem_sizes, mflops_arr)):
            # If there are too close data points, move one of them to avoid overlap
            # the distance should be scaled in log scale
            xytext = (0, 3)
            values = [
                math.log(mflops) - math.log(m[i]) for m in code_mflops if m[i] != mflops
            ]
            if len([v for v in values if v < 0.1 and v > 0]) > 0:
                xytext = (0, 10)
            plt.annotate(
                round(mflops, 1),
                (i, mflops),
                textcoords="offset points",
                xytext=xytext,
                ha="center",
                size=8,
            )

    # plt.xscale("log")
    # plt.yscale("log")

    plt.xlabel("Matrix Size (N)")
    if gflops:
        plt.ylabel("GFLOP/s")
    else:
        plt.ylabel("MFLOP/s")

    plt.legend(var_names, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()


# Memory Access: 2 * N + 2 * N^2
# double: 8 bytes
# Memory Bandwidth = [Memory Access] * 8 bytes / elapsed_time / 1_000_000_000
def calc_mvmul_memory_bw(elapsed_time, problem_size):
    return (
        (2 * problem_size + 2 * pow(problem_size, 2)) * 8 / elapsed_time / 1_000_000_000
    )


def save_memory_bandwidth(
    plot_fname,
    plot_title,
    problem_sizes,
    code_times,
    var_names,
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    plt.xticks(xlocs, problem_sizes)

    code_memory_bw = [
        [calc_mvmul_memory_bw(t, size) for (t, size) in zip(code_time, problem_sizes)]
        for code_time in code_times
    ]
    # print bandwidth with label(var_names)
    for i, var_name in enumerate(var_names):
        formatted_memory_bw = [f"{bw:.2f} GB/s ({100*bw/204.8:.1f}%)" for bw in code_memory_bw[i]]
        print(f"{var_name}: {formatted_memory_bw}")
    for memory_bw_arr, fmt in zip(code_memory_bw, PLOT_FORMATS):
        plt.plot(memory_bw_arr, fmt)
        for i, (size, memory_bw) in enumerate(zip(problem_sizes, memory_bw_arr)):
            # If there are too close data points, move one of them to avoid overlap
            # the distance should be scaled in log scale
            xytext = (0, 3)
            values = [
                math.log(memory_bw) - math.log(m[i])
                for m in code_memory_bw
                if m[i] != memory_bw
            ]
            if len([v for v in values if v < 0.1 and v > 0]) > 0:
                xytext = (0, 10)
            plt.annotate(
                round(memory_bw, 1),
                (i, memory_bw),
                textcoords="offset points",
                xytext=xytext,
                ha="center",
                size=8,
            )

    # plt.xscale("log")
    # plt.yscale("log")

    plt.xlabel("Matrix Size (N)")
    plt.ylabel("Memory Bandwidth (GB/s)")

    plt.legend(var_names, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()


def save_figures():
    fname = f"data/runtime.csv"
    df = pd.read_csv(fname, comment="#")
    print(df)

    var_names = list(df.columns)

    print("var names =", var_names)

    # split the df into individual vars

    problem_sizes = df[var_names[0]].values.tolist()
    blas_time = df[var_names[1]].values.tolist()
    basic_time = df[var_names[2]].values.tolist()
    vectorized_time = df[var_names[3]].values.tolist()
    openmp_1_time = df[var_names[4]].values.tolist()
    openmp_4_time = df[var_names[5]].values.tolist()
    openmp_16_time = df[var_names[6]].values.tolist()
    openmp_64_time = df[var_names[7]].values.tolist()

    save_elapsed_time(
        "Runtime.png",
        "Runtime",
        problem_sizes,
        [
            blas_time,
            basic_time,
            vectorized_time,
            openmp_1_time,
            openmp_4_time,
            openmp_16_time,
            openmp_64_time,
        ],
        var_names[1:],
    )
    save_mflops(
        "MFLOPs-all.png",
        "MFLOP/s",
        problem_sizes,
        [
            blas_time,
            basic_time,
            vectorized_time,
            openmp_1_time,
            openmp_4_time,
            openmp_16_time,
            openmp_64_time,
        ],
        var_names[1:],
    )
    # MFLOPs of blas, basic, vectorized
    save_mflops(
        "GFLOPs-serial.png",
        "GFLOP/s (CBLAS, Basic, Vectorized)",
        problem_sizes,
        [blas_time, basic_time, vectorized_time],
        var_names[1:4],
        gflops=True,
    )
    save_memory_bandwidth(
        "Memory-bandwidth.png",
        "Memory Bandwidth (GB/s)",
        problem_sizes,
        [
            blas_time,
            basic_time,
            vectorized_time,
            openmp_1_time,
            openmp_4_time,
            openmp_16_time,
            openmp_64_time,
        ],
        var_names[1:],
    )

    # MFLOPs of best-openmp and CBLAS
    save_mflops(
        "GFLOPs-parallel.png",
        "GFLOP/s (CBLAS, omp-16)",
        problem_sizes,
        [blas_time, openmp_16_time],
        [var_names[1], var_names[6]],
        gflops=True,
    )
    # speedup
    save_speedup(
        "Speedup.png",
        "Speedup (ideal, omp-4, omp-16, omp-64)",
        problem_sizes,
        vectorized_time,
        [openmp_1_time, openmp_4_time, openmp_16_time, openmp_64_time],
        64,
    )


if __name__ == "__main__":
    save_figures()


# EOF
