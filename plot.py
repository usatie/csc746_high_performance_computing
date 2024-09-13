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

RESULT_DIR = "images/"
PLOT_FORMATS = [
    "r-o",
    "b-x",
    "g-^",
    "k-s",
    "m-v",
    "c-+",
]


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
    plt.yscale("log")

    plt.xlabel("Problem Sizes")
    plt.ylabel("Elapsed Time")

    plt.legend(var_names, loc="best")

    plt.grid(axis="both")

    # save the figure before trying to show the plot
    plt.savefig(RESULT_DIR + plot_fname, dpi=300)
    plt.clf()


def save_mflops(
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
        code_time = code_times[i]
        mflops = [
            2 * pow(problem_sizes[j], 3) / code_time[j] / 1_000_000
            for j in range(len(problem_sizes))
        ]
        plt.plot(mflops, PLOT_FORMATS[i])

    # for i in range(len(problem_sizes)):
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
    problem_sizes = [int(x.replace(",", "")) for x in problem_sizes]
    blas_time = df[var_names[1]].values.tolist()
    basic_time = df[var_names[2]].values.tolist()
    code3_time = df[var_names[3]].values.tolist()
    code4_time = df[var_names[4]].values.tolist()
    code5_time = df[var_names[5]].values.tolist()
    code6_time = df[var_names[6]].values.tolist()

    save_elapsed_time(
        "Runtime.png",
        "Runtime",
        problem_sizes,
        [
            blas_time,
            basic_time,
            code3_time,
            code4_time,
            code5_time,
            code6_time,
        ],
        var_names[1:],
    )
    save_mflops(
        "Basic-MM-and-CBLAS_MFLOPs.png",
        "MFLOP/s",
        problem_sizes,
        [
            blas_time,
            basic_time,
        ],
        var_names[1:3],
    )
    save_mflops(
        "BMMCO-and-CBLAS_MFLOPs.png",
        "MFLOP/s",
        problem_sizes,
        [
            blas_time,
            code3_time,
            code4_time,
            code5_time,
            code6_time,
        ],
        var_names[1:2] + var_names[3:],
    )


if __name__ == "__main__":
    save_figures()


# EOF
