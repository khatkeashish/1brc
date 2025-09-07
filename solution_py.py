import time
from dataclasses import dataclass


def write_to_txt(filename, text):
    """
    Write a string to a text file.

    :param filename: Name of the file (e.g. "output.txt")
    :param text: The string content to write
    """
    with open(filename, "w", encoding="utf-8") as file:
        file.write(text)


@dataclass
class CityResult:
    mean: float = 0.0
    min: float = float("inf")
    max: float = float("-inf")
    counter: int = 0


def main1(filename):
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            city, value = line.strip().split(";")
            value = float(value)

            if city not in results:
                results[city] = CityResult()

            results[city].min = min(results[city].min, value)
            results[city].max = max(results[city].max, value)
            sum = results[city].mean * results[city].counter

            results[city].counter += 1
            results[city].mean = (sum + value) / results[city].counter

    result_txt = ""
    for city, city_data in results.items():
        result_txt += f"{city};{city_data.mean};{city_data.min};{city_data.max}\n"

    write_to_txt("test_sample_results_calculated.txt", result_txt)


def main2(filename):
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            city, value = line.strip().split(";")
            value = float(value)

            if city not in results:
                results[city] = CityResult()

            city_result = results[city]

            city_result.min = min(city_result.min, value)
            city_result.max = max(city_result.max, value)
            sum = city_result.mean * city_result.counter

            city_result.counter += 1
            city_result.mean = (sum + value) / city_result.counter

            results[city] = city_result

    result_txt = ""
    for city, city_data in results.items():
        result_txt += f"{city};{city_data.mean};{city_data.min};{city_data.max}\n"

    write_to_txt("test_sample_results_calculated.txt", result_txt)


def main3(filename):
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            city, value = line.strip().split(";")
            value = float(value)

            if city not in results:
                results[city] = {
                    "mean": 0,
                    "min": float("inf"),
                    "max": float("-inf"),
                    "counter": 0,
                }

            results[city]["min"] = min(results[city]["min"], value)
            results[city]["max"] = max(results[city]["max"], value)
            sum = results[city]["mean"] * results[city]["counter"]

            results[city]["counter"] += 1
            results[city]["mean"] = (sum + value) / results[city]["counter"]

    result_txt = ""
    for city, city_data in results.items():
        result_txt += (
            f"{city};{city_data['mean']};{city_data['min']};{city_data['max']}\n"
        )

    write_to_txt("test_sample_results_calculated.txt", result_txt)


def main4(filename):
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            city, value = line.strip().split(";")
            value = float(value)

            if city not in results:
                results[city] = {
                    "mean": 0,
                    "min": float("inf"),
                    "max": float("-inf"),
                    "counter": 0,
                }

            city_result = results[city]
            city_result["min"] = min(city_result["min"], value)
            city_result["max"] = max(city_result["max"], value)
            sum = city_result["mean"] * city_result["counter"]

            city_result["counter"] += 1
            city_result["mean"] = (sum + value) / city_result["counter"]

            results[city] = city_result

    result_txt = ""
    for city, city_data in results.items():
        result_txt += (
            f"{city};{city_data['mean']};{city_data['min']};{city_data['max']}\n"
        )

    write_to_txt("test_sample_results_calculated.txt", result_txt)


class CityResultSlots:
    __slots__ = ("mean", "min", "max", "counter")

    def __init__(
        self,
        mean=0.0,
        min_val=float("inf"),
        max_val=float("-inf"),
        counter=0,
    ):
        self.mean = mean
        self.min = min_val
        self.max = max_val
        self.counter = counter


def main5(filename):
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            city, value = line.strip().split(";")
            value = float(value)

            if city not in results:
                results[city] = CityResultSlots()

            city_result = results[city]

            city_result.min = min(city_result.min, value)
            city_result.max = max(city_result.max, value)
            sum = city_result.mean * city_result.counter

            city_result.counter += 1
            city_result.mean = (sum + value) / city_result.counter

            results[city] = city_result

    result_txt = ""
    for city, city_data in results.items():
        result_txt += f"{city};{city_data.mean};{city_data.min};{city_data.max}\n"

    write_to_txt("test_sample_results_calculated.txt", result_txt)


@dataclass(slots=True)
class CityResultDataclassesSlots:
    mean: float = 0.0
    min: float = float("inf")
    max: float = float("-inf")
    counter: int = 0
    sum: int = 0


def main6(filename):
    results = {}
    with open(filename, "r", encoding="utf-8") as file:
        for line in file:
            # Strip newline, then split on ';'
            city, value = line.strip().split(";")
            value = float(value)

            city_result = results.get(city, CityResultDataclassesSlots())

            city_result.min = min(city_result.min, value)
            city_result.max = max(city_result.max, value)
            city_result.sum += value

            city_result.counter += 1

            results[city] = city_result

    result_txt = ""
    for city, city_result in results.items():
        city_result.mean = city_result.sum / city_result.counter
        result_txt += f"{city};{city_result.mean};{city_result.min};{city_result.max}\n"

    write_to_txt("test_sample_results_calculated.txt", result_txt)


from csv import reader


def main7(filename):
    results = {}

    with open(filename, "r", encoding="utf-8", newline="") as f:
        rdr = reader(f, delimiter=";")
        res = results  # local alias is a tiny win

        for city, v_str in rdr:
            v = float(v_str)
            cr = res.get(city)

            if cr is None:
                # initialize from first observation
                cr = CityResult(mean=v, min=v, max=v, counter=1)
                res[city] = cr
            else:
                # fast min/max without extra temporaries
                if v < cr.min:
                    cr.min = v
                if v > cr.max:
                    cr.max = v
                # Welford update for the mean
                cnt1 = cr.counter + 1
                cr.mean += (v - cr.mean) / cnt1
                cr.counter = cnt1

    # Build output efficiently
    lines = [f"{city};{cr.mean};{cr.min};{cr.max}" for city, cr in results.items()]
    write_to_txt("test_sample_results_calculated.txt", "\n".join(lines) + "\n")


import pandas as pd


def main8(filename):
    df = pd.read_csv(
        filename,
        sep=";",
        names=["city", "value"],
        header=None,
        dtype={"city": "category", "value": "f8"},
        # engine="c",  # default; emphasizes C parser
        engine="pyarrow",  # columnar parsing
        dtype_backend="pyarrow",  # store columns as Arrow arrays
    )
    agg = df.groupby("city", observed=True)["value"].agg(["mean", "min", "max"])
    agg.to_csv("test_sample_results_calculated.txt", sep=";", header=False)


import polars as pl


def main9(filename: str):
    df = pl.read_csv(
        filename,
        separator=";",
        has_header=False,
        new_columns=["city", "value"],
        schema_overrides={"city": pl.Utf8, "value": pl.Float64},
    )

    # Group + aggregate
    agg = df.group_by("city").agg(
        [
            pl.col("value").mean().alias("mean"),
            pl.col("value").min().alias("min"),
            pl.col("value").max().alias("max"),
        ]
    )

    # Write results
    agg.write_csv(
        "test_sample_results_calculated.txt",
        separator=";",
        include_header=False,
    )


if __name__ == "__main__":
    start = time.perf_counter()
    main6("test_sample.txt")
    end = time.perf_counter()
    print(f"\n\nExecution time: {end - start:.6f} seconds")
