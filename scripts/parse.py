import re
import csv

# Read the input data from stdin
input_data = ""
while True:
    try:
        line = input()
        input_data += line + "\n"
    except EOFError:
        break

# Regex patterns to extract needed fields:
# Note: The "Depth: 32Sphere Grid Size:" part does not have a comma, so we handle it carefully.
info_pattern = re.compile(
    r"Sample:\s*(?P<sample>\d+),\s*Width:\s*(?P<width>\d+),\s*Height:\s*(?P<height>\d+),\s*Depth:\s*(?P<depth>\d+)Sphere Grid Size:\s*(?P<grid_size>\d+),\s*Collapse:\s*(?P<collapse>\S+),\s*Loop Order:\s*(?P<loop_order>\S+),\s*Threads:\s*(?P<threads>\d+),\s*Schedule:\s*(?P<schedule>\S+),\s*chunk size:\s*(?P<chunk_size>\d+)"
)
objects_pattern = re.compile(r"Objects.size\(\):\s*(?P<objects>\d+)")
time_pattern = re.compile(r"Elapsed time:\s*(?P<time>\d+\.\d+)")

blocks = input_data.strip().split(
    "------------------------------------------------------------------------------------------------------------------------"
)
experiments = []

for block in blocks:
    block = block.strip()
    if not block:
        continue
    info_match = info_pattern.search(block)
    obj_match = objects_pattern.search(block)
    time_match = time_pattern.search(block)
    if info_match and time_match:
        exp_data = {
            "Sample": info_match.group("sample"),
            "Width": info_match.group("width"),
            "Height": info_match.group("height"),
            "Depth": info_match.group("depth"),
            "Sphere Grid Size": info_match.group("grid_size"),
            "Collapse": info_match.group("collapse"),
            "Loop Order": info_match.group("loop_order"),
            "Threads": info_match.group("threads"),
            "Schedule": info_match.group("schedule"),
            "Chunk Size": info_match.group("chunk_size"),
            "Elapsed Time": time_match.group("time"),
        }
        if obj_match:
            exp_data["Objects.size()"] = obj_match.group("objects")
        experiments.append(exp_data)

# Write to CSV
fieldnames = [
    "Sample",
    "Width",
    "Height",
    "Depth",
    "Sphere Grid Size",
    "Collapse",
    "Loop Order",
    "Threads",
    "Schedule",
    "Chunk Size",
    "Objects.size()",
    "Elapsed Time",
]

with open("data/parsed_data.csv", "w", newline="") as csvfile:
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()
    for exp in experiments:
        writer.writerow(exp)

print("CSV file 'parsed_data.csv' created successfully.")
