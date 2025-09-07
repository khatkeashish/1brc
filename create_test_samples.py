import numpy as np

CITIES = [
    "New York",
    "Los Angeles",
    "Chicago",
    "Houston",
    "Phoenix",
    "London",
    "Manchester",
    "Birmingham",
    "Liverpool",
    "Edinburgh",
    "Paris",
    "Marseille",
    "Lyon",
    "Toulouse",
    "Nice",
    "Berlin",
    "Munich",
    "Frankfurt",
    "Hamburg",
    "Cologne",
    "Rome",
    "Milan",
    "Naples",
    "Turin",
    "Florence",
    "Madrid",
    "Barcelona",
    "Valencia",
    "Seville",
    "Bilbao",
    "Lisbon",
    "Porto",
    "Braga",
    "Faro",
    "Coimbra",
    "Tokyo",
    "Osaka",
    "Kyoto",
    "Nagoya",
    "Fukuoka",
    "Beijing",
    "Shanghai",
    "Shenzhen",
    "Guangzhou",
    "Chengdu",
    "Seoul",
    "Busan",
    "Incheon",
    "Daegu",
    "Daejeon",
    "Mumbai",
    "Delhi",
    "Bangalore",
    "Hyderabad",
    "Chennai",
    "Cairo",
    "Alexandria",
    "Giza",
    "Luxor",
    "Aswan",
    "Johannesburg",
    "Cape Town",
    "Durban",
    "Pretoria",
    "Port Elizabeth",
    "Sydney",
    "Melbourne",
    "Brisbane",
    "Perth",
    "Adelaide",
    "Toronto",
    "Vancouver",
    "Montreal",
    "Calgary",
    "Ottawa",
    "Mexico City",
    "Guadalajara",
    "Monterrey",
    "Puebla",
    "Cancún",
    "Buenos Aires",
    "Córdoba",
    "Rosario",
    "Mendoza",
    "La Plata",
    "São Paulo",
    "Rio de Janeiro",
    "Brasília",
    "Salvador",
    "Recife",
    "Moscow",
    "Saint Petersburg",
    "Novosibirsk",
    "Yekaterinburg",
    "Kazan",
    "Istanbul",
    "Ankara",
    "Izmir",
    "Antalya",
    "Bursa",
]


def get_random_float(start=-50.0, end=50.0, seed=42):
    rng = np.random.default_rng(seed)
    return float(rng.uniform(start, end))


def random_array(m, n, start=-10.0, end=50.0, seed=42):
    """
    Create an m x n NumPy array of random floats within [start, end].

    :param m: Number of rows
    :param n: Number of columns
    :param start: Minimum value
    :param end: Maximum value
    :param seed: Random seed for reproducibility
    :return: NumPy array of shape (m, n)
    """
    rng = np.random.default_rng(seed)
    return start + (end - start) * rng.random((m, n))


def write_to_txt(filename, text):
    with open(filename, "w", encoding="utf-8") as file:
        file.write(text)


def create_sample_data():
    n_measurements = 1_000_000
    n_cities = len(CITIES)

    measurements = random_array(n_cities, n_measurements)

    sample_txt = ""
    data_dict = {}
    for city, data in zip(CITIES, measurements):
        data_dict[city] = data

        for v in data:
            sample_txt += f"{city};{v}\n"

    write_to_txt("test_sample.txt", sample_txt)

    result_txt = ""
    for city, data in data_dict.items():
        result_txt += f"{city};{np.mean(data)};{np.min(data)};{np.max(data)}\n"

    write_to_txt("test_sample_results_truth.txt", result_txt)


def main():
    create_sample_data()


if __name__ == "__main__":
    main()
