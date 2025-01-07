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

result_dir = "images/"

def get_exponent(n):
    exponent = 0
    while n > 1:
        n >>= 1
        exponent += 1
    return exponent


def save_elapsed_time(
    plot_fname, plot_title, problem_sizes, code1_time, code2_time, code3_time, var_names
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    problem_sizes_exponent = [rf"$2^{{{get_exponent(x)}}}$" for x in problem_sizes]

    plt.xticks(xlocs, problem_sizes_exponent)

    plt.plot(code1_time, "r-o")
    plt.plot(code2_time, "b-x")
    plt.plot(code3_time, "g-^")

    # plt.xscale("log")
    plt.yscale("log")

    plt.xlabel("Problem Sizes")
    plt.ylabel("Elapsed Time")

    varNames = [var_names[1], var_names[2], var_names[3]]
    plt.legend(varNames, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(result_dir + plot_fname, dpi=300)
    plt.clf()


def save_mflops(
    plot_fname, plot_title, problem_sizes, code1_time, code2_time, code3_time, var_names
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    problem_sizes_exponent = [rf"$2^{{{get_exponent(x)}}}$" for x in problem_sizes]

    plt.xticks(xlocs, problem_sizes_exponent)

    code1_mflops = [
        problem_sizes[i] / code1_time[i] / 1_000_000 for i in range(len(problem_sizes))
    ]
    code2_mflops = [
        problem_sizes[i] / code2_time[i] / 1_000_000 for i in range(len(problem_sizes))
    ]
    code3_mflops = [
        problem_sizes[i] / code3_time[i] / 1_000_000 for i in range(len(problem_sizes))
    ]

    plt.plot(code1_mflops, "r-o")
    plt.plot(code2_mflops, "b-x")
    plt.plot(code3_mflops, "g-^")

    #for i in range(len(problem_sizes)):
    #    for mflops in [code1_mflops, code2_mflops, code3_mflops]:
    #        plt.annotate(
    #                round(mflops[i], 1),
    #                (i, mflops[i]),
    #                textcoords="offset points",
    #                xytext=(0,3),
    #                ha='center',
    #                size=8,
    #                )

    # plt.xscale("log")
    plt.yscale("log")

    plt.xlabel("Problem Sizes")
    plt.ylabel("MFLOP/s")

    varNames = [var_names[1], var_names[2], var_names[3]]
    plt.legend(varNames, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(result_dir + plot_fname, dpi=300)
    plt.clf()


def save_memory_bandwidth_utilization(
    plot_fname, plot_title, problem_sizes, code1_time, code2_time, code3_time, var_names
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    problem_sizes_exponent = [rf"$2^{{{get_exponent(x)}}}$" for x in problem_sizes]

    plt.xticks(xlocs, problem_sizes_exponent)

    code1_bandwidth = [0 for i in range(len(problem_sizes))]
    code2_bandwidth = [
        problem_sizes[i] * 8 / code2_time[i] / (204.8 * 1_000_000_000) * 100
        for i in range(len(problem_sizes))
    ]
    code3_bandwidth = [
        problem_sizes[i] * 8 / code3_time[i] / (204.8 * 1_000_000_000) * 100
        for i in range(len(problem_sizes))
    ]

    plt.plot(code1_bandwidth, "r-o")
    plt.plot(code2_bandwidth, "b-x")
    plt.plot(code3_bandwidth, "g-^")

    for i in range(len(problem_sizes)):
        for bandwidth in [code2_bandwidth, code3_bandwidth]:
            plt.annotate(
                    f"{bandwidth[i]:.2f}%",
                    (i, bandwidth[i]),
                    textcoords="offset points",
                    xytext=(0,3),
                    ha='center',
                    size=8,
                    )

    # plt.xscale("log")
    # plt.yscale("log")

    plt.xlabel("Problem Sizes")
    plt.ylabel("% of memory bandwidth utilized")

    varNames = [var_names[1], var_names[2], var_names[3]]
    plt.legend(varNames, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(result_dir + plot_fname, dpi=300)
    plt.clf()


def save_memory_latency(
    plot_fname, plot_title, problem_sizes, code1_time, code2_time, code3_time, var_names
):
    plt.title(plot_title)

    xlocs = [i for i in range(len(problem_sizes))]

    problem_sizes_exponent = [rf"$2^{{{get_exponent(x)}}}$" for x in problem_sizes]

    plt.xticks(xlocs, problem_sizes_exponent)

    code1_latency = [0 for i in range(len(problem_sizes))]
    code2_latency = [
        code2_time[i] / problem_sizes[i] * 1_000_000_000
        for i in range(len(problem_sizes))
    ]
    code3_latency = [
        code3_time[i] / problem_sizes[i] * 1_000_000_000
        for i in range(len(problem_sizes))
    ]

    plt.plot(code1_latency, "r-o")
    plt.plot(code2_latency, "b-x")
    plt.plot(code3_latency, "g-^")

    for i in range(len(problem_sizes)):
        for latency in [code2_latency, code3_latency]:
            plt.annotate(
                    f"{latency[i]:.2f}",
                    (i, latency[i]),
                    textcoords="offset points",
                    xytext=(0,5),
                    ha='center',
                    size=8,
                    )

    # plt.xscale("log")
    # plt.yscale("log")

    plt.xlabel("Problem Sizes")
    plt.ylabel("Memory Latency (ns)")

    varNames = [var_names[1], var_names[2], var_names[3]]
    plt.legend(varNames, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(result_dir + plot_fname, dpi=300)
    plt.clf()


def save_figures(flag, title_desc):
    fname = f"data/Elapsed_Time_{flag}.csv"
    df = pd.read_csv(fname, comment="#")
    print(df)

    var_names = list(df.columns)

    print("var names =", var_names)

    # split the df into individual vars
    # assumption: column order - 0=problem size, 1=blas time, 2=basic time

    problem_sizes = df[var_names[0]].values.tolist()
    problem_sizes = [int(x.replace(",", "")) for x in problem_sizes]
    code1_time = df[var_names[1]].values.tolist()
    code2_time = df[var_names[2]].values.tolist()
    code3_time = df[var_names[3]].values.tolist()

    save_elapsed_time(
        f"Elapsed_Time_{flag}.png",
        f"Elapsed Time {title_desc}",
        problem_sizes,
        code1_time,
        code2_time,
        code3_time,
        var_names,
    )
    save_mflops(
        f"MFLOPs_{flag}.png",
        f"MFLOP/s {title_desc}",
        problem_sizes,
        code1_time,
        code2_time,
        code3_time,
        var_names,
    )
    save_memory_bandwidth_utilization(
        f"Bandwidth_{flag}.png",
        f"Memory Bandwidth Utlization (%) {title_desc}",
        problem_sizes,
        code1_time,
        code2_time,
        code3_time,
        var_names,
    )
    save_memory_latency(
        f"Latency_{flag}.png",
        f"Memory Latency (ns) {title_desc}",
        problem_sizes,
        code1_time,
        code2_time,
        code3_time,
        var_names,
    )


if __name__ == "__main__":
    save_figures("O0", "without optimization (with -O0)")
    save_figures("O3", "with optimization (with -O3)")


# EOF
