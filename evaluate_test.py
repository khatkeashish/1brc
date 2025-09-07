import numpy as np


def read_and_split(filename):
    """
    Read a text file line by line and split each row on ';'.

    :param filename: Name of the file (e.g. "data.txt")
    :return: List of lists, where each inner list contains the split values
    """
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            parts = line.strip().split(";")
            results[parts[0]] = [float(s) for s in parts[1:]]
    return results


def main():
    truth = read_and_split("test_sample_results_truth.txt")
    calculated = read_and_split("test_sample_results_calculated.txt")
    cities = sorted(truth)

    fails = []
    for city in cities:
        mean_match = np.isclose(truth[city][0], calculated[city][0], atol=1e-5, rtol=0)
        min_match = np.isclose(truth[city][1], calculated[city][1], atol=1e-5, rtol=0)
        max_match = np.isclose(truth[city][2], calculated[city][2], atol=1e-5, rtol=0)

        if not (mean_match and min_match and max_match):
            print(
                f"Failed:: city: {city}; mean: {mean_match}; min: {min_match}; max: {max_match}"
            )
            fails.append(city)

    if fails:
        print("Fail")
    else:
        print("Success")


if __name__ == "__main__":
    main()
