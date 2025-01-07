"""
Generate a speedup chart for various scene complexities (Objects.size()) from parsed CSV data.

This script:
1. Loads the CSV data (containing Objects.size(), Threads, and Elapsed Time).
2. Filters data for the specified complexities: 8, 1028, 16388.
3. Computes speedup = (Time at 1 thread) / (Time at N threads) for each complexity.
4. Plots these speedups along with an ideal speedup line.

Dependencies: pandas, matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt
import os

RESULT_DIR = "images/"
PLOT_FORMATS = ["r-o", "b-x", "g-^", "m-+", "c-*", "y-s", "k-d"]


def load_data(file_path):
    df = pd.read_csv(file_path)
    return df


def compute_speedup_for_schedule(df, schedule):
    # Filter rows for the given complexity
    (kind, chunk_size, collapse) = schedule
    df = df[df["Schedule"] == kind]
    df = df[df["Chunk Size"] == chunk_size]
    cdf = df[df["Collapse"] == collapse].copy()
    # Ensure sorting by Threads to maintain a consistent x-axis
    cdf.sort_values(by="Threads", inplace=True)

    # Find the elapsed time at 1 thread
    base_time = cdf[cdf["Threads"] == 1]["Elapsed Time"].values[0]
    # Compute speedup
    cdf["Speedup"] = base_time / cdf["Elapsed Time"]
    return cdf[["Threads", "Speedup"]]


def plot_speedup_chart(df, schedules, title="Speedup Comparison"):
    # Prepare plot
    plt.title(title)
    plt.xlabel("Number of Threads")
    plt.ylabel("Speedup")

    # Compute speedups for each complexity
    lines = []
    labels = []

    # We'll keep track of the union of all thread counts across complexities
    # so the ideal line can be drawn consistently.
    all_threads = set()

    for idx, schedule in enumerate(schedules):
        (kind, chunk_size, collapse) = schedule
        speedup_df = compute_speedup_for_schedule(df, schedule)
        threads = speedup_df["Threads"].tolist()
        speedups = speedup_df["Speedup"].tolist()
        print(speedups)

        all_threads.update(threads)
        if kind == "Static":
            label = "Static"
        else:
            label = f"{kind}-{chunk_size} (Collapse)"

        plt.plot(
            threads,
            speedups,
            PLOT_FORMATS[idx],
            label=label,
        )

    # Sort threads to create a clean ideal speedup line
    all_threads = sorted(list(all_threads))

    # Ideal speedup is equal to the number of threads (relative to 1 thread),
    # i.e., ideal speedup = threads_count.
    # Since our baseline is 1-thread performance, the ideal speedup line = x.
    # ideal_speedup = all_threads  # since at 1 thread speedup=1, at 2 threads=2, etc.
    # plt.plot(all_threads, ideal_speedup, "k--", label="Ideal Speedup")

    # x-axis ticks should be equally spaced, i.e. log scale
    plt.xscale("log", base=2)
    plt.xticks(all_threads, all_threads)

    plt.grid(True)
    plt.legend(loc="best")

    os.makedirs(RESULT_DIR, exist_ok=True)
    plt.savefig(
        os.path.join(RESULT_DIR, "load_imbalance_speedup_comparison.png"), dpi=300
    )
    plt.clf()


if __name__ == "__main__":
    data_file = (
        "data/load-imbalance.csv"  # Update this to your actual parsed CSV file path
    )
    df = load_data(data_file)
    df = df[df["Loop Order"] == "YX"]

    # Create chart with the three complexities and ideal speedup
    schedules_of_interest = [
        ("Static", 0, "disabled"),
        ("Dynamic", 1, "enabled"),
        ("Dynamic", 16, "enabled"),
    ]
    plot_speedup_chart(
        df, schedules_of_interest, title="Speedup for Different Schedules"
    )
